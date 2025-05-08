#include "arduino_secrets.h"

#define WATER_SOIL_AT 710
#define MIST_AT 40

#define TEMP_LOW_BOUND 75
#define TEMP_HIGH_BOUND 75

#include "pin_defs.h"
#include <SPI.h>
#include <WiFiNINA.h>
#include "Arduino_MKRIoTCarrier.h"
#include "SmoothingFilter.h"
#include "Logger.h"
#include <time.h>
#include "SensorReader.h"
#include <ArduinoJson.h>
#include <arduino-timer.h>

MKRIoTCarrier carrier;

// WiFi credentials.
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

int oneSec = 1000L;
int oneMin = oneSec * 60;
int fifteenMin = 15 * oneMin;

auto timer = timer_create_default(); // create a timer with default settings
SmoothingFilter brightnessFilter(20);

WiFiClient wifiClient;
BearSSLClient sslClient(wifiClient);
AwsIotLogger awsIotLogger(sslClient);
Logger logger(Serial, awsIotLogger);
SensorReader sensors(carrier, logger, timer);

extern "C" char *sbrk(int incr);

int freeMemory()
{
  char stack_dummy = 0;
  return &stack_dummy - sbrk(0);
}

unsigned long getTime()
{
  return WiFi.getTime();
}

bool sendData(void *)
{
  Serial.println("sendData");
  sensors.updateAll();

  JsonDocument doc;
  doc["temp"] = sensors.getTemperature();
  doc["hum"] = sensors.getHumidity();
  doc["brit"] = sensors.getBrightness();
  doc["co2"] = sensors.getCo2();
  for (int i = 0; i < NUM_SOIL_SENSORS; i++)
  {
    String key = "soil" + String(i);
    doc[key] = sensors.getSoilDryness(i);
  }
  doc.shrinkToFit();

  logger.build()
      .cloud(true)
      .topic("greenhouse/data/env")
      .data(doc)
      .log();
  return true;
}

void sendData()
{
  sendData(nullptr);
}

bool checkBtns(void *)
{
  carrier.Buttons.update();
  if (carrier.Buttons.onTouchDown(TOUCH0))
  {
    sendData();
  }
  return true;
}

bool waterIfNeeded(void *)
{
  sensors.updateSoilDryness();
  if (digitalRead(PUMP_1) == LOW && sensors.getSoilDryness(0) > WATER_SOIL_AT)
  {
    logger.build()
        .serial(true)
        .cloud(true)
        .notification(true)
        .notifType("pump_1_watering")
        .data("Watering")
        .log();
    digitalWrite(PUMP_1, HIGH);
  }
  else if (digitalRead(PUMP_1) == HIGH && sensors.getSoilDryness(0) < WATER_SOIL_AT)
  {
    logger.build()
        .serial(true)
        .cloud(true)
        .notification(true)
        .notifType("pump_1_watering")
        .data("Stopping watering")
        .log();
    digitalWrite(PUMP_1, LOW);
  }
  return true;
}

bool humidifyIfNeeded(void *)
{
  sensors.updateHumidity();
  bool mister_on = digitalRead(MISTER) == HIGH;
  if (!mister_on && sensors.getHumidity() < MIST_AT)
  {
    logger.build()
        .serial(true)
        .cloud(true)
        .notification(true)
        .notifType("mister")
        .data("Mister On")
        .log();
    digitalWrite(MISTER, HIGH);
  }
  else if (mister_on && sensors.getHumidity() > MIST_AT)
  {
    logger.build()
        .serial(true)
        .cloud(true)
        .notification(true)
        .notifType("mister")
        .data("Mister Off")
        .log();
    digitalWrite(MISTER, LOW);
  }
  return true;
}

bool fanIfNeeded(void *)
{
  bool fan_on = digitalRead(FAN_1) == HIGH;
  sensors.updateTemperature();
  int temp = sensors.getTemperature();
  if (!fan_on && temp > TEMP_HIGH_BOUND)
  {
    logger.build()
        .serial(true)
        .cloud(true)
        .notification(true)
        .notifType("fan")
        .dataf("Fan On %sF", String(temp).c_str())
        .log();
    digitalWrite(FAN_1, HIGH);
  }
  else if (fan_on && temp < TEMP_LOW_BOUND)
  {
    logger.build()
        .serial(true)
        .cloud(true)
        .notification(true)
        .notifType("fan")
        .dataf("Fan Off %sF", String(temp).c_str())
        .log();
    digitalWrite(FAN_1, LOW);
  }
  return true;
}

void setupTime()
{
  unsigned long epoch = 0;
  int retries = 0;
  while ((epoch = WiFi.getTime()) == 0 && retries < 10)
  {
    Serial.println("Waiting for NTP time...");
    delay(1000);
    retries++;
  }

  if (epoch == 0)
  {
    Serial.println("Failed to retrieve time from NTP server.");
    return;
  }

  Serial.print("Epoch time: ");
  Serial.println(epoch);
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  while (!Serial && millis() < 5000)
  {
    // Wait for Serial Monitor to connect (max 5 seconds)
  }
  delay(2000);

  if (!ECCX08.begin())
  {
    Serial.println("No ECCX08 present!");
    while (1)
      ;
  }

  sslClient.setEccSlot(0, awsIotCertificatePemCrt);

  Serial.println("Starting carrier...");
  // Begin carrier hardware
  if (!carrier.begin())
  {
    Serial.println("Failed to initialize carrier!");
    while (1)
      ;
  }

  carrier.noCase();
  carrier.display.enableDisplay(false);

  Serial.println("Setting Pin modes...");
  // pinMode(PUMP_1, OUTPUT);
  // pinMode(MISTER, OUTPUT);
  // pinMode(FAN_1, OUTPUT);
  sensors.setup();

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected.");

  setupTime();
  ArduinoBearSSL.onGetTime(getTime);

  timer.every(fifteenMin, sendData);
  timer.every(100, checkBtns);
  timer.every(oneSec, [](void *) -> bool
              { sensors.sampleBrightness(); return true; });
  timer.every(oneSec, waterIfNeeded);
  timer.every(oneSec, fanIfNeeded);
  timer.every(oneSec, humidifyIfNeeded);

  logger.build()
      .serial(true)
      .cloud(true)
      .data("Setup complete")
      .log();
  sendData();
}

void loop()
{
  timer.tick();
  awsIotLogger.loop();
}

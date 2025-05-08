#include "arduino_secrets.h"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#define WATER_SOIL_AT 710
#define MIST_AT 40

#define TEMP_LOW_BOUND 75
#define TEMP_HIGH_BOUND 75

#include "pin_defs.h"
#include <SPI.h>
#include <WiFiNINA.h>
#include <BlynkSimpleWiFiNINA.h>
#include "Arduino_MKRIoTCarrier.h"
#include "SmoothingFilter.h"
#include "Logger.h"
#include <time.h>
#include "SensorReader.h"
#include <ArduinoJson.h>

MKRIoTCarrier carrier;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

int oneSec = 1000L;
int oneMin = oneSec * 60;
int fifteenMin = 15 * oneMin;

BlynkTimer timer;
SmoothingFilter brightnessFilter(20);

WiFiClient wifiClient;
BearSSLClient sslClient(wifiClient);
AwsIotLogger awsIotLogger(sslClient);
Logger logger(Serial, awsIotLogger);
SensorReader sensors(carrier, logger, timer);

unsigned long getTime()
{
  return WiFi.getTime();
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  Serial.println("Blynk Connected");
}

void sendData()
{
  Serial.println("sendData");
  sensors.updateAll();
  Blynk.beginGroup();
  Blynk.virtualWrite(V_TEMP, sensors.getTemperature());
  Blynk.virtualWrite(V_HUMIDITY, sensors.getHumidity());
  for (int i = 0; i < NUM_SOIL_SENSORS; i++)
  {
    Blynk.virtualWrite(V_SOIL_PINS[i], sensors.getSoilDryness(i));
  }
  Blynk.virtualWrite(V_BRIGHT, sensors.getBrightness());
  Blynk.virtualWrite(V_CO2, sensors.getCo2());
  Blynk.endGroup();

  uint16_t *log_buff = new uint16_t[256];
  JsonDocument doc;
  doc["temp"] = sensors.getTemperature();
  doc["humidity"] = sensors.getHumidity();
  doc["brightness"] = sensors.getBrightness();
  doc["co2"] = sensors.getCo2();
  for (int i = 0; i < NUM_SOIL_SENSORS; i++)
  {
    doc["soil_dryness_" + String(i)] = sensors.getSoilDryness(i);
  }
  serializeJson(doc, log_buff, 256);

  logger.build()
      .cloud(true)
      .topic("greenhouse/data/temperature")
      .data(sensors.getTemperature())
      .log();

  logger.build()
      .cloud(true)
      .topic("greenhouse/data/humidity")
      .data(sensors.getHumidity())
      .log();

  logger.build()
      .cloud(true)
      .topic("greenhouse/data/brightness")
      .data(sensors.getBrightness())
      .log();

  logger.build()
      .cloud(true)
      .topic("greenhouse/data/co2")
      .data(sensors.getCo2())
      .log();

  for (int i = 0; i < NUM_SOIL_SENSORS; i++)
  {
    logger.build()
        .cloud(true)
        .topic(("greenhouse/data/soil_dryness/" + String(i)).c_str())
        .data(sensors.getSoilDryness(i))
        .log();
  }

  logger.build()
      .serial(true)
      .cloud(true)
      .data("sendData Complete")
      .log();
}

void checkBtns()
{
  carrier.Buttons.update();
  if (carrier.Buttons.onTouchDown(TOUCH0))
  {
    sendData();
  }
}

void waterIfNeeded()
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
}

void humidifyIfNeeded()
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
}

void fanIfNeeded()
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
  pinMode(PUMP_1, OUTPUT);
  pinMode(MISTER, OUTPUT);
  pinMode(FAN_1, OUTPUT);
  sensors.setup();

  Serial.println("Starting Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // // You can also specify server:
  // // Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  // // Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);
  // Serial.println("Connecting to WiFi...");
  // WiFi.begin(ssid, pass);
  // while (WiFi.status() != WL_CONNECTED)
  // {
  //   Serial.print(".");
  //   delay(1000);
  // }
  // Serial.println("Connected.");

  setupTime();
  ArduinoBearSSL.onGetTime(getTime);

  timer.setInterval(fifteenMin, sendData);
  timer.setInterval(100, checkBtns);
  timer.setInterval(oneSec, []() { sensors.sampleBrightness(); });
  timer.setInterval(oneSec, waterIfNeeded);
  timer.setInterval(oneSec, fanIfNeeded);
  timer.setInterval(oneSec, humidifyIfNeeded);

  logger.build()
      .serial(true)
      .cloud(true)
      .data("Setup complete")
      .log();
}

void loop()
{
  Blynk.run();
  timer.run();
  awsIotLogger.loop();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}

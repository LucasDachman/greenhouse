#include "arduino_secrets.h"
#include "globals.h"
#include "targets.h"
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
#include <WDTZero.h>
#include "LogHelpers.hpp"
#include "Actuator.h"
#include "FanStrategy.hpp"
#include "MisterStrategy.hpp"
#include "PumpStrategy.hpp"

MKRIoTCarrier carrier;

WDTZero watchdog;

#include "LED.hpp"

// WiFi credentials.
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

int oneSec = 1000L;
int oneMin = oneSec * 60;
int tenMin = 10 * oneMin;

auto timer = Timer<
    10,     /* max tasks */
    millis, /* time function for timer */
    void *  /* handler argument type */
    >();

SensorReader sensors(carrier);

FanStrategy fanStrategy;
Actuator<int> fan(FAN_1, fanStrategy);

MisterStrategy misterStrategy;
Actuator<int> mister(MISTER, misterStrategy);

PumpStrategy pumpStrategy(0);
Actuator<PumpStrategyParams> pump_1(PUMP_1, pumpStrategy);

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

void onShutdown()
{
  Serial.println("Shutting down...");
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
    doc[SOIL_KEYS[i]] = sensors.getSoilDryness(i);
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
  PumpStrategyParams params;
  sensors.getSoilDryness(params.values);
  params.start = 2;
  params.end = 3;
  pump_1.respondTo(params);
  return true;
}

bool humidifyIfNeeded(void *)
{
  sensors.updateHumidity();
  mister.respondTo(sensors.getHumidity());
  return true;
}

bool fanIfNeeded(void *)
{
  sensors.updateTemperature();
  fan.respondTo(sensors.getTemperature());
  return true;
}

bool healthBlink(void *)
{
  static bool led_on = false;
  carrier.leds.setPixelColor(0, 0, 255, 0);
  if (led_on)
  {
    led_on = false;
    carrier.leds.setBrightness(50);
  }
  else
  {
    carrier.leds.setBrightness(0);
    led_on = true;
  }
  carrier.leds.show();
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
    ledRed(0);
    while (1)
      ;
  }

  Serial.print("Epoch time: ");
  Serial.println(epoch);
}

void setupWiFi()
{
  Serial.println("Connecting to WiFi...");
  ledBlue(SETUP_LED);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(WiFi.status());
    delay(1000);
  }
  Serial.println("Connected.");
}

bool reconnectWiFi(void *)
{
  switch (WiFi.status())
  {
  case WL_CONNECT_FAILED:
  case WL_CONNECTION_LOST:
  case WL_DISCONNECTED:
    setupWiFi();
    return true;
  }
  return false;
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

  Serial.println("Starting watchdog...");
  watchdog.attachShutdown(onShutdown);
  watchdog.setup(WDT_SOFTCYCLE4M);

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
  carrier.leds.setBrightness(100);

  ledWhite(SETUP_LED);
  if (!ECCX08.begin())
  {
    Serial.println("No ECCX08 present!");
    ledRed(SETUP_LED);
    while (1)
      ;
  }
  sslClient.setEccSlot(0, awsIotCertificatePemCrt);

  Serial.println("Setting Pin modes...");
  pinMode(PUMP_1, OUTPUT);
  pinMode(PUMP_2, OUTPUT);
  pinMode(MISTER, OUTPUT);
  pinMode(FAN_1, OUTPUT);
  sensors.setup();

  setupWiFi();

  ledPurple(SETUP_LED);
  setupTime();
  ArduinoBearSSL.onGetTime(getTime);

  timer.every(tenMin, sendData);
  // timer.every(100, checkBtns);
  timer.every(oneSec, [](void *) -> bool
              { sensors.sampleBrightness(); return true; });
  timer.every(5 * oneMin, waterIfNeeded);
  timer.every(oneSec, fanIfNeeded);
  timer.every(oneSec, humidifyIfNeeded);
  timer.every(30 * oneSec, reconnectWiFi);
  // timer.every(5 * oneSec, [](void *) -> bool
  //             { sensors.updateAll(); sensors.printAll(); return true; });
  // timer.every(250, healthBlink);

  logger.build()
      .serial(true)
      .cloud(true)
      .data("Setup complete")
      .log();

  // sendData();
}

void loop()
{
  watchdog.clear();
  timer.tick();
  awsIotMqttClient.loop();
  checkBtns(nullptr);
}

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

MKRIoTCarrier carrier;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

int oneSec = 1000L;
int oneMin = oneSec * 60;
int fifteenMin = 15 * oneMin;

int temp;
int soil_dryness;
int humidity;
int brightness;
float co2;
int soilDrynessValues[NUM_SOIL_SENSORS];

BlynkTimer timer;
SmoothingFilter brightnessFilter(20);

WiFiClient wifiClient;
BearSSLClient sslClient(wifiClient);
AwsIotLogger awsIotLogger(sslClient);
Logger logger(Serial, awsIotLogger);

unsigned long getTime()
{
  return WiFi.getTime();
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  Serial.println("Blynk Connected");
}

// The selectMuxPin function sets the S0, S1, and S2 pins
// accordingly, given a pin from 0-7.
void selectMuxPin(byte pin)
{
  for (int i = 0; i < 3; i++)
  {
    if (pin & (1 << i))
      digitalWrite(muxSelectPins[i], HIGH);
    else
      digitalWrite(muxSelectPins[i], LOW);
  }
}

void sampleBrightness()
{
  // Check that new color data with brightness is available.
  if (carrier.Light.colorAvailable())
  {
    int r, g, b, brightness;
    carrier.Light.readColor(r, g, b, brightness);

    // Add the brightness sample to our filter.
    brightnessFilter.addSample(brightness);
  }
}

void updateTemp()
{
  temp = carrier.Env.readTemperature(FAHRENHEIT);
}

void updateHumidity()
{
  humidity = carrier.Env.readHumidity();
}

void updateSoilDryness()
{
  soil_dryness = analogRead(A6);
}

void updateBrightness()
{
  brightness = brightnessFilter.getSmoothedValueWithin(5000);
}

void updateC02()
{
  co2 = carrier.AirQuality.readCO2();
}

void updateAllSoilSensors()
{
  digitalWrite(SENSOR_POWER, HIGH);
  timer.setTimeout(1000, []() {
    for (int i = 0; i < NUM_SOIL_SENSORS; i++)
    {
      int muxPin = soilSensorMuxPins[i];
      selectMuxPin(muxPin);
      soil_dryness = analogRead(A6);
      soilDrynessValues[i] = soil_dryness;
      Serial.print("Soil Sensor ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(soil_dryness);
    }
    digitalWrite(SENSOR_POWER, LOW);
  });
}

void updateReadings()
{
  // Serial.println("Reading sensors");
  updateTemp();
  updateHumidity();
  updateSoilDryness();
  updateBrightness();
  updateC02();
  updateAllSoilSensors();
  // Serial.print("Temperature: \t");
  // Serial.println(temp);
  // Serial.print("Humidity: \t");
  // Serial.println(humidity);
  // Serial.print("Moisture: \t");
  // Serial.println(soil_dryness);
  // Serial.print("Brightness: \t");
  // Serial.println(brightness);
  // Serial.print("CO2: \t");
  // Serial.println(co2);
}

void sendData()
{
  Serial.println("sendData");
  updateReadings();
  Blynk.beginGroup();
  Blynk.virtualWrite(V_TEMP, temp);
  Blynk.virtualWrite(V_HUMIDITY, humidity);
  Blynk.virtualWrite(V_SOIL_M, soil_dryness);
  Blynk.virtualWrite(V_BRIGHT, brightness);
  Blynk.virtualWrite(V_CO2, co2);
  Blynk.endGroup();
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
  updateSoilDryness();
  if (digitalRead(PUMP_1) == LOW && soil_dryness > WATER_SOIL_AT)
  {
    logger.build()
        .setSerial(true)
        .setCloudLog(true)
        .setNotification(true)
        .setNotifType("pump_1_watering")
        .setMessage("Watering")
        .log();
    digitalWrite(PUMP_1, HIGH);
  }
  else if (digitalRead(PUMP_1) == HIGH && soil_dryness < WATER_SOIL_AT)
  {
    logger.build()
        .setSerial(true)
        .setCloudLog(true)
        .setNotification(true)
        .setNotifType("pump_1_watering")
        .setMessage("Stopping watering")
        .log();
    digitalWrite(PUMP_1, LOW);
  }
}

void humidifyIfNeeded()
{
  updateHumidity();
  bool mister_on = digitalRead(MISTER) == HIGH;
  if (!mister_on && humidity < MIST_AT)
  {
    logger.build()
        .setSerial(true)
        .setCloudLog(true)
        .setNotification(true)
        .setNotifType("mister")
        .setMessage("Mister On")
        .log();
    digitalWrite(MISTER, HIGH);
  }
  else if (mister_on && humidity > MIST_AT)
  {
    logger.build()
        .setSerial(true)
        .setCloudLog(true)
        .setNotification(true)
        .setNotifType("mister")
        .setMessage("Mister Off")
        .log();
    digitalWrite(MISTER, LOW);
  }
}

void fanIfNeeded()
{
  bool fan_on = digitalRead(FAN_1) == HIGH;
  updateTemp();
  if (!fan_on && temp > TEMP_HIGH_BOUND)
  {
    logger.build()
        .setSerial(true)
        .setCloudLog(true)
        .setNotification(true)
        .setNotifType("fan")
        .setMessagef("Fan On %sF", String(temp).c_str())
        .log();
    digitalWrite(FAN_1, HIGH);
  }
  else if (fan_on && temp < TEMP_LOW_BOUND)
  {
    logger.build()
        .setSerial(true)
        .setCloudLog(true)
        .setNotification(true)
        .setNotifType("fan")
        .setMessagef("Fan Off %sF", String(temp).c_str())
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
  pinMode(SENSOR_POWER, OUTPUT);
  for (int i = 0; i < 3; i++)
  {
    pinMode(muxSelectPins[i], OUTPUT);
  }

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
  timer.setInterval(oneSec, sampleBrightness);
  timer.setInterval(oneSec, waterIfNeeded);
  timer.setInterval(oneSec, fanIfNeeded);

  logger.build()
      .setSerial(true)
      .setCloudLog(true)
      .setMessage("Setup complete")
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

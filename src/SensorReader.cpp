#include "SensorReader.h"
#include "helpers/LedHelpers.hpp"

SensorReader::SensorReader()
    : temperature(-1),
      humidity(-1),
      soilDrynessValues{-1, -1, -1, -1, -1},
      sht31()
{
}

void SensorReader::setup()
{
  pinMode(SENSOR_POWER, OUTPUT);
  for (byte i = 0; i < NUM_SOIL_SENSORS; i++)
  {
    pinMode(SOIL_SENSOR_PINS[i], INPUT);
  }
  if (!sht31.begin(0x44))
  {
    Serial.println("Couldn't find SHT31");
    InternalLed::red();
    while (1)
      delay(1);
  }
}

void SensorReader::updateSoilDryness()
{
  // SENSOR_POWER is connected to a high-side switch
  digitalWrite(SENSOR_POWER, LOW);
  delay(1000); // Allow time for the sensor to stabilize
  for (byte i = 0; i < NUM_SOIL_SENSORS; i++)
  {
    // Take 10 samples and average them
    SmoothingFilter filter(10);
    for (int j = 0; j < 10; j++)
    {
      filter.addSample(analogRead(SOIL_SENSOR_PINS[i]));
      delay(50);
    }
    soilDrynessValues[i] = filter.getSmoothedValue();
    normalizeSoilDryness(soilDrynessValues[i], i);
    // Serial.print("Soil Sensor ");
    // Serial.print(i);
    // Serial.print(": ");
    // Serial.println(soilDrynessValues[i]);
  }
  digitalWrite(SENSOR_POWER, HIGH);
}

void SensorReader::updateTemperature()
{
  float temp_c = sht31.readTemperature();
  if (isnan(temp_c))
  {
    temperature = -1;
    Serial.println("Failed to read temperature");
    return;
  }
  temperature = 9.0 / 5.0 * temp_c + 32.0; // Convert to Fahrenheit
}

void SensorReader::updateHumidity()
{
  float hum = sht31.readHumidity();
  if (isnan(hum))
  {
    humidity = -1;
    Serial.println("Failed to read humidity");
    return;
  }
  humidity = hum;
}

void SensorReader::updateTempAndHumid()
{
  float temp_c, hum;
  bool success = sht31.readBoth(&temp_c, &hum);
  if (!success || isnan(temp_c) || isnan(hum))
  {
    temperature = -1;
    humidity = -1;
    Serial.println("Failed to read temperature and humidity");
    return;
  }
  temperature = 9.0 / 5.0 * temp_c + 32.0; // Convert to Fahrenheit
  humidity = hum;
}

void SensorReader::updateAll()
{
  updateSoilDryness();
  updateTempAndHumid();
}

void SensorReader::getSoilDryness(int values[NUM_SOIL_SENSORS])
{
  for (int i = 0; i < NUM_SOIL_SENSORS; i++)
  {
    values[i] = soilDrynessValues[i];
  }
}

int SensorReader::getSoilDryness(int sensorIndex)
{
  if (sensorIndex < 0 || sensorIndex >= NUM_SOIL_SENSORS)
  {
    return -1; // Invalid index
  }
  return soilDrynessValues[sensorIndex];
}

int SensorReader::getTemperature()
{
  return temperature;
}

int SensorReader::getHumidity()
{
  return humidity;
}

void SensorReader::printAll()
{
  Serial.print("Temperature: ");
  Serial.println(temperature);

  Serial.print("Humidity: ");
  Serial.println(humidity);

  for (int i = 0; i < NUM_SOIL_SENSORS; i++)
  {
    Serial.print("Soil Sensor ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(soilDrynessValues[i]);
  }
}

void SensorReader::heaterBurst()
{
  sht31.heater(true);
  delay(200);
  sht31.heater(false);
}

void SensorReader::normalizeSoilDryness(int &soilDryness, int sensorIndex)
{
  if (sensorIndex < 0 || sensorIndex >= NUM_SOIL_SENSORS)
  {
    return; // Invalid index
  }
  int ub = SOIL_UB[sensorIndex];
  int lb = SOIL_LB[sensorIndex];
  soilDryness = map(soilDryness, lb, ub, 0, 100);
}

#include "SensorReader.h"

// The selectMuxPin function sets the S0, S1, and S2 pins
// accordingly, given a pin from 0-7.
void selectMuxPin(byte pin)
{
  for (int i = 0; i < 3; i++)
  {
    if (pin & (1 << i))
      digitalWrite(MUX_SELECT_PINS[i], HIGH);
    else
      digitalWrite(MUX_SELECT_PINS[i], LOW);
  }
}

SensorReader::SensorReader()
    : temperature(-1),
      humidity(-1),
      soilDrynessValues{0, 0, 0, 0, 0},
      sht31()
{
}

void SensorReader::setup()
{
  pinMode(MUX_OUTPUT, INPUT);
  pinMode(SENSOR_POWER, OUTPUT);
  for (byte i = 0; i < 3; i++)
  {
    pinMode(MUX_SELECT_PINS[i], OUTPUT);
  }
  if (!sht31.begin(0x44))
  {
    Serial.println("Couldn't find SHT31");
    while (1)
      delay(1);
  }
}

void SensorReader::updateSoilDryness()
{
  // SENSOR_POWER is connected to a high-side switch
  digitalWrite(SENSOR_POWER, LOW);
  for (byte i = 0; i < NUM_SOIL_SENSORS; i++)
  {
    byte muxPin = SOIL_SENSOR_MUX_PINS[i];
    selectMuxPin(muxPin);
    delay(1000); // Allow time for the sensor to stabilize
    soilDrynessValues[i] = analogRead(MUX_OUTPUT);
    // Take 10 samples and average them
    SmoothingFilter filter(10);
    for (int j = 0; j < 10; j++)
    {
      filter.addSample(analogRead(MUX_OUTPUT));
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
  temperature = 9 / 5 * temp_c + 32; // Convert to Fahrenheit
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

void SensorReader::updateAll()
{
  updateSoilDryness();
  updateTemperature();
  updateHumidity();
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

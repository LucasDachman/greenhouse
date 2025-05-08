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

SensorReader::SensorReader(MKRIoTCarrier &carrier, Logger &logger, Timer<> &timer)
    : carrier(carrier),
      logger(logger),
      timer(timer),
      temperature(-1),
      humidity(-1),
      brightness(-1),
      co2(-1),
      brightnessFilter(20)
{
  for (int i = 0; i < NUM_SOIL_SENSORS; i++)
  {
    soilDrynessValues[i] = -1;
  }
}

void SensorReader::setup()
{
  pinMode(MUX_OUTPUT, INPUT);
  pinMode(SENSOR_POWER, OUTPUT);
  for (byte i = 0; i < 3; i++)
  {
    pinMode(MUX_SELECT_PINS[i], OUTPUT);
  }
}

void SensorReader::updateSoilDryness()
{
  // SENSOR_POWER is connected to a high-side switch
  digitalWrite(SENSOR_POWER, LOW);
  delay(500); // Allow time for the sensor to stabilize
  for (byte i = 0; i < NUM_SOIL_SENSORS; i++)
  {
    byte muxPin = SOIL_SENSOR_MUX_PINS[i];
    selectMuxPin(muxPin);
    delay(500); // Allow time for the sensor to stabilize
    soilDrynessValues[i] = analogRead(MUX_OUTPUT);
    // Serial.print("Soil Sensor ");
    // Serial.print(i);
    // Serial.print(": ");
    // Serial.println(soilDrynessValues[i]);
  }
  digitalWrite(SENSOR_POWER, HIGH);
}

void SensorReader::updateTemperature()
{
  temperature = carrier.Env.readTemperature(FAHRENHEIT);
}

void SensorReader::updateHumidity()
{
  humidity = carrier.Env.readHumidity();
}

void SensorReader::updateBrightness()
{
  brightness = brightnessFilter.getSmoothedValueWithin(5000);
}

void SensorReader::sampleBrightness()
{
  if (carrier.Light.colorAvailable())
  {
    int r, g, b, brightness_sample;
    carrier.Light.readColor(r, g, b, brightness_sample);

    brightnessFilter.addSample(brightness);
  }
}

void SensorReader::updateCo2()
{
  co2 = carrier.AirQuality.readCO2();
}

void SensorReader::updateAll()
{
  updateSoilDryness();
  updateTemperature();
  updateHumidity();
  sampleBrightness();
  updateCo2();
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

int SensorReader::getBrightness()
{
  return brightness;
}

float SensorReader::getCo2()
{
  return co2;
}

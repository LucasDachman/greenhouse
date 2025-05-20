#ifndef SensorReader_h
#define SensorReader_h

// Encapsulates reading sensor values from the arduino

#include "Logger.h"
#include "pin_defs.h"
#include "SmoothingFilter.h"
#include <arduino-timer.h>
#include <Adafruit_SHT31.h>

// Upper bound for soil sensor readings
// This is the value of the sensor in water
const int SOIL_UB[NUM_SOIL_SENSORS] = {
    687,  // Sensor 0
    689,  // Sensor 1
    700,  // Sensor 2
    1023,  // Sensor 3
    1023   // Sensor 4
};

// Lower bound for soil sensor readings
// This is the value of the sensor in dry air
const int SOIL_LB[NUM_SOIL_SENSORS] = {
    293,  // Sensor 0
    296,  // Sensor 1
    305,  // Sensor 2
    0, // Sensor 3
    0 // Sensor 4
};


class SensorReader
{
  public:
    SensorReader();

    void setup();

    void updateSoilDryness();
    void updateTemperature();
    void updateHumidity();
    void updateAll();

    void getSoilDryness(int values[NUM_SOIL_SENSORS]);
    int getSoilDryness(int sensorIndex);
    int getTemperature();
    int getHumidity();

    void printAll();

    private:

    float temperature;
    float humidity;
    int soilDrynessValues[NUM_SOIL_SENSORS];
    Adafruit_SHT31 sht31;

    void normalizeSoilDryness(int &soilDryness, int sensorIndex);
};

#endif
// SensorReader_h

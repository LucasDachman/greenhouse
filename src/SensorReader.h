#ifndef SensorReader_h
#define SensorReader_h

// Encapsulates reading sensor values from the arduino

#include "Arduino_MKRIoTCarrier.h"
#include "Logger.h"
#include "pin_defs.h"
#include "SmoothingFilter.h"
#include <arduino-timer.h>

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
    SensorReader(MKRIoTCarrier &carrier);

    void setup();

    void updateSoilDryness();
    void updateTemperature();
    void updateHumidity();
    void updateBrightness();
    void sampleBrightness();
    void updateCo2();
    void updateAll();

    void getSoilDryness(int values[NUM_SOIL_SENSORS]);
    int getSoilDryness(int sensorIndex);
    int getTemperature();
    int getHumidity();
    int getBrightness();
    float getCo2();

    void printAll();

    private:

    MKRIoTCarrier &carrier;

    int temperature;
    int humidity;
    int brightness;
    float co2;
    SmoothingFilter brightnessFilter;
    int soilDrynessValues[NUM_SOIL_SENSORS];

    void normalizeSoilDryness(int &soilDryness, int sensorIndex);
};

#endif
// SensorReader_h

// Encapsulates reading sensor values from the arduino
#ifndef SensorReader_h
#define SensorReader_h

#include "Arduino_MKRIoTCarrier.h"
#include "Logger.h"
#include "pin_defs.h"
#include "SmoothingFilter.h"

class SensorReader
{
  public:
    SensorReader(MKRIoTCarrier &carrier, Logger &logger);

    void updateAll();
    void updateSoilDryness();
    void updateTemperature();
    void updateHumidity();
    void updateBrightness();
    void updateCo2();

    int getSoilDryness(int sensorIndex);
    int getTemperature();
    int getHumidity();
    int getBrightness();
    float getCo2();

    private:
    MKRIoTCarrier &carrier;
    Logger &logger;
    int soilDrynessValues[NUM_SOIL_SENSORS];
    int temperature;
    int humidity;
    int brightness;
    float co2;
    SmoothingFilter brightnessFilter;
};

#endif
// SensorReader_h

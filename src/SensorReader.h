#ifndef SensorReader_h
#define SensorReader_h

// Encapsulates reading sensor values from the arduino

#include "Arduino_MKRIoTCarrier.h"
#include "Logger.h"
#include "pin_defs.h"
#include "SmoothingFilter.h"
#include <Adapters/BlynkWiFiCommon.h>

class SensorReader
{
  public:
    SensorReader(MKRIoTCarrier &carrier, Logger &logger, BlynkTimer &timer);

    void setup();

    void updateSoilDryness();
    void updateTemperature();
    void updateHumidity();
    void updateBrightness();
    void sampleBrightness();
    void updateCo2();
    void updateAll();

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
    BlynkTimer timer;
    SmoothingFilter brightnessFilter;
};

#endif
// SensorReader_h

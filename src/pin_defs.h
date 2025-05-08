#ifndef PIN_DEFS_H
#define PIN_DEFS_H

#include <Blynk/BlynkHandlers.h>

// Pin definitions for the greenhouse project


#define V_TEMP V0
#define V_HUMIDITY V1
#define V_BRIGHT V3
#define V_LOG V4
#define V_CO2 V5

#define FAN_1 9
#define MISTER 10
#define PUMP_1 11
#define PUMP_2 12
#define SENSOR_POWER 0
#define MUX_OUTPUT A0
#define NUM_SOIL_SENSORS 5

const int V_SOIL_PINS[NUM_SOIL_SENSORS] = {
  V2, // Parsely 1
  V6, // Parsely 2
  V4, // Tomato
  V7, // Basil
  V8  // Chives
};

const byte MUX_SELECT_PINS[3] = {3, 4, 5}; // S0, S1, S2
const byte SOIL_SENSOR_MUX_PINS[NUM_SOIL_SENSORS] = {0, 1, 2, 3, 4}; // Y0, Y1, Y2, Y3, Y4


#endif // PIN_DEFS_H

#ifndef PIN_DEFS_H
#define PIN_DEFS_H

// Pin definitions for the greenhouse project


#define V_TEMP V0
#define V_HUMIDITY V1
#define V_SOIL_M V2
#define V_BRIGHT V3
#define V_LOG V4
#define V_CO2 V5

#define PUMP_1 8
#define FAN_1 9
#define MISTER 10
#define SENSOR_POWER 0
#define NUM_SOIL_SENSORS 5

const int muxSelectPins[3] = {3, 4, 5}; // S0, S1, S2
const int soilSensorMuxPins[NUM_SOIL_SENSORS] = {0, 1, 2, 3, 4}; // Y0, Y1, Y2, Y3, Y4


#endif // PIN_DEFS_H

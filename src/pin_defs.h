#ifndef PIN_DEFS_H
#define PIN_DEFS_H

// Pin definitions for the greenhouse project

#define FAN_1 9
#define MISTER 10
#define PUMP_1 11
#define PUMP_2 12
#define SENSOR_POWER 0
#define MUX_OUTPUT A0
#define NUM_SOIL_SENSORS 5

const byte MUX_SELECT_PINS[3] = {3, 4, 5}; // S0, S1, S2
const byte SOIL_SENSOR_MUX_PINS[NUM_SOIL_SENSORS] = {0, 1, 2, 3, 4}; // Y0, Y1, Y2, Y3, Y4


#endif // PIN_DEFS_H

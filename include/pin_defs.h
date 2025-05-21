#ifndef PIN_DEFS_H
#define PIN_DEFS_H

// Pin definitions for the greenhouse project

#define FAN_1 5
#define MISTER 4
#define PUMP_1 3
#define PUMP_2 2
#define SENSOR_POWER 1

#define SETUP_LED 4
#define BTN_1 6

#define NUM_SOIL_SENSORS 5

const byte SOIL_SENSOR_PINS[NUM_SOIL_SENSORS] = {A1, A2, A3, A4, A5};

#endif // PIN_DEFS_H

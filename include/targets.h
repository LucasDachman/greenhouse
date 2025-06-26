#ifndef TARGETS_H
#define TARGETS_H

#define MIST_HIGH_BOUND 55
#define MIST_LOW_BOUND 45

#define TEMP_LOW_BOUND 72
#define TEMP_HIGH_BOUND 74

#include "pin_defs.h"

static const char *SOIL_KEYS[NUM_SOIL_SENSORS] = {
    "soil0",
    "soil1",
    "soil2",
    "soil3",
    "soil4"};

// Percentage threshold for soil dryness
// This is the value where we start watering
static const int SOIL_THRESHOLDS[NUM_SOIL_SENSORS] = {
    59,  // Sensor 0
    36,  // Sensor 1
    68,  // Sensor 2
    100,  // Sensor 3
    100   // Sensor 4
};

#endif

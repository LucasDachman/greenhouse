#ifndef LOGHELPERS_H
#define LOGHELPERS_H

#include <ArduinoJson.h>
#include "targets.h"

inline JsonDocument fanLogDoc(byte state, int temp)
{
  JsonDocument doc;
  doc["fanState"] = state;
  doc["temp"] = temp;
  doc.shrinkToFit();
  return doc;
}

inline JsonDocument misterLogDoc(byte state, int hum)
{
  JsonDocument doc;
  doc["misterState"] = state;
  doc["hum"] = hum;
  doc.shrinkToFit();
  return doc;
}

inline JsonDocument pumpLogDoc(byte state, byte pumpIndex, int soilValues[])
{
  JsonDocument doc;
  doc["pumpState"] = state;
  doc["pumpIdx"] = pumpIndex;
  for (int i = 0; i < NUM_SOIL_SENSORS; i++)
  {
    doc[SOIL_KEYS[i]] = soilValues[i];
  }
  doc.shrinkToFit();
  return doc;
}

#endif

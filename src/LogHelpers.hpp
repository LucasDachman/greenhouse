#include <ArduinoJson.h>

JsonDocument fanLogDoc(byte state, int temp)
{
  JsonDocument doc;
  doc["fanState"] = state;
  doc["temp"] = temp;
  doc.shrinkToFit();
  return doc;
}

JsonDocument misterLogDoc(byte state, int hum)
{
  JsonDocument doc;
  doc["misterState"] = state;
  doc["hum"] = hum;
  doc.shrinkToFit();
  return doc;
}

JsonDocument pumpLogDoc(byte state, byte pumpIndex, int soilDryness)
{
  JsonDocument doc;
  doc["pumpState"] = state;
  doc["pumpIdx"] = pumpIndex;
  doc["soilDryness"] = soilDryness;
  doc.shrinkToFit();
  return doc;
}

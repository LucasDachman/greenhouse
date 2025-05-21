#include <cstddef>
#ifndef MQTT_EVENT_HANDLER_H
#define MQTT_EVENT_HANDLER_H

class MqttEventHandler
{
public:
  virtual void onConnectStart() = 0;
  virtual void afterConnect(bool success) = 0;
  virtual void afterPublish(const char *topic, bool success) = 0;
};

#endif

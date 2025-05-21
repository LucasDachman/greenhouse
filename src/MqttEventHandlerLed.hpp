#include "MqttEventHandler.hpp"
#include "helpers/LedHelpers.hpp"

class MqttEventHandlerLed : public MqttEventHandler
{

  void onConnectStart() override {
    InternalLed::orange();
  }

  void afterConnect(bool success) override {
    if (success)
    {
      InternalLed::yellow();
    }
    else
    {
      InternalLed::red();
    }
  }

  void afterPublish(const char *topic, bool success) override {
    if (success)
    {
      InternalLed::green();
    }
    else
    {
      InternalLed::red();
    }
  }
};

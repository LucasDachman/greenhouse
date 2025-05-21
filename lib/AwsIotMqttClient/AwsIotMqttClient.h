#ifndef AWS_IOT_MQTT_CLIENT_H
#define AWS_IOT_MQTT_CLIENT_H

#include <WiFiNINA.h>
#include <ArduinoBearSSL.h>
#include <ArduinoECCX08.h>
#include <ArduinoMqttClient.h>
#include <MqttEventHandler.hpp>

class AwsIotMqttClient
{
private:
  const char *awsEndpoint;
  static constexpr const char *awsTopic = "greenhouse/default";
  int awsPort = 8883;

  MqttClient mqttClient;
  MqttEventHandler *eventHandler = nullptr;

public:
  AwsIotMqttClient(const char *awsEndpoint, BearSSLClient &sslClient, MqttEventHandler *eventHandler = nullptr);
  bool connect();
  bool publish(const char *data, const char* topic);
  void loop();
};

#endif // AWS_IOT_MQTT_CLIENT_H

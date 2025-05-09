#ifndef AWS_IOT_LOGGER_H
#define AWS_IOT_LOGGER_H

#include <WiFiNINA.h>
#include "arduino_secrets.h"
#include <ArduinoBearSSL.h>
#include <ArduinoECCX08.h>
#include <ArduinoMqttClient.h>

class AwsIotMqttClient
{
private:
  MqttClient mqttClient;

  const char *awsEndpoint = awsIotEndpoint;
  int awsPort = 8883;
  static constexpr const char *awsTopic = "greenhouse/logs/mkrwifi1010";

public:
  AwsIotMqttClient(BearSSLClient &sslClient);
  bool connect();
  bool publish(const char *data, const char* topic);
  void loop();
};

#endif // AWS_IOT_LOGGER_H

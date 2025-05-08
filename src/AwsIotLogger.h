#ifndef AWS_IOT_LOGGER_H
#define AWS_IOT_LOGGER_H

#include <WiFiNINA.h>
#include "arduino_secrets.h"
#include <ArduinoBearSSL.h>
#include <ArduinoECCX08.h>
#include <ArduinoMqttClient.h>

class AwsIotLogger
{
private:
  MqttClient mqttClient;

  const char *awsEndpoint = awsIotEndpoint;
  int awsPort = 8883;
  static constexpr const char *awsTopic = "greenhouse/logs/mkrwifi1010";

public:
  AwsIotLogger(BearSSLClient &sslClient);
  bool connect();
  bool publishLog(const char *logMessage, const char* topic);
  void loop();
};

#endif // AWS_IOT_LOGGER_H

#include "globals.h"
#include "arduino_secrets.h"
#include "MqttEventHandlerLed.hpp"

WiFiClient wifiClient;
BearSSLClient sslClient(wifiClient);
MqttEventHandlerLed *mqttEventHandler;
AwsIotMqttClient awsIotMqttClient(awsIotEndpoint, sslClient, mqttEventHandler);
Logger logger(Serial, awsIotMqttClient);

#include "globals.h"

WiFiClient wifiClient;
BearSSLClient sslClient(wifiClient);
AwsIotMqttClient awsIotMqttClient(sslClient);
Logger logger(Serial, awsIotMqttClient);

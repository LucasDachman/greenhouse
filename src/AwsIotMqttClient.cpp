#include "AwsIotMqttClient.h"
#include "LED.hpp"
#include "pin_defs.h"

int freeMemory();

AwsIotMqttClient::AwsIotMqttClient(BearSSLClient &sslClient) : mqttClient(sslClient)
{
}

bool AwsIotMqttClient::connect()
{
  if (!ECCX08.begin())
  {
    Serial.println("No ECCX08 present!");
    while (1)
      ;
  }

  // Connect to AWS IoT Core
  while (!mqttClient.connected())
  {
    Serial.print("Free memory: ");
    Serial.println(freeMemory());
    Serial.println("Connecting to AWS IoT Core...");
    if (mqttClient.connect(awsIotEndpoint, awsPort))
    {
      Serial.println("Connected to AWS IoT Core");
      return true;
    }
    else
    {
      Serial.print("Failed, rc=");
      Serial.println(mqttClient.connectError());
      delay(2000);
    }
  }
  return false;
}

bool AwsIotMqttClient::publish(const char *data, const char *topic = awsTopic)
{
  ledOrange(4);
  Serial.println("Publishing log data to AWS IoT Core");
  if (!mqttClient.connected())
  {
    Serial.println("MQTT client not connected. Reconnecting...");
    if (connect()) {
      Serial.println("Reconnected to AWS IoT Core");
    }
    else
    {
      return false;
    }
  }

  // Serial.println("mqttClient connected");
  mqttClient.beginMessage(topic, false, 1, false);
  mqttClient.print(data);
  int result = mqttClient.endMessage();
  if (result)
  {
    // Serial.println("Log data published successfully");
    ledGreen(4);
    return true;
  }
  else
  {
    Serial.print("Failed to publish log data. Code: ");
    Serial.println(result);
    ledRed(4);
    return false;
  }
}

void AwsIotMqttClient::loop()
{
  mqttClient.poll();
}

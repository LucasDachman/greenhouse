#include "AwsIotMqttClient.h"

AwsIotMqttClient::AwsIotMqttClient(const char *awsEndpoint, BearSSLClient &sslClient, MqttEventHandler *eventHandler)
    : awsEndpoint(awsEndpoint), mqttClient(sslClient), eventHandler(eventHandler)
{
}

bool AwsIotMqttClient::connect()
{
  if (eventHandler)
  {
    eventHandler->onConnectStart();
  }
  if (!ECCX08.begin())
  {
    Serial.println("No ECCX08 present!");
    while (1)
      ;
  }

  // Connect to AWS IoT Core
  while (!mqttClient.connected())
  {
    Serial.println("Connecting to AWS IoT Core...");
    if (mqttClient.connect(awsEndpoint, awsPort))
    {
      Serial.println("Connected to AWS IoT Core");
      if (eventHandler)
      {
        eventHandler->afterConnect(true);
      }
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
  Serial.println("Publishing log data to AWS IoT Core");
  if (!mqttClient.connected())
  {
    Serial.println("MQTT client not connected. Reconnecting...");
    if (connect())
    {
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
    Serial.println("Log data published successfully");
    if (eventHandler)
    {
      eventHandler->afterPublish(topic, true);
    }
    return true;
  }
  else
  {
    Serial.print("Failed to publish log data. Code: ");
    Serial.println(result);
    if (eventHandler)
    {
      eventHandler->afterPublish(topic, false);
    }
    return false;
  }
}

void AwsIotMqttClient::loop()
{
  mqttClient.poll();
}

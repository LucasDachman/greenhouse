#include "AwsIotLogger.h"


AwsIotLogger::AwsIotLogger(BearSSLClient &sslClient) : mqttClient(sslClient)
{
}

bool AwsIotLogger::connect()
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

bool AwsIotLogger::publishLog(const char *logData)
{
  Serial.println("Publishing log data to AWS IoT Core");
  if (!mqttClient.connected())
  {
    Serial.println("MQTT client not connected. Reconnecting...");
    if (!connect())
    {
      return false;
    }
  }

  // Serial.println("mqttClient connected");
  mqttClient.beginMessage(awsTopic, false, 1, false);
  mqttClient.print(logData);
  int result = mqttClient.endMessage();
  if (result)
  {

    // Serial.println("Log data published successfully");
    return true;
  }
  else
  {
    Serial.print("Failed to publish log data. Code: ");
    Serial.println(result);
    return false;
  }
}

void AwsIotLogger::loop()
{
  mqttClient.poll();
}



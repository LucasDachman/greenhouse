#include "Logger.h"
#include <Arduino.h>
#include "arduino_secrets.h"
#include "pin_defs.h"
#include <HardwareSerial.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <cstdarg>
#include <time.h>

int freeMemory();

Logger::Logger(Stream &stream, AwsIotMqttClient &awsIotMqttClient) : stream(&stream), mqttClient(&awsIotMqttClient) {}

void Logger::log(const LogParams &params)
{
  if (params.serial)
  {
    size_t len = measureJson(params.doc["data"]);
    char *data = new char[len + 1];
    serializeJson(params.doc["data"], data, len + 1);
    stream->write(data);
    stream->write("\n");
  }
  if (params.cloud || params.notification)
  {
    // Get the current time
    time_t now = WiFi.getTime();
    struct tm *timeinfo = localtime(&now);
    char timeString[20];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", timeinfo);

    // Serial.print("timeString: ");
    // Serial.println(timeString);

    // Serial.print("data: ");
    // Serial.println(data);

    Serial.print("Free memory: ");
    Serial.println(freeMemory());

    JsonDocument doc;
    doc["t"] = timeString;
    doc["data"] = params.doc["data"];
    if (params.notification)
    {
      doc["ntfn"] = true;
    }
    doc.shrinkToFit();
    size_t len = measureJson(doc);
    char *json = new char[len + 1];
    serializeJson(doc, json, len + 1);

    // Serial.print("log: ");
    // Serial.println(log);
    mqttClient->publish(json, params.topic);

    delete[] json;
  }
  // delete[] data;
}

LogParamsBuilder Logger::build()
{
  return LogParamsBuilder(this);
}

// LogParamsBuilder class implementation

LogParamsBuilder::LogParamsBuilder(Logger *logger) : logger(logger)
{
}

LogParamsBuilder &LogParamsBuilder::serial(bool serial)
{
  params.serial = serial;
  return *this;
}

LogParamsBuilder &LogParamsBuilder::notification(bool notification)
{
  params.notification = notification;
  return *this;
}

LogParamsBuilder &LogParamsBuilder::cloud(bool cloud_log)
{
  params.cloud = cloud_log;
  return *this;
}

LogParamsBuilder &LogParamsBuilder::topic(const char *topic)
{
  params.topic = topic;
  return *this;
}

LogParamsBuilder &LogParamsBuilder::data(JsonVariant data)
{
  params.doc["data"] = data;
  return *this;
}

LogParamsBuilder &LogParamsBuilder::data(const char *data)
{
  params.doc["data"] = data;
  return *this;
}

LogParamsBuilder &LogParamsBuilder::data(String &data)
{
  params.doc["data"] = data;
  return *this;
}

LogParamsBuilder &LogParamsBuilder::data(int data)
{
  params.doc["data"] = data;
  return *this;
}

LogParamsBuilder &LogParamsBuilder::dataf(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  char data[256];
  vsnprintf(data, sizeof(data), format, args);
  va_end(args);
  return this->data(data);
}

Logger::LogParams LogParamsBuilder::build()
{
  return params;
}

void LogParamsBuilder::log()
{
  logger->log(params);
}

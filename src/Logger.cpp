#include "Logger.h"
#include <Arduino.h>
#include "arduino_secrets.h"
#include "pin_defs.h"
#include <HardwareSerial.h>
#include <SPI.h>
#include <WiFiNINA.h>
#define NO_GLOBAL_BLYNK true
#include <BlynkSimpleWiFiNINA.h>
#include <cstdarg>
#include <time.h>

Logger::Logger(Stream &stream, AwsIotLogger &awsIotLogger) : stream(&stream), cloudLogger(&awsIotLogger) {}

void Logger::log(const LogParams &params)
{
  if (params.serial)
  {
    stream->write(params.message);
    stream->write("\n");
  }
  if (params.notification)
  {
    Blynk.logEvent(params.notif_type, params.message);
  }
  if (params.cloud)
  {
    // Get the current time
    time_t now = WiFi.getTime();
    struct tm *timeinfo = localtime(&now);
    char timeString[64];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", timeinfo);

    char *log;
    snprintf(log, 256, "{\"time\": \"%s\", \"message\": \"%s\"}", timeString, params.message);
    cloudLogger->publishLog(log);
  }
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

LogParamsBuilder &LogParamsBuilder::notifType(const char *notif_type)
{
  params.notif_type = notif_type;
  return *this;
}

LogParamsBuilder &LogParamsBuilder::message(const char *message)
{
  params.message = message;
  return *this;
}

LogParamsBuilder &LogParamsBuilder::message(String &message)
{
  params.message = message.c_str();
  return *this;
}

LogParamsBuilder &LogParamsBuilder::message(int message)
{
  params.message = String(message).c_str();
  return *this;
}

LogParamsBuilder &LogParamsBuilder::messagef(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  char message[256];
  vsnprintf(message, sizeof(message), format, args);
  va_end(args);
  return this->message(message);
}

Logger::LogParams LogParamsBuilder::build()
{
  return params;
}

void LogParamsBuilder::log()
{
  logger->log(params);
}

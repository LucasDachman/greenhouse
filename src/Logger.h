#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "arduino_secrets.h"
#include "AwsIotLogger.h"

class LogParamsBuilder;

class Logger
{
public:
  struct LogParams
  {
    bool serial = false;
    bool notification = false;
    const char *notif_type = "Log";
    bool cloud = false;
    const char *topic = "greenhouse/logs/mkrwifi1010";
    JsonVariant dataJson;
    const char *data;
  };

  Logger(Stream &stream, AwsIotLogger &awsIotLogger);

  void log(const LogParams &params);

  LogParamsBuilder build();

  Stream *stream;
  AwsIotLogger *cloudLogger = nullptr;
};

class LogParamsBuilder
{
private:
  Logger::LogParams params;
  Logger *logger;

public:
  LogParamsBuilder(Logger *logger);

  LogParamsBuilder &serial(bool serial);

  LogParamsBuilder &notification(bool notification);

  LogParamsBuilder &cloud(bool cloud_log);

  LogParamsBuilder &topic(const char *topic);

  LogParamsBuilder &notifType(const char *notif_type);

  LogParamsBuilder &data(const char *data);

  LogParamsBuilder &data(String &data);

  LogParamsBuilder &data(int data);

  LogParamsBuilder &data(JsonVariant data);

  LogParamsBuilder &dataf(const char *format, ...);

  Logger::LogParams build();

  void log();
};

#endif // LOGGER_H

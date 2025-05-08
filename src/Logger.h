#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
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
    char *notif_type = "Log";
    bool cloud = false;
    char *topic = "greenhouse/logs/mkrwifi1010";
    char *message;
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

  LogParamsBuilder &notifType(char *notif_type);

  LogParamsBuilder &message(char *message);

  LogParamsBuilder &messagef(const char *format, ...);

  Logger::LogParams build();

  void log();
};

#endif // LOGGER_H

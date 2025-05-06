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
    bool cloud_log = false;
    String notif_type = "Log";
    String message;
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

  LogParamsBuilder &setSerial(bool serial);

  LogParamsBuilder &setNotification(bool notification);

  LogParamsBuilder &setCloudLog(bool cloud_log);

  LogParamsBuilder &setNotifType(const String &notif_type);

  LogParamsBuilder &setMessage(const String &message);

  LogParamsBuilder &setMessagef(const char *format, ...);

  Logger::LogParams build();

  void log();
};

#endif // LOGGER_H

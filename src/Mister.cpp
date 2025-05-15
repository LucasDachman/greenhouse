#include "Mister.h"
#include "LogHelpers.hpp"
#include "targets.h"

Mister::Mister(int pin, Logger &logger) : pin(pin), logger(logger)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void Mister::start()
{
  digitalWrite(pin, HIGH);
}

void Mister::stop()
{
  digitalWrite(pin, LOW);
}

bool Mister::isOn()
{
  return digitalRead(pin) == HIGH;
}

void Mister::respondTo(int humidity)
{
  if (!isOn() && humidity < MIST_AT)
  {
    start();
    logger.build()
        .serial(true)
        .notification(true)
        .topic("greenhouse/data/actions")
        .data(misterLogDoc(1, humidity))
        .log();
  }
  else if (isOn() && humidity > MIST_AT)
  {
    stop();
    logger.build()
        .serial(true)
        .notification(true)
        .topic("greenhouse/data/actions")
        .data(misterLogDoc(0, humidity))
        .log();
  }
}

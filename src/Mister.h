#ifndef MISTER_H
#define MISTER_H

#include "Logger.h"

class Mister
{
  public:
    Mister(int pin, Logger &logger);
    void start();
    void stop();
    bool isOn();
    void respondTo(int humidity);

  private:
    int pin;
    Logger logger;
};

#endif

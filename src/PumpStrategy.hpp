#ifndef PUMPSTRATEGY_H
#define PUMPSTRATEGY_H

#include "ActuatorStrategy.h"
#include <Arduino.h>
#include "targets.h"
#include "Logger.h"
#include "LogHelpers.hpp"
#include "globals.h"

struct PumpStrategyParams
{
  int values[NUM_SOIL_SENSORS];
  size_t start;
  size_t end;
};

class PumpStrategy : public ActuatorStrategy<PumpStrategyParams>
{
public:
  byte idx;

  PumpStrategy(byte idx) : idx(idx) {}

  void execute(PumpStrategyParams &params, bool isOn, std::function<void()> start, std::function<void()> stop) override
  {

    bool pump = false;
    for (size_t i = params.start; i < params.end; i++)
    {
      int soilDryness = params.values[i];
      int threshold = SOIL_THRESHOLDS[i];

      if (soilDryness > threshold)
      {
        pump = true;
        break;
      }
    }
    if (pump)
    {
      logger.build()
          .serial(true)
          .notification(true)
          .topic("greenhouse/data/actions")
          .data(pumpLogDoc(1, idx, params.values))
          .log();
      start();
      delay(3000);
      stop();
    }
  }
};

#endif

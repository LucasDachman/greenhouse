#ifndef PUMPSTRATEGY_H
#define PUMPSTRATEGY_H

#include <ActuatorStrategy.h>
#include <Actuator.hpp>
#include <Arduino.h>
#include "targets.h"
#include "Logger.h"
#include "helpers/LogHelpers.hpp"
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

  PumpStrategy(byte idx) : idx(idx) {};

  void execute(PumpStrategyParams &value, Actuator<PumpStrategyParams> &pump) override
  {

    bool shouldStart = true;
    for (size_t i = value.start; i < value.end; i++)
    {
      int soilDryness = value.values[i];
      int threshold = SOIL_THRESHOLDS[i];

      if (soilDryness < threshold)
      {
        shouldStart = false;
        break;
      }
    }
    if (shouldStart)
    {
      logger.build()
          .serial(true)
          .notification(true)
          .topic("greenhouse/data/actions")
          .data(pumpLogDoc(1, idx, value.values))
          .log();
      pump.start();
      delay(3000);
      pump.stop();
    }
  }
};

#endif

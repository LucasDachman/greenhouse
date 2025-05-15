#include <Arduino.h>
#include "LogHelpers.hpp"
#include "targets.h"
#include "Actuator.h"
#include "ActuatorStrategy.h"
#include "globals.h"

class FanStrategy : public ActuatorStrategy<int>
{

public:

  void execute(int &temp, bool isOn, std::function<void()> start, std::function<void()> stop) override
  {
    if (!isOn && temp > TEMP_HIGH_BOUND)
    {
      start();
      logger.build()
          .serial(true)
          .notification(true)
          .topic("greenhouse/data/actions")
          .data(fanLogDoc(1, temp))
          .log();
    }
    else if (isOn && temp < TEMP_LOW_BOUND)
    {
      stop();
      logger.build()
          .serial(true)
          .notification(true)
          .topic("greenhouse/data/actions")
          .data(fanLogDoc(0, temp))
          .log();
    }
  }
};

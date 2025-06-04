#include <Arduino.h>
#include "helpers/LogHelpers.hpp"
#include "targets.h"
#include <ActuatorStrategy.h>
#include <Actuator.hpp>
#include "globals.h"

class FanStrategy : public ActuatorStrategy<int>
{

public:

  void execute(int &temp, Actuator<int> &fan) override
  {
    if (!fan.isOn() && temp > TEMP_HIGH_BOUND)
    {
      fan.start();
      logger.build()
          .serial(true)
          .cloud(true)
          .topic("greenhouse/data/actions")
          .data(fanLogDoc(1, temp))
          .log();
    }
    else if (fan.isOn() && temp < TEMP_LOW_BOUND)
    {
      fan.stop();
      logger.build()
          .serial(true)
          .cloud(true)
          .topic("greenhouse/data/actions")
          .data(fanLogDoc(0, temp))
          .log();
    }
  }
};

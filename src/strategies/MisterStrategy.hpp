#ifndef MISTERSTRATEGY_H
#define MISTERSTRATEGY_H

#include "ActuatorStrategy.h"
#include <Actuator.hpp>
#include <Arduino.h>
#include <Logger.h>
#include "targets.h"
#include "helpers/LogHelpers.hpp"
#include "globals.h"

class MisterStrategy : public ActuatorStrategy<int>
{
public:

    void execute(int &humidity, Actuator<int> &mister) override
    {
        if (!mister.isOn() && humidity < MIST_LOW_BOUND)
        {
            mister.start();
            logger.build()
                .serial(true)
                .topic("greenhouse/data/actions")
                .data(misterLogDoc(1, humidity))
                .log();
        }
        else if (mister.isOn() && humidity > MIST_HIGH_BOUND)
        {
            mister.stop();
            logger.build()
                .serial(true)
                .topic("greenhouse/data/actions")
                .data(misterLogDoc(0, humidity))
                .log();
        }
    }
};

#endif

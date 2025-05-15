#ifndef MISTERSTRATEGY_H
#define MISTERSTRATEGY_H

#include "ActuatorStrategy.h"
#include "Actuator.h"
#include <Arduino.h>
#include "targets.h"
#include "Logger.h"
#include "LogHelpers.hpp"
#include "globals.h"

class MisterStrategy : public ActuatorStrategy<int>
{
public:

    void execute(int &humidity, Actuator<int> &mister) override
    {
        if (!mister.isOn() && humidity < MIST_AT)
        {
            mister.start();
            logger.build()
                .serial(true)
                .notification(true)
                .topic("greenhouse/data/actions")
                .data(misterLogDoc(1, humidity))
                .log();
        }
        else if (mister.isOn() && humidity > MIST_AT)
        {
            mister.stop();
            logger.build()
                .serial(true)
                .notification(true)
                .topic("greenhouse/data/actions")
                .data(misterLogDoc(0, humidity))
                .log();
        }
    }
};

#endif

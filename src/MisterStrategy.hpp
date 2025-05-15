#ifndef MISTERSTRATEGY_H
#define MISTERSTRATEGY_H

#include "ActuatorStrategy.h"
#include <Arduino.h>
#include "targets.h"
#include "Logger.h"
#include "LogHelpers.hpp"
#include "globals.h"

class MisterStrategy : public ActuatorStrategy<int>
{
public:

    void execute(int &humidity, bool isOn, std::function<void()> start, std::function<void()> stop)
    {
        if (!isOn && humidity < MIST_AT)
        {
            start();
            logger.build()
                .serial(true)
                .notification(true)
                .topic("greenhouse/data/actions")
                .data(misterLogDoc(1, humidity))
                .log();
        }
        else if (isOn && humidity > MIST_AT)
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
};

#endif

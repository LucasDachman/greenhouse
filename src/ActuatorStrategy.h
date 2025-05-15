#ifndef ACTUATORSTRATEGY_H
#define ACTUATORSTRATEGY_H

#include <functional>

template <typename ValueType>
class ActuatorStrategy {
  public:

    virtual void execute(ValueType &value, bool isOn, std::function<void()> start, std::function<void()> stop) = 0;
};

#endif

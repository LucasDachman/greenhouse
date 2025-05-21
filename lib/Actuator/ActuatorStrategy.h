#ifndef ACTUATORSTRATEGY_H
#define ACTUATORSTRATEGY_H

template <typename T> class Actuator;

template <typename T>
class ActuatorStrategy {
  public:

    virtual void execute(T &value, Actuator<T> &actuator) = 0;
};

#endif

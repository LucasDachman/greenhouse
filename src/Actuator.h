#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "Logger.h"
#include "ActuatorStrategy.h"

template <typename T>
class Actuator
{
public:
  Actuator(int pin, ActuatorStrategy<T> &strategy);
  void start();
  void stop();
  bool isOn();
  void respondTo(T value);

protected:
  int pin;
  ActuatorStrategy<T> &strategy;
};

#endif

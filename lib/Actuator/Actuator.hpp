#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "ActuatorStrategy.h"

template <typename T>
class Actuator
{
public:
  Actuator(int pin, ActuatorStrategy<T> &strategy) : pin(pin), strategy(strategy)
  {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }

  void start()
  {
    digitalWrite(pin, HIGH);
  }

  void stop()
  {
    digitalWrite(pin, LOW);
  }

  bool isOn()
  {
    return digitalRead(pin) == HIGH;
  }

  void respondTo(T value)
  {
    strategy.execute(value, *this);
  }

protected:
  int pin;
  ActuatorStrategy<T> &strategy;
};

#endif

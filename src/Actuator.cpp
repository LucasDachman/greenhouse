#include "Actuator.h"
#include "PumpStrategy.hpp"

template <typename T>
Actuator<T>::Actuator(int pin, ActuatorStrategy<T> &strategy) : pin(pin), strategy(strategy)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

template <typename T>
void Actuator<T>::start()
{
  digitalWrite(pin, HIGH);
}

template <typename T>
void Actuator<T>::stop()
{
  digitalWrite(pin, LOW);
}

template <typename T>
bool Actuator<T>::isOn()
{
  return digitalRead(pin) == HIGH;
}

template <typename T>
void Actuator<T>::respondTo(T value)
{
  strategy.execute(value, isOn(), std::bind(&Actuator<T>::start, this), std::bind(&Actuator<T>::stop, this));
}

template class Actuator<int>;
template class Actuator<PumpStrategyParams>;

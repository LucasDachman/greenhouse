#ifndef LED_H
#define LED_H

#include <WiFiNINA.h>
#include <utility/wifi_drv.h>

#define INT_LED_GREEN 25
#define INT_LED_RED 26
#define INT_LED_BLUE 27

class InternalLed {
  public:
  static inline void setup()
  {
    WiFiDrv::pinMode(INT_LED_GREEN, OUTPUT);
    WiFiDrv::pinMode(INT_LED_RED, OUTPUT);
    WiFiDrv::pinMode(INT_LED_BLUE, OUTPUT);
  }

  static inline void led(int red, int green, int blue)
  {
    WiFiDrv::analogWrite(INT_LED_GREEN, green); //GREEN
    WiFiDrv::analogWrite(INT_LED_RED, red);   //RED
    WiFiDrv::analogWrite(INT_LED_BLUE, blue);   //BLUE
  }

  static inline void white()
  {
    led(255, 255, 255);
  }

  static inline void yellow()
  {
    led(255, 255, 0);
  }

  static inline void orange()
  {
    led(255, 165, 0);
  }

  static inline void blue()
  {
    led(0, 0, 255);
  }

  static inline void purple()
  {
    led(148, 0, 211);
  }

  static inline void red()
  {
    led(255, 0, 0);
  }

  static inline void green()
  {
    led(0, 255, 0);
  }
};

#endif

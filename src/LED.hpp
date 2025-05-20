#ifndef LED_H
#define LED_H

#include <WiFiNINA.h>
#include <utility/wifi_drv.h>

#define INT_LED_RED 25
#define INT_LED_GREEN 26
#define INT_LED_BLUE 27
#define LED_MAX 25

class InternalLed {
  public:
  static inline void setup()
  {
    WiFiDrv::pinMode(INT_LED_GREEN, OUTPUT);
    WiFiDrv::pinMode(INT_LED_RED, OUTPUT);
    WiFiDrv::pinMode(INT_LED_BLUE, OUTPUT);
  }

  static inline void led(int r, int g, int b)
  {
    r = map(r, 0, 255, 0, LED_MAX);
    g = map(g, 0, 255, 0, LED_MAX);
    b = map(b, 0, 255, 0, LED_MAX);
    WiFiDrv::analogWrite(INT_LED_RED, r);
    WiFiDrv::analogWrite(INT_LED_GREEN, g);
    WiFiDrv::analogWrite(INT_LED_BLUE, b);
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

#ifndef LED_H
#define LED_H

#include <Arduino_MKRIoTCarrier.h>

extern MKRIoTCarrier carrier;

inline void ledWhite(uint16_t i)
{
  carrier.leds.setPixelColor(i, 255, 255, 255);
  carrier.leds.show();
}

inline void ledYellow(uint16_t i)
{
  carrier.leds.setPixelColor(i, 255, 255, 0);
  carrier.leds.show();
}

inline void ledOrange(uint16_t i)
{
  carrier.leds.setPixelColor(i, 255, 165, 0);
  carrier.leds.show();
}

inline void ledBlue(uint16_t i)
{
  carrier.leds.setPixelColor(i, 0, 0, 255);
  carrier.leds.show();
}

inline void ledPurple(uint16_t i)
{
  carrier.leds.setPixelColor(i, 148, 0, 211);
  carrier.leds.show();
}

inline void ledRed(uint16_t i)
{
  carrier.leds.setPixelColor(i, 255, 0, 0);
  carrier.leds.show();
}

inline void ledGreen(uint16_t i)
{
  carrier.leds.setPixelColor(i, 0, 255, 0);
  carrier.leds.show();
}

#endif

#ifndef __DS3231_H
#define __DS3231_H

#include <Arduino.h>
//#include <Whire.h>

class DS3231 {
public:
  static void init(bool fast = false);
#if defined(ESP8266) || defined(ESP32)
  static void init(int8_t pinSDA, int8_t pinSCL, bool fast = false);
#endif
  static bool begin();
  static uint32_t get();
  static void set(uint32_t time);
};

extern DS3231 ds3231;

#endif

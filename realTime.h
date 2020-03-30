#ifndef REAL_TIME_H
#define REAL_TIME_H

#include <Arduino.h>
#ifdef ESP32
    #include "WiFi.h"
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
#endif

// #include "StringLog.h"
#include "Date.h"


// #define USEDS3231 //Uncoment if using ds3231 rtc mdule

#ifdef USEDS3231
    #include "DS3231.h"
    const bool fastI2C = true; // I2C Fast mode (400 kHz)
#endif

class RealTimeClock{
public:
    RealTimeClock();
    void begin(const String & ntpServer1, const String & ntpServer2, 
                const String & ntpServer3, const int8_t &ntpTimeZone, 
                Print* ptrLog = NULL);
    uint32_t getTime();
    void setTime(uint32_t now);
    #ifdef USEDS3231
        void ds3231Syncronize();
    #endif
private:
    const uint32_t ntpUpdateInterval = 3600000; // Интервал в миллисекундах для обновления времени с NTP-серверов (по умолчанию 1 час)
    Print* _log;
    String server1;
    String server2;
    String server3;
    int8_t TimeZone;
    uint32_t lastNtpTime; // Последнее полученное от NTP-серверов время в формате UNIX-time
    uint32_t lastNtpUpdate; // Значение millis() в момент последней синхронизации времени
};

extern RealTimeClock realTimeClock;



#endif

#include "realTime.h"


RealTimeClock realTimeClock;

RealTimeClock::RealTimeClock(){
                lastNtpTime = 0; // Последнее полученное от NTP-серверов время в формате UNIX-time
                lastNtpUpdate = 0; // Значение millis() в момент последней синхронизации времени
                server1 = "";
                server2 = "";
                server3 = "";
                TimeZone = 0;
                _log = NULL;
            }

/**
 * Config a Real Time Clock by NTP servers or start connection with ds3231 RTC module.
 * @param _ntp conficuration object
 * @param _log is a pointer to stream for loging
 */
void RealTimeClock::begin(const String & ntpServer1, const String & ntpServer2, const String & ntpServer3, const int8_t &ntpTimeZone, Print* ptrLog){
	server1 = ntpServer1;
	server2 = ntpServer2;
	server3 = ntpServer3;
	TimeZone = ntpTimeZone;
    _log = ptrLog;
    Serial.println("NTP is "+server1);
    #ifdef USEDS3231 //Инициализация часового модуля DS3231
        ds3231.init(fastI2C);
        if (! ds3231.begin() && _log != NULL) {
            _log->println(F("DS3231 not detected!"));
        }
    #endif

    if(server1.length() || server2.length() || server3.length()) {
        configTime(TimeZone*3600, 0, (char*)server1.c_str(), (server2.length() > 0)?(char*)server2.c_str(): nullptr, (server3.length() > 0)?(char*)server2.c_str(): nullptr);
    }
}

/**
 * Gets a time now 
 * @return time now in unix time format
 */
uint32_t RealTimeClock::getTime() {
  #ifdef USEDS3231
    return ds3231.get();
  #endif
  #ifndef USEDS3231 
    if ((WiFi.getMode() == WIFI_STA) && (server1.length() || server2.length() || server3.length()) && ((!lastNtpTime) || (ntpUpdateInterval && (millis() - lastNtpUpdate >= ntpUpdateInterval)))) {

		tm timeinfo;
		if(!getLocalTime(&timeinfo)){
			Serial.println("Failed to obtain time");
			return 0;
		}
        //   Serial.println(&timeinfo, "%A, %d %B %Y %H:%M:%S");
		// uint32_t now = combineUnixTime(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, timeinfo.tm_mday, timeinfo.tm_mon, timeinfo.tm_year);//sntp_get_current_timestamp() + (-8 + _ntpTimeZone) * 3600;
		time_t now;
		time(&now);
		now += TimeZone * 3600;
		if (now > 1483228800UL) { // 01.01.2017 0:00:00
			lastNtpTime = now;
			lastNtpUpdate = millis();
            if(_log != NULL){
                _log->print(dateTimeToStr(now));
                _log->println(F(" time updated successfully"));
            }
		}
    }
    if (lastNtpTime)
      return lastNtpTime + (millis() - lastNtpUpdate) / 1000;
    else
      return 0;
  #endif
}

/**
 * Set a time now 
 * @param now time in unix format to adjust date/time
 * @return time now in unix time format
 */
void RealTimeClock::setTime(uint32_t now) {
    #ifdef USEDS3231
        ds3231.set(now);
    #endif  


    #ifndef USEDS3231
        lastNtpTime = now;
        lastNtpUpdate = millis();
    #endif
    if(_log != NULL){
        _log->print(dateTimeToStr(now));
        _log->println(F(" time updated manualy"));
    }
}

#ifdef USEDS3231
void RealTimeClock::ds3231Syncronize(){
    if ((WiFi.getMode() == WIFI_STA) && (server1.length() || server2.length() || server3.length()) && ((!lastNtpTime) || (ntpUpdateInterval && (millis() - lastNtpUpdate >= ntpUpdateInterval)))) {
		tm timeinfo;
		if(!getLocalTime(&timeinfo)){
			Serial.println("Failed to obtain time");
			return;
		}
		time_t now;
		time(&now);
		now += TimeZone * 3600;

        ds3231.set(now);
    }
}
#endif
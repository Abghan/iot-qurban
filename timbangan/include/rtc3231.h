#ifndef RTC_ESP_H
#define RTC_ESP_H
#include <Arduino.h>

class RTC_ESP
{
private:
public:
    void rtc_setup();
    uint32_t get_unixtime();
    String get_timestamp();
    String get_rtc_temp();
    uint8_t get_hours();
    uint8_t get_minute();
    uint8_t get_second();

    uint8_t get_day();
    uint8_t get_month();
    uint16_t get_year();
    uint8_t get_dayOfTheWeek();
    void set_rtc(unsigned long int unix);
    
};
#endif
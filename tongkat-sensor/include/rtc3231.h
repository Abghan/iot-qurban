#ifndef RTC_ESP_H
#define RTC_ESP_H
#include <Arduino.h>

class RTC_ESP
{
private:
public:
    void rtc_setup();
    String get_date();
    uint32_t get_unixtime();
    String get_timestamp();
    String get_rtc_temp();
    void set_rtc(unsigned long int unix);
};
#endif
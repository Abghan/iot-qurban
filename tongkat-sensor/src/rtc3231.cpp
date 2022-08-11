// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include "RTClib.h"
#include "rtc3231.h"
#include "JRD100.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

/*
const long ONE_SECOND = 1000;      //DEFINE ONE SECOND
const long ONE_MINUTE = 60 * ONE_SECOND;   //DEFINE ONE MINUTE
const long ONE_HOUR = 60 * ONE_MINUTE;     //DEFINE ONE HOUR
*/

#define MY_TIMEZONE 7   // Zona waktu indonesi GMT+7
#define MY_TIMEZONE_IN_SECONDS (MY_TIMEZONE * 3600)

DateTime dt_now; 


void RTC_ESP::rtc_setup () {
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
  }
  
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  //rtc.adjust(DateTime(2021, 11, 11, 18, 06, 0));
  
}
uint32_t RTC_ESP::get_unixtime(void)
{
    dt_now = rtc.now();
    return dt_now.unixtime() - MY_TIMEZONE_IN_SECONDS; 
}
String RTC_ESP::get_timestamp()
{
    dt_now = rtc.now();
    return dt_now.timestamp(); 
}
String RTC_ESP::get_date()
{
    dt_now = rtc.now();
    uint8_t tgl = dt_now.day();
    uint8_t bulan = dt_now.month();
    uint16_t year = dt_now.year();

    String date = String(tgl) +String(bulan) + String(year);
    return date;
}
String RTC_ESP::get_rtc_temp()
{
    String temp="";
    temp +=  rtc.getTemperature();
    temp += " C";
    return temp;
}
void RTC_ESP::set_rtc(unsigned long int unix)
{     

    dt_now = unix + MY_TIMEZONE_IN_SECONDS; 
    rtc.adjust(dt_now);
    Serial.println(dt_now.unixtime());
}
// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include "RTClib.h"
#include "rtc3231.h"

RTC_DS3231 rtc;



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
  
  if (rtc.lostPower()) 
  {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
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

uint8_t RTC_ESP::get_hours()
{
    dt_now = rtc.now();
    return dt_now.hour(); 
}

uint8_t RTC_ESP::get_minute()
{
    dt_now = rtc.now();
    return dt_now.minute(); 
}

uint8_t RTC_ESP::get_second()
{
    dt_now = rtc.now();
    return dt_now.second(); 
}

uint8_t RTC_ESP::get_day()
{
    dt_now = rtc.now();
    return dt_now.day(); 
}

uint8_t RTC_ESP::get_month()
{
    dt_now = rtc.now();
    return dt_now.month(); 
}

uint16_t RTC_ESP::get_year()
{
    dt_now = rtc.now();
    return dt_now.year(); 
}

uint8_t RTC_ESP::get_dayOfTheWeek()
{
    dt_now = rtc.now();
    return dt_now.dayOfTheWeek(); 
}

/*
void rtc_loop () {
    DateTime now = rtc.now();

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");

    // calculate a date which is 7 days, 12 hours, 30 minutes, 6 seconds into the future
    DateTime future (now + TimeSpan(7,12,30,6));

    Serial.print(" now + 7d + 12h + 30m + 6s: ");
    Serial.print(future.year(), DEC);
    Serial.print('/');
    Serial.print(future.month(), DEC);
    Serial.print('/');
    Serial.print(future.day(), DEC);
    Serial.print(' ');
    Serial.print(future.hour(), DEC);
    Serial.print(':');
    Serial.print(future.minute(), DEC);
    Serial.print(':');
    Serial.print(future.second(), DEC);
    Serial.println();

    Serial.print("Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");

    Serial.println();
    delay(3000);
}
*/
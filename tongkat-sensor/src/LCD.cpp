/*********************************************************************
This is an example for our Monochrome OLEDs based on SH1106 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x64 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include "LCD.h"
#include "rtc3231.h"

RTC_ESP my_rtc;
//#define OLED_SDA 21
//#define OLED_SCL 22

Adafruit_SH1106 display(21, 22);
// Adafruit_SH1106 display;

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH 16

static const unsigned char PROGMEM logo_bmp[] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x83, 0xb0, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x03, 0x1d, 0x60, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x04, 0x6b, 0xa0, 0x02, 0x00,
        0x00, 0x00, 0x00, 0x0d, 0xad, 0x40, 0x04, 0x00,
        0x00, 0x00, 0x00, 0x1a, 0xdb, 0x80, 0x08, 0x00,
        0x00, 0x00, 0x00, 0x17, 0x6d, 0x00, 0x10, 0x00,
        0x00, 0x00, 0x00, 0x7a, 0xdb, 0x00, 0x20, 0x00,
        0x00, 0x00, 0x01, 0xad, 0xb5, 0x00, 0xc0, 0x00,
        0x00, 0x00, 0x0e, 0xd6, 0xde, 0x00, 0x80, 0x00,
        0x00, 0x00, 0x35, 0xbb, 0x6a, 0x03, 0x00, 0x00,
        0x00, 0x00, 0xd6, 0xd6, 0xb4, 0x04, 0x00, 0x00,
        0x00, 0x07, 0x6d, 0x7b, 0xdc, 0x0a, 0x00, 0x00,
        0x00, 0x14, 0x07, 0xad, 0x68, 0x14, 0x00, 0x00,
        0x00, 0x50, 0x00, 0x56, 0xb0, 0x28, 0x00, 0x00,
        0x02, 0x00, 0x00, 0x1b, 0xd0, 0x60, 0x00, 0x00,
        0x08, 0x00, 0x00, 0x05, 0x50, 0x80, 0x00, 0x00,
        0x00, 0x00, 0x10, 0x06, 0xe1, 0x80, 0x00, 0x00,
        0x00, 0x00, 0x40, 0x03, 0x5a, 0x80, 0x00, 0x00,
        0x00, 0x00, 0x80, 0x01, 0xb3, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x00, 0x01, 0x52, 0x00, 0x00, 0x00,
        0x00, 0x06, 0x00, 0x21, 0x9a, 0x00, 0x00, 0x00,
        0x00, 0x18, 0x00, 0x41, 0x04, 0x00, 0x00, 0x00,
        0x00, 0x28, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00,
        0x00, 0xd0, 0x00, 0x81, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x40, 0x01, 0x00, 0x00, 0x85, 0x10, 0x20,
        0x03, 0x00, 0x01, 0x02, 0x1d, 0x31, 0x10, 0x20,
        0x00, 0x00, 0x02, 0x00, 0x24, 0x26, 0xa0, 0x00,
        0x00, 0x00, 0x06, 0x04, 0x00, 0x28, 0x48, 0x40,
        0x00, 0x00, 0x04, 0x00, 0x08, 0x20, 0x48, 0x40,
        0x00, 0x00, 0x08, 0x04, 0x08, 0x40, 0xa4, 0x40,
        0x00, 0x00, 0x18, 0x00, 0x08, 0x42, 0x02, 0x00,
        0x00, 0x00, 0x10, 0x00, 0x08, 0x04, 0x40, 0x80,
        0x00, 0x00, 0x30, 0x00, 0x10, 0x90, 0x41, 0x80,
        0x00, 0x00, 0x20, 0x00, 0x10, 0xc0, 0x81, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0xa4, 0x08, 0x90, 0x68, 0x04, 0x00, 0x00,
        0x00, 0x08, 0xd5, 0x24, 0x13, 0x52, 0x95, 0x00,
        0x00, 0xa6, 0x49, 0x12, 0x52, 0x25, 0x4a, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x08, 0x00, 0x01, 0x12, 0x00, 0x40, 0xec, 0x00,
        0x0a, 0xad, 0x9a, 0x54, 0x6e, 0x40, 0x80, 0x00,
        0x02, 0x22, 0x51, 0x11, 0x11, 0x28, 0x8a, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const unsigned char PROGMEM logo16_glcd_bmp[] =
    {B00000000, B11000000,
     B00000001, B11000000,
     B00000001, B11000000,
     B00000011, B11100000,
     B11110011, B11100000,
     B11111110, B11111000,
     B01111110, B11111111,
     B00110011, B10011111,
     B00011111, B11111100,
     B00001101, B01110000,
     B00011011, B10100000,
     B00111111, B11100000,
     B00111111, B11110000,
     B01111100, B11110000,
     B01110000, B01110000,
     B00000000, B00110000};

#if (SH1106_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SH1106.h!");
#endif

void lcd::setup()
{
  my_rtc.rtc_setup();
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SH1106_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3D (for the 128x64)
  // init done

#ifdef TEST
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();

  // draw a single pixel
  display.drawPixel(10, 10, WHITE);
  // Show the display buffer on the hardware.
  // NOTE: You _must_ call display after making any drawing commands
  // to make them visible on the display hardware!
  display.display();
  delay(2000);
  display.clearDisplay();

  // draw many lines
  testdrawline();
  display.display();
  delay(2000);
  display.clearDisplay();

  // draw rectangles
  testdrawrect();
  display.display();
  delay(2000);
  display.clearDisplay();

  // draw multiple rectangles
  testfillrect();
  display.display();
  delay(2000);
  display.clearDisplay();

  // draw mulitple circles
  testdrawcircle();
  display.display();
  delay(2000);
  display.clearDisplay();

  // draw a white circle, 10 pixel radius
  display.fillCircle(display.width() / 2, display.height() / 2, 10, WHITE);
  display.display();
  delay(2000);
  display.clearDisplay();

  testdrawroundrect();
  delay(2000);
  display.clearDisplay();

  testfillroundrect();
  delay(2000);
  display.clearDisplay();

  testdrawtriangle();
  delay(2000);
  display.clearDisplay();

  testfilltriangle();
  delay(2000);
  display.clearDisplay();

  // draw the first ~12 characters in the font
  testdrawchar();
  display.display();
  delay(2000);
  display.clearDisplay();

  // draw scrolling text
  testscrolltext();
  delay(2000);
  display.clearDisplay();

  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Hello, world!");
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.println(3.141592);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("0x");
  display.println(0xDEADBEEF, HEX);
  display.display();
  delay(20000);
#endif
  // miniature bitmap display
  display.clearDisplay();

  display.drawBitmap(
      (display.width() - 60) / 2,
      (display.height() - 60) / 2,
      logo_bmp, 60, 60, 1);
  display.display();
  delay(500);

#ifdef TEST
  // invert the display
  display.invertDisplay(true);
  delay(1000);
  display.invertDisplay(false);
  delay(1000);

  // draw a bitmap icon and 'animate' movement
  testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
  Serial.println("LCD SETUP");
#endif
}

int lcd::getBarsSignal(long rssi)
{
  // 5. High quality: 90% ~= -55db
  // 4. Good quality: 75% ~= -65db
  // 3. Medium quality: 50% ~= -75db
  // 2. Low quality: 30% ~= -85db
  // 1. Unusable quality: 8% ~= -96db
  // 0. No signal
  int bars;

  if ((rssi > -55) && (rssi != 0))
  {
    bars = 5;
  }
  else if (rssi<-55 & rssi> - 65)
  {
    bars = 4;
  }
  else if (rssi<-65 & rssi> - 75)
  {
    bars = 3;
  }
  else if (rssi<-75 & rssi> - 85)
  {
    bars = 2;
  }
  else if (rssi<-85 & rssi> - 96)
  {
    bars = 1;
  }
  else
  {
    bars = 0;
  }
  return bars;
}

void lcd::disp_bar()
{
  display.clearDisplay();

  if (wifi_is_connected)
  {
    int signal = getBarsSignal(val_rssi);

    display.setTextColor(WHITE, BLACK);
    display.setTextSize(1);  // Normal 1:1 pixel scale
    display.setCursor(0, 0); // Start at top-left corner
    display.println(my_rtc.get_timestamp());

    for (int b = 0; b <= signal; b++)
      display.fillRect(110 + (b * 3), 16 - (b * 3), 2, b * 3, WHITE);
  }

  else
  {
    display.setTextColor(WHITE, BLACK);
    display.setTextSize(1);  // Normal 1:1 pixel scale
    display.setCursor(0, 0); // Start at top-left corner
    // display.println("WIFI OFF");
    display.println(my_rtc.get_timestamp());
  }
}

void lcd::disp_idle()
{
    disp_bar();

  if (mqtt_status)
  {
    display.setTextSize(2);      // Normal 1:1 pixel scale
    display.setTextColor(WHITE); // Draw white text
    if(temperature_ok || rfid_ok)
    {
      display.setCursor(0, 16);    // Start at top-left corner
      display.println(val_suhu);
    
      display.setCursor(0, 34);    // Start at top-left corner
      display.println(val_rfid);
    }
    if(!temperature_ok && !rfid_ok) 
    {
      display.setCursor(0, 16); 
      display.println("ONLINE");
    }
  }

  else
  {
    display.setTextSize(2);      // Normal 1:1 pixel scale
    display.setTextColor(WHITE); // Draw white text
    if(temperature_ok || rfid_ok)
    {
      display.setCursor(0, 16);    // Start at top-left corner
      display.println(val_suhu);

      display.setCursor(0, 34);    // Start at top-left corner
      display.println(val_rfid);
    }
    if(!temperature_ok && !rfid_ok)
    {
      display.setCursor(0, 16); 
      display.println("OFFLINE");
    }
  }
  display.display();
}
void lcd::draw_idle()
{
  if (on_draw_notif)
  {
    static uint32_t last_time; // last execution time
    uint32_t now = millis();
    if (now - last_time >= 1200)
    {
      last_time = now;
      disp_idle();
      on_draw_notif = false;
    }
  }
  else disp_idle();
}

void lcd::draw_notif(String header, String baris1, String baris2)
{
  display.clearDisplay();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.println(header);

  display.setCursor(0, 16); // Start at top-left corner
  display.println(baris1);

  display.setCursor(0, 34); // Start at top-left corner
  display.println(baris2);
  display.display();

  on_draw_notif = true;
}
void lcd::loop()
{
  draw_idle();
}

void lcd::testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h)
{
  uint8_t icons[NUMFLAKES][3];

  // initialize
  for (uint8_t f = 0; f < NUMFLAKES; f++)
  {
    icons[f][XPOS] = random(display.width());
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;

    Serial.print("x: ");
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(" y: ");
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(" dy: ");
    Serial.println(icons[f][DELTAY], DEC);
  }

  while (1)
  {
    // draw each icon
    for (uint8_t f = 0; f < NUMFLAKES; f++)
    {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, WHITE);
    }
    display.display();
    delay(200);

    // then erase it + move it
    for (uint8_t f = 0; f < NUMFLAKES; f++)
    {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > display.height())
      {
        icons[f][XPOS] = random(display.width());
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = random(5) + 1;
      }
    }
  }
}
void lcd::testdrawchar(void)
{
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  for (uint8_t i = 0; i < 168; i++)
  {
    if (i == '\n')
      continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }
  display.display();
}
void lcd::testdrawcircle(void)
{
  for (int16_t i = 0; i < display.height(); i += 2)
  {
    display.drawCircle(display.width() / 2, display.height() / 2, i, WHITE);
    display.display();
  }
}
void lcd::testfillrect(void)
{
  uint8_t color = 1;
  for (int16_t i = 0; i < display.height() / 2; i += 3)
  {
    // alternate colors
    display.fillRect(i, i, display.width() - i * 2, display.height() - i * 2, color % 2);
    display.display();
    color++;
  }
}
void lcd::testdrawtriangle(void)
{
  for (int16_t i = 0; i < min(display.width(), display.height()) / 2; i += 5)
  {
    display.drawTriangle(display.width() / 2, display.height() / 2 - i,
                         display.width() / 2 - i, display.height() / 2 + i,
                         display.width() / 2 + i, display.height() / 2 + i, WHITE);
    display.display();
  }
}
void lcd::testfilltriangle(void)
{
  uint8_t color = WHITE;
  for (int16_t i = min(display.width(), display.height()) / 2; i > 0; i -= 5)
  {
    display.fillTriangle(display.width() / 2, display.height() / 2 - i,
                         display.width() / 2 - i, display.height() / 2 + i,
                         display.width() / 2 + i, display.height() / 2 + i, WHITE);
    if (color == WHITE)
      color = BLACK;
    else
      color = WHITE;
    display.display();
  }
}
void lcd::testdrawroundrect(void)
{
  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2)
  {
    display.drawRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, WHITE);
    display.display();
  }
}
void lcd::testfillroundrect(void)
{
  uint8_t color = WHITE;
  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2)
  {
    display.fillRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, color);
    if (color == WHITE)
      color = BLACK;
    else
      color = WHITE;
    display.display();
  }
}
void lcd::testdrawrect(void)
{
  for (int16_t i = 0; i < display.height() / 2; i += 2)
  {
    display.drawRect(i, i, display.width() - 2 * i, display.height() - 2 * i, WHITE);
    display.display();
  }
}
void lcd::testdrawline()
{
  for (int16_t i = 0; i < display.width(); i += 4)
  {
    display.drawLine(0, 0, i, display.height() - 1, WHITE);
    display.display();
  }
  for (int16_t i = 0; i < display.height(); i += 4)
  {
    display.drawLine(0, 0, display.width() - 1, i, WHITE);
    display.display();
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i = 0; i < display.width(); i += 4)
  {
    display.drawLine(0, display.height() - 1, i, 0, WHITE);
    display.display();
  }
  for (int16_t i = display.height() - 1; i >= 0; i -= 4)
  {
    display.drawLine(0, display.height() - 1, display.width() - 1, i, WHITE);
    display.display();
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i = display.width() - 1; i >= 0; i -= 4)
  {
    display.drawLine(display.width() - 1, display.height() - 1, i, 0, WHITE);
    display.display();
  }
  for (int16_t i = display.height() - 1; i >= 0; i -= 4)
  {
    display.drawLine(display.width() - 1, display.height() - 1, 0, i, WHITE);
    display.display();
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i = 0; i < display.height(); i += 4)
  {
    display.drawLine(display.width() - 1, 0, 0, i, WHITE);
    display.display();
  }
  for (int16_t i = 0; i < display.width(); i += 4)
  {
    display.drawLine(display.width() - 1, 0, i, display.height() - 1, WHITE);
    display.display();
  }
  delay(250);
}
void lcd::testscrolltext(void)
{
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.clearDisplay();
  display.println("scroll");
  display.display();

  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
}

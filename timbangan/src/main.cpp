#include <Arduino.h>
#include <weight_A12E.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <FlickerFreePrint.h>
#include "rtc3231.h"
#include <SPIFFS.h>
#include "ArduinoJson.h"
#include "myWifii.h"
#include "logo.h"
#include "time.h"
#include "Fonts\FreeSerif9pt7b.h"
#include "Fonts\FreeSerif24pt7b.h"
#include "jrd100.h"
#include "variableGlobal.h"
#include "pitches.h"
#include <PubSubClient.h>
#include "EEPROM.h"

note_t mun;
#define TFT_DC 15
#define TFT_CS 2
#define TFT_RST 4

// setup some colors
#define C_BLACK 0x0000
#define C_BLUE 0x001F
#define C_RED 0xF800
#define C_GREEN 0x07E0
#define C_MAGENTA 0xF81F
#define C_YELLOW 0xFFE0
#define C_WHITE 0xFFFF
#define C_ABUABU 0x10A3 // 0x39E7//0x9CF2
#define C_COKLAT 0x6145
#define C_COKLAT2 0x6B06
#define C_CYAN 0x01C7 // 0x0AEC//0x1B6E//0x4535//0x4619//0x469B//0x5F3D
#define C_NOTIF 0x65FB
#define C_YELLOWDARK 0x6B62
#define C_REDDARK 0x78C0

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);
FlickerFreePrint<Adafruit_ILI9341> Data1(&tft, C_WHITE, C_BLACK);
FlickerFreePrint<Adafruit_ILI9341> Data2(&tft, C_WHITE, C_BLACK);
FlickerFreePrint<Adafruit_ILI9341> Data3(&tft, C_WHITE, C_BLACK);
// Adafruit_ILI9341 tft (TFT_CS, TFT_DC, TFT_RST);
RTC_ESP my_rtc;
char daysOfTheWeek[7][12] = {"MINGGU", "SENIN", "SELASA", "RABU", "KAMIS", "JUM'AT", "SABTU"};
char my_mounth[12][12] = {"JANUARI", "FEBUARI", "MARET", "APRIL", "MEI", "JUNI", "JULY", "AGUSTUS", "SEPTEMBER", "OKTOBER", "NOVEMBER", "DESEMBER"};

Weight_A12E Timbangan;
String weight_;

JRD100 rfid;

char str_clock[100];
char str_weight[10];

void initWiFi();
void setup_ap();

void createArray(const char *filename);
void jpegInfo();
void jpegRender(int xpos, int ypos);
void drawFSJpeg(const char *filename, int xpos, int ypos);

bool SPIFFSInited = false;
int imageIndex = 0;
// bool state_Wifii=false;

bool state_rtc = false;
my my_;

const char *ntpServer = "pool.ntp.org";
unsigned long epochTime;

void show_clock();
void show_indiWifii();
void show_indiMqtt();
void update_rtc();
void Draw_frame();
void Draw_idKartu();
unsigned long getTime();

long prevMillis = 0;
long pMillis = 0;
long sendMillis=0;
// String weight_send;
const int ledPin = 14;
const int freq = 2637;
const int ledChannel = 0;
const int ledChannel1 = 1;
const int resolution = 8;

void ToneBuzzer();
void ToneBuzzer_weight();
void ToneStart();
int melody[] = {NOTE_C5, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_G4, 0, NOTE_B4, NOTE_C5};
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};

int melody_weight[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5};
int melody_rfid[] =   {NOTE_E4, NOTE_F4, NOTE_G4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_E4, NOTE_F4};

const int buttonPin1 = 27;
const int buttonPin2 = 26;

int buttonState, buttonState2;
int lastButtonState = HIGH, lastButtonState2 = HIGH;

unsigned long lastDebounceTime = 0, lastDebounceTime2 = 0;
unsigned long debounceDelay = 50;

void Button1();
void Button2();
void Draw_lock_weight_true();
void Draw_lock_weight_false();
void Cek_Param();
void Draw_notif();
void Draw_start();
void Cek_Kroscek();
void test_interval();
void Draw_send_ok();
void Draw_send_false();
void Draw_timbangan();
bool cek_param = false;
uint8_t cnt=0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);

  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("\0D\0A");
  Serial.println("Starting........");

  if (!EEPROM.begin(32)) 
  {
    delay(1000);
  }

  Msg_id=EEPROM.readULong64(0);
  Serial.print("Msg_id= ");
  Serial.println(Msg_id);

  rfid.beginJRD100();
  Timbangan.begin();
  my_rtc.rtc_setup();

  tft.begin();
  tft.fillScreen(C_BLACK);
  tft.setRotation(1);

  load_config_file();

  initWiFi();
  webServer();
  Draw_frame();
  ToneStart();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  show_clock();
  show_indiWifii();
  show_indiMqtt();
  update_rtc();
  my_.ReCon();
  webServerOn();

  //if ((WiFi.status() == WL_CONNECTED))
  //{
  my_.loop_mqtt();
  //}

  Draw_timbangan();
  rfid.measure_jrd100();
  
  if (millis() - prevMillis >= 100)
  {
    prevMillis = millis();
    rfid.send_cmd();
  }

  if (rfid.trig_dataID == true)
  {
    rfid.trig_dataID = false;
    Draw_idKartu();
  }

  if (send_ok == true)
  {
    send_ok = false;
    ULong_EPC1 = 0;
    ULong_EPC2 = 0;
    ULong_EPC3 = 0;
    ULong_EPC4 = 0;
    ULong_EPC5 = 0;
    Draw_idKartu();
  }

  if (lock_state == true)
  {
    digitalWrite(5, LOW);
  }
  else
  {
    digitalWrite(5, HIGH);
  }

  ToneBuzzer();
  ToneBuzzer_weight();
  Button1();
  Button2();
  Cek_Param();

  if(send_ok_mqtt==true)
  {
    Draw_send_ok();
  }

  if(send_ok_mqtt==false && save_data==true)
  {
    Draw_send_false();
  }
  //test_interval();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void show_clock()
{
  tahun = my_rtc.get_year();
  bulan = my_rtc.get_month();
  hari = my_rtc.get_day();
  jam = my_rtc.get_hours();
  menit = my_rtc.get_minute();
  detik = my_rtc.get_second();

  sprintf(str_clock, "%S, %02d %S %04d    %02d:%02d:%02d", daysOfTheWeek[my_rtc.get_dayOfTheWeek()], my_rtc.get_day(), my_mounth[my_rtc.get_month()], my_rtc.get_year(), my_rtc.get_hours(), my_rtc.get_minute(), my_rtc.get_second());
  tft.setFont(&FreeSerif9pt7b);
  tft.setTextSize(1);
  tft.setCursor(2, 234);
  Data1.setTextColor(C_WHITE, C_BLACK);
  Data1.print(str_clock);
}

void show_indiWifii()
{
  if ((WiFi.status() != WL_CONNECTED)) // && state_Wifii==false)
  {
    tft.drawBitmap(281, 1, wifi39x30, 39, 30, C_RED);
    // state_Wifii=true;
    // Serial.println(state_Wifii);
    // Serial.println("masuk1");
  }
  if ((WiFi.status() == WL_CONNECTED)) // && state_Wifii==true)
  {
    tft.drawBitmap(281, 1, wifi39x30, 39, 30, C_GREEN);
    // state_Wifii=false;
    // Serial.println(state_Wifii);
    // Serial.println("masuk2");
  }
}

void show_indiMqtt()
{
  if (state_mqtt == false)
  {
    tft.drawBitmap(238, 1, mosquitto41x38, 41, 38, C_RED);
  }
  if (state_mqtt == true)
  {
    tft.drawBitmap(238, 1, mosquitto41x38, 41, 38, C_GREEN);
  }
}

// Function that gets current epoch time
unsigned long getTime()
{
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    // Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}

void update_rtc()
{
  if ((WiFi.status() == WL_CONNECTED) && state_rtc == false)
  {
    // delay(100);
    state_rtc = true;
    configTime(0, 0, ntpServer);
    epochTime = getTime();
    Serial.print("Epoch Time: ");
    Serial.println(epochTime);
    if (epochTime != 0)
    {
      // state_rtc=true;
      my_rtc.set_rtc(epochTime);
    }
    // delay(100);
  }
}

void Draw_frame()
{
  tft.fillRoundRect(2, 70, 156, 130, 4, C_CYAN);
  tft.fillRoundRect(162, 70, 156, 130, 4, C_ABUABU);

  tft.setFont();
  tft.setTextSize(1);
  tft.setCursor(10, 75);
  tft.setTextColor(C_WHITE, C_CYAN);
  tft.print("WEIGHT");
  tft.drawLine(10, 85, 44, 85, C_WHITE);

  tft.setCursor(170, 75);
  tft.setTextColor(C_WHITE, C_ABUABU);
  tft.print("ID KARTU");
  tft.drawLine(170, 85, 217, 85, C_WHITE);

  tft.drawBitmap(281, 1, wifi39x30, 39, 30, C_RED);
  tft.drawBitmap(238, 1, mosquitto41x38, 41, 38, C_RED);

  tft.setFont();
  tft.setTextSize(2);
  tft.setTextColor(C_WHITE, C_ABUABU);

  tft.setCursor(165, 95);
  tft.print("1.");
  tft.setCursor(165, 115);
  tft.print("2.");
  tft.setCursor(165, 135);
  tft.print("3.");
  tft.setCursor(165, 155);
  tft.print("4.");
  tft.setCursor(165, 175);
  tft.print("5.");

  tft.setFont(&FreeSerif9pt7b);
  tft.setTextSize(1);
  tft.setTextColor(C_WHITE, C_CYAN);
  tft.setCursor(60, 180);
  tft.print("( Kg )");

  //sprintf(str_weight, "%S", weight_send);
  tft.setFont(&FreeSerif24pt7b);
  tft.setTextSize(1);
  tft.setCursor(12, 140);
  Data2.setTextColor(C_WHITE, C_CYAN);
  Data2.print("");
}

void Draw_idKartu()
{
  tft.fillRoundRect(162, 70, 156, 130, 4, C_ABUABU);
  tft.setFont();
  tft.setTextSize(1);
  tft.setCursor(170, 75);
  tft.setTextColor(C_WHITE, C_ABUABU);
  tft.print("ID KARTU");
  tft.drawLine(170, 85, 217, 85, C_WHITE);
  tft.setFont();
  tft.setTextSize(2);
  tft.setTextColor(C_WHITE, C_ABUABU);

  tft.setCursor(165, 95);
  tft.print("1.");
  tft.setCursor(165, 115);
  tft.print("2.");
  tft.setCursor(165, 135);
  tft.print("3.");
  tft.setCursor(165, 155);
  tft.print("4.");
  tft.setCursor(165, 175);
  tft.print("5.");
  //////////////////////////////////////////////////
  tft.setFont();
  tft.setTextSize(2);
  Data3.setTextColor(C_WHITE, C_ABUABU);

  tft.setCursor(195, 95);
  Data3.print(ULong_EPC1);

  tft.setCursor(195, 115);
  Data3.print(ULong_EPC2);

  tft.setCursor(195, 135);
  Data3.print(ULong_EPC3);

  tft.setCursor(195, 155);
  Data3.print(ULong_EPC4);

  tft.setCursor(195, 175);
  Data3.print(ULong_EPC5);
}

void ToneBuzzer()
{
  //ledcSetup(ledChannel, NOTE_C5, resolution);
  if (Buzz == true)
  {
    ledcSetup(ledChannel1, NOTE_C5, resolution);
    ledcAttachPin(ledPin, ledChannel1);
    //ledcWrite(ledChannel1, 240);
    for (int thisNote = 0; thisNote < 8; thisNote++)
    {
      ledcSetup(ledChannel1, melody_rfid[thisNote], resolution);
      ledcWrite(ledChannel1, 240);
      delay(50);
      ledcWrite(ledChannel1, 0);
    }

    ledcWrite(ledChannel1, 0);
    Buzz = false;
    //Serial.println("masuk");
  }
  else
  {
    ledcWrite(ledChannel1, 0);
  }
}

void ToneBuzzer_weight()
{
  if (Buzz_weight == true)
  {
    ledcSetup(ledChannel, NOTE_B5, resolution);
    ledcAttachPin(ledPin, ledChannel);
    //ledcSetup(ledChannel, NOTE_B5, resolution);
    //ledcWrite(ledChannel, 240);
    for (int thisNote = 0; thisNote < 8; thisNote++)
    {
      ledcSetup(ledChannel, melody_weight[thisNote], resolution);
      ledcWrite(ledChannel, 240);
      delay(50);
      ledcWrite(ledChannel, 0);
    }

    ledcWrite(ledChannel, 0);
    Buzz_weight = false;
  }
  else
  {
    ledcWrite(ledChannel, 0);
  }
}

void ToneStart()
{
  ledcSetup(ledChannel, NOTE_C5, resolution);
  ledcAttachPin(ledPin, ledChannel);

  for (int thisNote = 0; thisNote < 8; thisNote++)
  {
    int noteDuration = 1000 / noteDurations[thisNote];
    ledcSetup(ledChannel, melody[thisNote], resolution);
    ledcWrite(ledChannel, 240);
    int pauseBetweenNotes = noteDuration *1.3;
    delay(pauseBetweenNotes);
    ledcWrite(ledChannel, 0);
  }

  ledcWrite(ledChannel, 0);
  
}

void Button1()
{
  int reading = digitalRead(buttonPin1);

  if (reading != lastButtonState)
  {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    if (reading != buttonState)
    {
      buttonState = reading;
      if (buttonState == LOW)
      {
        if (cek_param == true)
        {
          sendData_mqtt = true;
          Serial.println("masuk");
          Serial.println(sendData_mqtt);
        }
        else
        {
          Cek_Kroscek();
        }

        // Draw_notif();
      }
    }
  }

  lastButtonState = reading;
}

void Button2()
{
  int reading = digitalRead(buttonPin2);

  if (reading != lastButtonState2)
  {
    lastDebounceTime2 = millis();
  }

  if ((millis() - lastDebounceTime2) > debounceDelay)
  {
    if (reading != buttonState2)
    {
      buttonState2 = reading;
      if (buttonState2 == LOW)
      {
        lock_state = !lock_state;

        if (lock_state == true)
        {
          Draw_lock_weight_true();
        }

        if (lock_state == false)
        {
          Draw_lock_weight_false();
        }

        // Draw_start();
        /*if(weight_send !=0 && lock_state==false)
        {
          Draw_lock_weight();
        }*/

        // Serial.println(lock_state);
      }
    }
  }

  lastButtonState2 = reading;
}

void Draw_lock_weight_true()
{
  tft.setFont(&FreeSerif9pt7b);
  tft.setTextSize(1);
  tft.setCursor(240, 60);
  tft.setTextColor(C_RED, C_BLACK);
  tft.print("L O C K");
}

void Draw_lock_weight_false()
{
  tft.setFont(&FreeSerif9pt7b);
  tft.setTextSize(1);
  tft.setCursor(240, 60);
  tft.setTextColor(C_BLACK, C_BLACK);
  tft.print("L O C K");
}

void Cek_Param()
{
  if (lock_state == true && weight_send != "" && weight_send != "0.0" && rfid_send != "" )//&& state_mqtt == true && (WiFi.status() == WL_CONNECTED))
  {
    cek_param = true;
  }

  if (lock_state == false || weight_send == "" || weight_send == "0.0"|| rfid_send == "" )//|| state_mqtt == false || (WiFi.status() != WL_CONNECTED))
  {
    cek_param = false;
  }
}

void Draw_notif()
{
  for (int i = 1; i <= 100; i++)
  {
    // tft.fillScreen(C_BLACK);
    tft.fillRoundRect(160 - i, 120 - i, i + i, i + i, 6, C_NOTIF);
    delay(5);
  }

  tft.setFont(&FreeSerif9pt7b);
  tft.setTextSize(1);
  tft.setCursor(100, 50);
  tft.setTextColor(C_RED, C_BLACK);
  tft.print("W A R N I N G");

  if (lock_state == false)
  {
    tft.setCursor(62, 80);
    tft.setTextColor(C_BLACK, C_WHITE);
    tft.print("DATA BELUM DI-LOCK");
  }

  if (weight_send == "" || weight_send == "0.0")
  {
    tft.setCursor(62, 105);
    tft.setTextColor(C_BLACK, C_WHITE);
    tft.print("DATA BERAT NULL");
  }

  if (rfid_send == "")
  {
    tft.setCursor(62, 130);
    tft.setTextColor(C_BLACK, C_WHITE);
    tft.print("DATA RFID NULL");
  }

  if (state_mqtt == false)
  {
    tft.setCursor(62, 155);
    tft.setTextColor(C_BLACK, C_WHITE);
    tft.print("MQTT DISCONNEC");
  }

  if ((WiFi.status() != WL_CONNECTED))
  {
    tft.setCursor(62, 180);
    tft.setTextColor(C_BLACK, C_WHITE);
    tft.print("WIFFI DISCONNEC");
  }

  tft.setCursor(100, 200);
  tft.setTextColor(C_RED, C_BLACK);
  tft.print("GAGAL KIRIM");
}

void Draw_start()
{
  tft.fillScreen(C_BLACK);
  Draw_frame();
  Draw_idKartu();
  send_ok_weight=true;
  if (lock_state == true)
  {
    Draw_lock_weight_true();
  }

  if (lock_state == false)
  {
    Draw_lock_weight_false();
  }
}

void Cek_Kroscek()
{
  if (cek_param == false)
  {
    Draw_notif();
    delay(5000);
    Draw_start();
  }
}

void Draw_send_ok()
{
  if(millis()-sendMillis > 1000)
  {
    sendMillis=millis();
    cnt++;
    if(cnt>=3)
    {
      send_ok_mqtt=false;
      cnt=0;
      tft.fillRoundRect(5, 5, 70, 50, 4, C_BLACK);
      tft.setFont();
      tft.setTextSize(2);
      tft.setTextColor(C_BLACK, C_BLACK);

      tft.setCursor(16,11);
      tft.print("SEND");
      tft.setCursor(28,33);
      tft.print("OK");
    }
    else
    {
      tft.fillRoundRect(5, 5, 70, 50, 4, C_GREEN);
      tft.setFont();
      tft.setTextSize(2);
      tft.setTextColor(C_BLACK, C_GREEN);

      tft.setCursor(16,11);
      tft.print("SEND");
      tft.setCursor(28,33);
      tft.print("OK");
    }
  }
}

void Draw_send_false()
{
  if(millis()-sendMillis > 1000)
  {
    sendMillis=millis();
    cnt++;
    if(cnt>=3)
    {
      send_ok_mqtt=false;
      save_data=false;
      cnt=0;
      tft.fillRoundRect(5, 5, 70, 50, 4, C_BLACK);
      tft.setFont();
      tft.setTextSize(2);
      tft.setTextColor(C_BLACK, C_BLACK);

      tft.setCursor(16,11);
      tft.print("SAVE");
      tft.setCursor(17,33);
      tft.print("FILE");
    }
    else
    {
      tft.fillRoundRect(5, 5, 70, 50, 4, C_RED);
      tft.setFont();
      tft.setTextSize(2);
      tft.setTextColor(C_BLACK, C_RED);

      tft.setCursor(16,11);
      tft.print("SAVE");
      tft.setCursor(17,33);
      tft.print("FILE");
    }
  }
}

void test_interval()
{
  if(millis()-pMillis > 5000)
  {
    pMillis=millis();
    sendData_mqtt_dummy=true;
  }

}

void Draw_timbangan()
{
  if (Timbangan.measure())
  {
    if (lock_state == false)
    {
      sprintf(str_weight, "%S", weight_send);
      tft.setFont(&FreeSerif24pt7b);
      tft.setTextSize(1);
      tft.setCursor(12, 140);
      Data2.setTextColor(C_YELLOW, C_CYAN);
      Data2.print(str_weight);
    }
  }

  if(send_ok_weight==true)
  {
    send_ok_weight=false;
    Serial.println(weight_send);
    //weight_send="0.0";
    sprintf(str_weight, "%S", weight_send);
    tft.setFont(&FreeSerif24pt7b);
    tft.setTextSize(1);
    tft.setCursor(12, 140);
    Data2.setTextColor(C_YELLOW, C_CYAN);
    Data2.print(str_weight);

  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
/*void onConnectionEstablished()
{
  // Subscribe to "mytopic/test" and display received message to Serial
  my_client.subscribe("iot/test", [](const String & payload) {
    Serial.println(payload);
  });

  // Subscribe to "mytopic/wildcardtest/#" and display received message to Serial
  my_client.subscribe("iot/test", [](const String & topic, const String & payload) {
    Serial.println("(From wildcard) topic: " + topic + ", payload: " + payload);
  });

  // Publish a message to "mytopic/test"
  my_client.publish("iot/test", "This is a message"); // You can activate the retain flag by setting the third parameter to true

  // Execute delayed instructions
  my_client.executeDelayed(5 * 1000, []() {
    my_client.publish("iot/test", "This is a message sent 5 seconds later");
  });
}

void initWiFi()
{
  // char ssid_array[cfg.ssid.length()+1];
  // char pass_ssid_array[cfg.password.length()];

  // cfg.ssid.toCharArray(ssid_array, cfg.ssid.length());
  // cfg.password.toCharArray(pass_ssid_array, cfg.password.length());

  WiFi.mode(WIFI_STA);

  // WiFi.begin(ssid_array, pass_ssid_array);
  WiFi.begin(cfg.ssid.c_str(), cfg.password.c_str());
  Serial.println("Connecting to WiFi ..");
  // LCD.disp_temp("STATUS:", "WIFI-INIT", 0, 2);
  Serial.println(cfg.ssid.c_str());
  Serial.println(cfg.password.c_str());

  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval))
  {
    // Serial.print(millis());

    Serial.println("Reconnecting to WiFi...");
    // LCD.disp_temp("STATUS:", "WIFI-OFF", 0, 2);
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
  Serial.println(WiFi.localIP());
}

void setup_ap()
{
  //LCD.form.mode_device = md_suhu;

  if (!WiFi.config(local_IP, gateway, subnet))
  {
    Serial.println("Failed to configure static IP");
  }

  initWiFi();

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Connection established!");
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());
    //mqtt_broker_on_check();
  }
}*/

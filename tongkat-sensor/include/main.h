#include <SPIFFS.h>
#include "Reactduino.h"
#include "ArduinoJson.h"
#include "EspMQTTClient.h"
#include "JRD100.h"
#include "temperature.h"
#include "rtc3231.h"
#include "logging.h"
//#include <nvs.h>
#include <nvs_flash.h>
#include "pitch.h"
//#include "LCD.h"



#define leddbg 5
#define buzzer 18

#define DEBUG false
bool state = false;
int hardware=0;
//Analog pin proximity sensor 
const int portPin = 34;
bool object_detected;
bool buzzer_on_progress = false;

//1,2,4,3

bool on_hold_button = false;
bool proxymity_ondetect = false;

int ProximityValue = 0;

unsigned long previousMillis = 0;
unsigned long interval = 3000;

int8_t thermo_cnt_data_scan = 0;
int8_t rfid_cnt_data_scan = 0;

String temperature_data_on_hold;
String rfid_data_on_hold;

bool send_data_suhu = false;
bool send_data_rfid = false;

long number_data_rfid;                      // we want to keep these values after reset
long number_send_id = 1;

String last_date;

int32_t no_urut_data = 0; // value will default to 0, if not set yet in NVS
esp_err_t err ;

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

//##############**"Mario underworld" **##############//
int MarioUW_note[] = {
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,NOTE_AS3, NOTE_AS4, 0, 0,
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4, NOTE_AS3, NOTE_AS4, 0,0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,NOTE_DS3, NOTE_DS4, 0, 0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,NOTE_DS3, NOTE_DS4, 0,
  0, NOTE_DS4, NOTE_CS4, NOTE_D4,
  NOTE_CS4, NOTE_DS4, NOTE_DS4, NOTE_GS3, NOTE_G3, NOTE_CS4,
  NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
  NOTE_GS4, NOTE_DS4, NOTE_B3,  NOTE_AS3, NOTE_A3, NOTE_GS3,0, 0, 0
};

int MarioUW_note1[] = {
  NOTE_GS4, NOTE_DS4, NOTE_B3,  NOTE_AS3, NOTE_A3, NOTE_GS3,0, 0, 0
};

int MarioUW_duration1[] = {
  6, 18, 18, 18, 18, 18
};

int MarioUW_note2[] = {
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,NOTE_DS3, NOTE_DS4, 0, 0
};

int MarioUW_duration2[] = {
 12, 12, 12, 12, 12, 12, 6
};

//56
int MarioUW_duration[] = {
 12, 12, 12, 12,12, 12, 6,  3,
 12, 12, 12, 12, 12, 12, 6, 3,
 12, 12, 12, 12, 12, 12, 6, 3,
 12, 12, 12, 12,
 12, 12, 6, 6, 18, 18, 
 18, 6, 6, 6, 6, 6, 
 6, 18, 18, 18, 18, 18
};
//###########End of Mario underworld#############//

//lcd LCD;
temperature Temperature;
JRD100 RFIDReader;
RTC_ESP MyRTC; 
logging Log;

IPAddress local_IP;
IPAddress gateway;
IPAddress subnet;

reaction r_temperature;
reaction r_rfid;
reaction r_button;
reaction r_buzzer;
reaction r_wifi;
reaction r_mqtt;
reaction r_draw;



struct Config
{
  String ssid;        // Enter SSID here
  String password;    // Enter Password here
  char broker_IP[15]; // Ip broker
  String mqtt_user;
  String mqtt_pass;
  String client_device;
  int mqtt_port;
};

struct Config cfg;

EspMQTTClient my_client(
    cfg.ssid.c_str(),
    cfg.password.c_str(),
    cfg.broker_IP,         // MQTT Broker server ip
    cfg.mqtt_user.c_str(), // Can be omitted if not needed
    cfg.mqtt_pass.c_str(), // Can be omitted if not needed
    cfg.client_device.c_str(),
    cfg.mqtt_port          // The MQTT port, default to 1883. this line can be omitted
);

StaticJsonDocument<200> json_doc_data_temperature;
StaticJsonDocument<200> json_doc_data_rfid;
StaticJsonDocument<200> json_doc_data_suhu_rfid;

JsonArray array_data_temperature;
JsonArray array_data_rfid;
JsonArray array_data_suhu_rfid;

String human_datetime = "";
unsigned long number_data_temperature = 0; 
unsigned long number_data_suhu_rfid = 0; 

String topic="testtopic";
uint32_t formattedDate = 0;

void do_stuff();

void Readcallback();
void Sendcommand(uint8_t com_nub);

void read_button();

void Play_MarioUW_1();
void Play_MarioUW_2();

void publish_suhu_rfid_object();
void publish_file();
void send_suhu_rfid();

void rtemperature();
void rfid();
void rmqtt();
void rdraw();
void rbutton();
void mqtt_broker_on_check();
void app_onConnectionEstablished();


bool load_config_file();

void read_nvs();
void write_nvs(unsigned long idx);
void app_setup();
void app_main();


Reactduino app(app_main);

    


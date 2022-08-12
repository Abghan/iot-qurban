#ifndef _ADAFRUIT_MYWIFIIH_
#define _ADAFRUIT_MYWIFIIH_

#include "Arduino.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#include <SPIFFS.h>
#include "ArduinoJson.h"

//char man;
//bool trig1=false;

//const char* ssid = "Tigaresi Lantai 2";
//const char* password = "Tigaresi56";
//const char* mqtt_server = "192.168.0.143";

void setup_wifi();
void callback(char* topic, byte* message, unsigned int length);
bool load_config_file();
void initmqtt();
void webServer();
void SendHTML_Header();
void HomePage();
void File_Download();
void File_Upload();
void handleFileUpload();
void File_Stream();
void File_Delete();
void SPIFFS_dir();
void SendHTML_Content();
void SendHTML_Stop();
void DownloadFile(String filename);
void ReportFileNotPresent(String target);
void ReportCouldNotCreateFile(String target);
void SelectInput(String heading1, String command, String arg_calling_name);
void ReportSPIFFSNotPresent();
void SPIFFS_file_stream(String filename);
void SPIFFS_file_delete(String filename);
void printDirectory(const char * dirname, uint8_t levels);
String file_size(int bytes);
void webServerOn();
void publish_file();

void reConnet_wifii();

class my
{
  public:
  bool trig1=false;
  bool ReCon(void);
  bool show_update=false;
  void loop_mqtt();
  String _weight_;
  String _rfid_;
};

#endif
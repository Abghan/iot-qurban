#include "myWifii.h"
#include "jrd100.h"
#include <weight_A12E.h>
#include "variableGlobal.h"
#include <WebServer.h>
#include "EEPROM.h"

#define ServerVersion ""
String webpage = "";
bool SPIFFS_present = false;

#include "CSS.h"
WebServer server(80);

JRD100 rfid_;
Weight_A12E _Timbangan;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

float temperature = 0;
float humidity = 0;
const int ledPin = 4;

IPAddress local_IP;
IPAddress gateway;
IPAddress subnet;

unsigned long Msg_id;
unsigned long id_count = 0;

unsigned long Msg_count_dummy = 0;

unsigned int tahun;
unsigned int bulan;
unsigned int hari;
unsigned int jam;
unsigned int menit;
unsigned int detik;

bool sendData_mqtt = false;
bool sendData_mqtt_dummy = false;
bool send_ok = false;
bool send_ok_mqtt = false;
bool state_mqtt = false;
bool send_ok_weight = false;
bool save_data = false;

struct Config
{
  String ssid;        // Enter SSID here
  String password;    // Enter Password here
  char broker_IP[15]; // Ip broker
  char mqtt_user[50];
  char mqtt_pass[50];
  char mqtt_topic[50];
  char client_device[20];
  int mqtt_port;
  char local_ip[20];
  char local_gateway[20];
  char local_subnet[20];
};
struct Config cfg;

unsigned long previousMillis = 0, previousMillis_reconnect = 0;
unsigned long interval = 3000;

// IPAddress local_IP(192, 168, 1, 184);

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(cfg.ssid.c_str());

  WiFi.begin(cfg.ssid.c_str(), cfg.password.c_str());
  // Serial.println("\nConnected to "+WiFi.SSID()+" Use IP address: "+WiFi.localIP().toString());

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Reconnecting to WiFi...");
    // Serial.print("Connected to= ");
    Serial.println("\nConnected to " + WiFi.SSID() + " Use IP address: " + WiFi.localIP().toString());
    // Serial.print("Use IP address= ");
    // Serial.println(WiFi.localIP().toString());
  }
  else
  {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    // Serial.print("Connected to= ");
    Serial.println("\nConnected to " + WiFi.SSID() + " Use IP address: " + WiFi.localIP().toString());
    // Serial.print("Use IP address= ");
    // Serial.println(WiFi.localIP().toString());
    // Serial.println("\nConnected to "+WiFi.SSID()+" Use IP address: "+WiFi.localIP().toString()); // Report which SSID and IP is in use
  }
}

void reConnet_wifii()
{
  if ((WiFi.status() != WL_CONNECTED) && (millis() - previousMillis >= interval))
  {
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    // Serial.print("Connected to= ");
    Serial.println("\nConnected to " + WiFi.SSID() + " Use IP address: " + WiFi.localIP().toString());
    // Serial.print("Use IP address= ");
    // Serial.println(WiFi.localIP().toString());
    // Serial.println("\nConnected to "+WiFi.SSID()+" Use IP address: "+WiFi.localIP().toString()); // Report which SSID and IP is in use
    previousMillis = millis();
    // trig1 =false;
    // my_wifii_.my
  }
}

bool my::ReCon(void)
{
  if ((WiFi.status() != WL_CONNECTED) && (millis() - previousMillis >= interval))
  {
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    // Serial.print("Connected to= ");
    Serial.println("\nConnected to " + WiFi.SSID() + " Use IP address: " + WiFi.localIP().toString());
    // Serial.print("Use IP address= ");
    // Serial.println(WiFi.localIP().toString());
    previousMillis = millis();
    show_update = false;
    state_mqtt = false;
  }

  return true;
}

bool load_config_file()
{
  delay(100);
  Serial.println("////////////////////////////////////////////////////////////////////////////////////////");
  Serial.println("Load config start...");

  bool success = SPIFFS.begin();
  if (!success)
  {
    SPIFFS_present = false;
    Serial.println("Error mounting the file system");
    return false;
  }
  else
  {
    SPIFFS_present = true;
  }

  Serial.println("Load config timbangan..");
  File configFile = SPIFFS.open("/config_timbangan.txt", "r");

  if (!configFile)
  {
    Serial.println("GAGAL");
    return false;
  }

  StaticJsonDocument<1024> doc;

  DeserializationError error = deserializeJson(doc, configFile);
  if (error)
  {
    Serial.println(F("Failed to read file, using default configuration"));
    Serial.println(error.c_str());
  }

  String doc_ssid = doc["ssid"];
  String doc_pass = doc["password"];
  String doc_broker_IP = doc["broker"];
  String doc_mqtt_user = doc["mqtt_user"];
  String doc_mqtt_pass = doc["mqtt_pass"];
  String doc_mqtt_topic = doc["mqtt_topic"];
  String doc_client_device = doc["client_device"];

  String doc_local_ip = doc["local_ip"];
  String doc_local_gateway = doc["gateway"];
  String doc_local_subnet = doc["subnet"];

  int doc_port = doc["mqtt_port"];

  cfg.ssid = doc_ssid;     // Enter SSID here
  cfg.password = doc_pass; // Enter Password here

  doc_broker_IP.toCharArray(cfg.broker_IP, sizeof(cfg.broker_IP));
  doc_client_device.toCharArray(cfg.client_device, sizeof(cfg.client_device));
  doc_mqtt_topic.toCharArray(cfg.mqtt_topic, sizeof(cfg.mqtt_topic));

  doc_mqtt_user.toCharArray(cfg.mqtt_user, sizeof(cfg.mqtt_user));
  doc_mqtt_pass.toCharArray(cfg.mqtt_pass, sizeof(cfg.mqtt_pass));

  cfg.mqtt_port = doc_port;

  Serial.println("");
  Serial.print("SSID= ");
  Serial.println(cfg.ssid);

  Serial.print("Password= ");
  Serial.println(cfg.password);

  Serial.print("BrokerIP= ");
  Serial.println(cfg.broker_IP);

  Serial.print("Device= ");
  Serial.println(cfg.client_device);

  Serial.print("MQTTPort= ");
  Serial.println(cfg.mqtt_port);

  Serial.print("MQTTTopic= ");
  Serial.println(cfg.mqtt_topic);

  local_IP.fromString(doc_local_ip);
  gateway.fromString(doc_local_gateway);
  subnet.fromString(doc_local_subnet);

  Serial.print("LocalIP= ");
  Serial.println(local_IP);

  Serial.print("Gateway= ");
  Serial.println(gateway);

  Serial.print("Subnet= ");
  Serial.println(subnet);

  if (!WiFi.config(local_IP, gateway, subnet))
  {
    Serial.println("STA Failed to configure");
  }

  configFile.close();
  // SPIFFS.end();
  Serial.println("Load config end...");
  Serial.println("////////////////////////////////////////////////////////////////////////////////////////");
  return true;
}

void initWiFi()
{
  setup_wifi();
  client.setServer(cfg.broker_IP, cfg.mqtt_port);
  client.setCallback(callback);
}

void callback(char *topic, byte *message, unsigned int length)
{
  // Serial.print("Message arrived on topic: ");
  // Serial.println(topic);
  // Serial.print("Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    messageTemp += (char)message[i];
  }
  Serial.println(messageTemp);

  if (String(topic) == "Board_2")
  {
    char buff[15];
    messageTemp.toCharArray(buff, 15);
    ;
    if (memcmp(buff, "Get File", 8) == 0)
    {
      // Serial.println("on");
      publish_file();
    }
    if (memcmp(buff, "Delete File", 11) == 0)
    {
      // Serial.println("off");
      File file = SPIFFS.open("/save_data.txt", FILE_WRITE);
      //File file = SPIFFS.open("/save_data.txt", FILE_APPEND);
      if (!file)
      {
        Serial.println("There was an error opening the file for writing");
        return;
      }
      if (file.println(""))
      {
        Serial.println("File was written");
      }
      else
      {
        Serial.println("File write failed");
      }

      file.close();
      Msg_id=0;
      EEPROM.writeULong64(0, Msg_id);
      EEPROM.commit();
      delay(10);
    }
    // Serial.println("");
  }
}

void my::loop_mqtt()
{
  if (!client.connected() && (millis() - previousMillis_reconnect >= 5000))
  {
    state_mqtt = false;
    previousMillis_reconnect = millis();
    trig1 = false;

    // if (client.connect(cfg.client_device))
    if (client.connect(cfg.client_device, cfg.mqtt_user, cfg.mqtt_pass))
    {
      Serial.println("connected");
      client.subscribe("Board_2"); // cfg.mqtt_topic);
      // my1_.trig1=true;
    }
    else
    {
      Serial.print("\0D\0Afailed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
    }
  }

  if (client.connected())
  {
    state_mqtt = true;
    trig1 = true;
  }

  client.loop();

  if (sendData_mqtt == true)
  {
    sendData_mqtt = false;
    cek_param = false;
    char mqttBuff[500];

    Msg_id++;
    id_count++;

    //{"id": 1, "dev_id":"Board_2", "msg_id": 1, "time_stamp": "2020-05-25 00:25:00+07", "rfid": 84966541, "value": 11}'
    sprintf(mqttBuff, "{\"id\":%d,\"dev_id\": \"%S\",\"msg_id\" : %d,\"time_stamp\" : \"%04d-%02d-%02d %02d:%02d:%02d+07\",\"rfid\" : \"%S\",\"value\" : \"%S\"}", id_count, cfg.client_device, Msg_id, tahun, bulan, hari, jam, menit, detik, rfid_send, weight_send);
    // sprintf(mqttBuff,"{\"data\" :{\"device_id\": \"%S\",\"message_id\" : %d,\"timestamp\" : \"2022-06-07.11:00:24\",\"rfid\" : \"%S\",\"value\" : \"%S\"}}",cfg.client_device,Msg_count,rfid_send,weight_send);
    // sprintf(mqttBuff, "{\"data\" :{\"device_id\": \"%S\",\"message_id\" : %d,\"timestamp\" : \"%04d-%02d-%02d.%02d:%02d:%02d\",\"rfid\" : \"%S\",\"value\" : \"%S\"}}", cfg.client_device, Msg_count, tahun, bulan, hari, jam, menit, detik, rfid_send, weight_send);

    // Serial.println(client.publish(cfg.mqtt_topic, mqttBuff));
    if (client.publish(cfg.mqtt_topic, mqttBuff))
    {
      Serial.println(mqttBuff);
      send_ok = true;
      send_ok_mqtt = true;
      send_ok_weight = true;
      rfid_send = "";
      weight_send = "0.0";

      // File file = SPIFFS.open("/save_data.txt", FILE_WRITE);
      File file = SPIFFS.open("/save_data.txt", FILE_APPEND);
      if (!file)
      {
        Serial.println("There was an error opening the file for writing");
        return;
      }
      if (file.println(mqttBuff))
      {
        Serial.println("File was written");
      }
      else
      {
        Serial.println("File write failed");
      }

      file.close();

      EEPROM.writeULong64(0, Msg_id);
      EEPROM.commit();
      delay(10);
    }
    else
    {
      Serial.println(mqttBuff);
      send_ok = true;
      save_data = true;
      send_ok_mqtt = false;
      send_ok_weight = true;
      rfid_send = "";
      weight_send = "0.0";

      // sprintf(mqttBuff, "{\"id\":%d,\"dev_id\": \"%S\",\"msg_id\" : %d,\"time_stamp\" : \"%04d-%02d-%02d %02d:%02d:%02d+07\",\"rfid\" : \"%S\",\"value\" : \"%S\"}",id_count, cfg.client_device, Msg_id, tahun, bulan, hari, jam, menit, detik, rfid_send, weight_send);

      File file = SPIFFS.open("/save_data.txt", FILE_APPEND);
      if (!file)
      {
        Serial.println("There was an error opening the file for writing");
        return;
      }
      if (file.println(mqttBuff))
      {
        Serial.println("File was written");
      }
      else
      {
        Serial.println("File write failed");
      }

      file.close();

      EEPROM.writeULong64(0, Msg_id);
      EEPROM.commit();
      delay(10);
    }

    Serial.println("Keluar");
    memset(mqttBuff, 0, sizeof(mqttBuff));
  }

  /*if (sendData_mqtt_dummy == true)
  {
    sendData_mqtt_dummy=false;
    cek_param = false;
    char mqttBuff[500];
    Msg_count++;
    Msg_count_dummy+=5;
    String rfid_dummy="12345678";

    sprintf(mqttBuff, "{\"id\":1,\"dev_id\": \"%S\",\"msg_id\" : %d,\"time_stamp\" : \"%04d-%02d-%02d %02d:%02d:%02d+07\",\"rfid\" : \"%S\",\"value\" : \"%d\"}",cfg.client_device, Msg_count, tahun, bulan, hari, jam, menit, detik, rfid_dummy, Msg_count_dummy);
    //client.publish(cfg.mqtt_topic, mqttBuff);
    if(client.publish(cfg.mqtt_topic, mqttBuff))
    {
      Serial.println(mqttBuff);
      send_ok_mqtt=true;
    }
    else
    {
      send_ok_mqtt=false;
    }
    memset(mqttBuff, 0, sizeof(mqttBuff));

  }*/
}

void publish_file()
{
  File file = SPIFFS.open("/save_data.txt", FILE_READ);
  if (!file)
  {
    Serial.println("There was an error opening the file for read");
    return;
  }
  else
  {
    Serial.println("Publishing data using temp file");
  }
  char *data = (char *)heap_caps_malloc(file.size(), MALLOC_CAP_8BIT);

  int i = 0;
  while (file.available())
  {
    data[i++] = file.read();
  }
  Serial.println(String(i) + " bytes read");
  // Serial.println(data);
  // sprintf()

  // panjang topic 7 Board_1_data
  // MQTT_MAX_HEADER_SIZE 5 + 2 = 7;
  if (client.setBufferSize(file.size()))
  {
    Serial.printf("Size Payload %d and size %d", file.size(), client.getBufferSize());
    delay(10);
    // char nor[client.getBufferSize()];
    // sprintf(nor,"%S",String(data).substring(0,file.size()- 2));
    client.publish_P("Board_2", data, true);
    // Serial.println(nor);
    delay(10);
    Serial.println("");
    Serial.println("Done publish.");
  }
}

void webServer()
{
  ///////////////////////////// Server Commands
  server.on("/", HomePage);
  server.on("/download", File_Download);
  server.on("/upload", File_Upload);
  server.on(
      "/fupload", HTTP_POST, []()
      { server.send(200); },
      handleFileUpload);
  server.on("/stream", File_Stream);
  server.on("/delete", File_Delete);
  server.on("/dir", SPIFFS_dir);

  ///////////////////////////// End of Request commands
  server.begin();
  Serial.println("HTTP server started");
}

void webServerOn()
{
  server.handleClient();
}

// All supporting functions from here...
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void HomePage()
{
  SendHTML_Header();
  webpage += F("<a href='/download'><button>Download</button></a>");
  webpage += F("<a href='/upload'><button>Upload</button></a>");
  webpage += F("<a href='/stream'><button>Stream</button></a>");
  webpage += F("<a href='/delete'><button>Delete</button></a>");
  webpage += F("<a href='/dir'><button>Directory</button></a>");
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop(); // Stop is needed because no content length was sent
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void File_Download()
{ // This gets called twice, the first pass selects the input, the second pass then processes the command line arguments
  if (server.args() > 0)
  { // Arguments were received
    if (server.hasArg("download"))
      DownloadFile(server.arg(0));
  }
  else
    SelectInput("Enter filename to download", "download", "download");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void DownloadFile(String filename)
{
  if (SPIFFS_present)
  {
    File download = SPIFFS.open("/" + filename, "r");
    if (download)
    {
      server.sendHeader("Content-Type", "text/text");
      server.sendHeader("Content-Disposition", "attachment; filename=" + filename);
      server.sendHeader("Connection", "close");
      server.streamFile(download, "application/octet-stream");
      download.close();
    }
    else
      ReportFileNotPresent("download");
  }
  else
    ReportSPIFFSNotPresent();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void File_Upload()
{
  append_page_header();
  webpage += F("<h3>Select File to Upload</h3>");
  webpage += F("<FORM action='/fupload' method='post' enctype='multipart/form-data'>");
  webpage += F("<input class='buttons' style='width:40%' type='file' name='fupload' id = 'fupload' value=''><br>");
  webpage += F("<br><button class='buttons' style='width:10%' type='submit'>Upload File</button><br>");
  webpage += F("<a href='/'>[Back]</a><br><br>");
  append_page_footer();
  server.send(200, "text/html", webpage);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
File UploadFile;
void handleFileUpload()
{                                           // upload a new file to the Filing system
  HTTPUpload &uploadfile = server.upload(); // See https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/srcv
                                            // For further information on 'status' structure, there are other reasons such as a failed transfer that could be used
  if (uploadfile.status == UPLOAD_FILE_START)
  {
    String filename = uploadfile.filename;
    if (!filename.startsWith("/"))
      filename = "/" + filename;
    Serial.print("Upload File Name: ");
    Serial.println(filename);
    SPIFFS.remove(filename);                 // Remove a previous version, otherwise data is appended the file again
    UploadFile = SPIFFS.open(filename, "w"); // Open the file for writing in SPIFFS (create it, if doesn't exist)
  }
  else if (uploadfile.status == UPLOAD_FILE_WRITE)
  {
    if (UploadFile)
      UploadFile.write(uploadfile.buf, uploadfile.currentSize); // Write the received bytes to the file
  }
  else if (uploadfile.status == UPLOAD_FILE_END)
  {
    if (UploadFile) // If the file was successfully created
    {
      UploadFile.close(); // Close the file again
      Serial.print("Upload Size: ");
      Serial.println(uploadfile.totalSize);
      webpage = "";
      append_page_header();
      webpage += F("<h3>File was successfully uploaded</h3>");
      webpage += F("<h2>Uploaded File Name: ");
      webpage += uploadfile.filename + "</h2>";
      webpage += F("<h2>File Size: ");
      webpage += file_size(uploadfile.totalSize) + "</h2><br>";
      append_page_footer();
      server.send(200, "text/html", webpage);
    }
    else
    {
      ReportCouldNotCreateFile("upload");
    }
  }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef ESP32
void SPIFFS_dir()
{
  if (SPIFFS_present)
  {
    File root = SPIFFS.open("/");
    if (root)
    {
      root.rewindDirectory();
      SendHTML_Header();
      webpage += F("<h3 class='rcorners_m'>SD Card Contents</h3><br>");
      webpage += F("<table align='center'>");
      webpage += F("<tr><th>Name/Type</th><th style='width:20%'>Type File/Dir</th><th>File Size</th></tr>");
      printDirectory("/", 0);
      webpage += F("</table>");
      SendHTML_Content();
      root.close();
    }
    else
    {
      SendHTML_Header();
      webpage += F("<h3>No Files Found</h3>");
    }
    append_page_footer();
    SendHTML_Content();
    SendHTML_Stop(); // Stop is needed because no content length was sent
  }
  else
    ReportSPIFFSNotPresent();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void printDirectory(const char *dirname, uint8_t levels)
{
  File root = SPIFFS.open(dirname);
  if (!root)
  {
    return;
  }
  if (!root.isDirectory())
  {
    return;
  }
  File file = root.openNextFile();
  while (file)
  {
    if (webpage.length() > 1000)
    {
      SendHTML_Content();
    }
    if (file.isDirectory())
    {
      webpage += "<tr><td>" + String(file.isDirectory() ? "Dir" : "File") + "</td><td>" + String(file.name()) + "</td><td></td></tr>";
      printDirectory(file.name(), levels - 1);
    }
    else
    {
      webpage += "<tr><td>" + String(file.name()) + "</td>";
      webpage += "<td>" + String(file.isDirectory() ? "Dir" : "File") + "</td>";
      int bytes = file.size();
      String fsize = "";
      if (bytes < 1024)
        fsize = String(bytes) + " B";
      else if (bytes < (1024 * 1024))
        fsize = String(bytes / 1024.0, 3) + " KB";
      else if (bytes < (1024 * 1024 * 1024))
        fsize = String(bytes / 1024.0 / 1024.0, 3) + " MB";
      else
        fsize = String(bytes / 1024.0 / 1024.0 / 1024.0, 3) + " GB";
      webpage += "<td>" + fsize + "</td></tr>";
    }
    file = root.openNextFile();
  }
  file.close();
}
#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void File_Stream()
{
  if (server.args() > 0)
  { // Arguments were received
    if (server.hasArg("stream"))
      SPIFFS_file_stream(server.arg(0));
  }
  else
    SelectInput("Enter a File to Stream", "stream", "stream");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SPIFFS_file_stream(String filename)
{
  if (SPIFFS_present)
  {
    File dataFile = SPIFFS.open("/" + filename, "r"); // Now read data from SPIFFS Card
    if (dataFile)
    {
      if (dataFile.available())
      { // If data is available and present
        String dataType = "application/octet-stream";
        if (server.streamFile(dataFile, dataType) != dataFile.size())
        {
          Serial.print(F("Sent less data than expected!"));
        }
      }
      dataFile.close(); // close the file:
    }
    else
      ReportFileNotPresent("Cstream");
  }
  else
    ReportSPIFFSNotPresent();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void File_Delete()
{
  if (server.args() > 0)
  { // Arguments were received
    if (server.hasArg("delete"))
      SPIFFS_file_delete(server.arg(0));
  }
  else
    SelectInput("Select a File to Delete", "delete", "delete");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SPIFFS_file_delete(String filename)
{ // Delete the file
  if (SPIFFS_present)
  {
    SendHTML_Header();
    File dataFile = SPIFFS.open("/" + filename, "r"); // Now read data from SPIFFS Card
    if (dataFile)
    {
      if (SPIFFS.remove("/" + filename))
      {
        Serial.println(F("File deleted successfully"));
        webpage += "<h3>File '" + filename + "' has been erased</h3>";
        webpage += F("<a href='/delete'>[Back]</a><br><br>");
      }
      else
      {
        webpage += F("<h3>File was not deleted - error</h3>");
        webpage += F("<a href='delete'>[Back]</a><br><br>");
      }
    }
    else
      ReportFileNotPresent("delete");
    append_page_footer();
    SendHTML_Content();
    SendHTML_Stop();
  }
  else
    ReportSPIFFSNotPresent();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SendHTML_Header()
{
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  append_page_header();
  server.sendContent(webpage);
  webpage = "";
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SendHTML_Content()
{
  server.sendContent(webpage);
  webpage = "";
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SendHTML_Stop()
{
  server.sendContent("");
  server.client().stop(); // Stop is needed because no content length was sent
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SelectInput(String heading1, String command, String arg_calling_name)
{
  SendHTML_Header();
  webpage += F("<h3>");
  webpage += heading1 + "</h3>";
  webpage += F("<FORM action='/");
  webpage += command + "' method='post'>"; // Must match the calling argument e.g. '/chart' calls '/chart' after selection but with arguments!
  webpage += F("<input type='text' name='");
  webpage += arg_calling_name;
  webpage += F("' value=''><br>");
  webpage += F("<type='submit' name='");
  webpage += arg_calling_name;
  webpage += F("' value=''><br><br>");
  webpage += F("<a href='/'>[Back]</a><br><br>");
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ReportSPIFFSNotPresent()
{
  SendHTML_Header();
  webpage += F("<h3>No SPIFFS Card present</h3>");
  webpage += F("<a href='/'>[Back]</a><br><br>");
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ReportFileNotPresent(String target)
{
  SendHTML_Header();
  webpage += F("<h3>File does not exist</h3>");
  webpage += F("<a href='/");
  webpage += target + "'>[Back]</a><br><br>";
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ReportCouldNotCreateFile(String target)
{
  SendHTML_Header();
  webpage += F("<h3>Could Not Create Uploaded File (write-protected?)</h3>");
  webpage += F("<a href='/");
  webpage += target + "'>[Back]</a><br><br>";
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
String file_size(int bytes)
{
  String fsize = "";
  if (bytes < 1024)
    fsize = String(bytes) + " B";
  else if (bytes < (1024 * 1024))
    fsize = String(bytes / 1024.0, 3) + " KB";
  else if (bytes < (1024 * 1024 * 1024))
    fsize = String(bytes / 1024.0 / 1024.0, 3) + " MB";
  else
    fsize = String(bytes / 1024.0 / 1024.0 / 1024.0, 3) + " GB";
  return fsize;
}
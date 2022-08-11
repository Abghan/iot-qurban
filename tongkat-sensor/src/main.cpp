#include "main.h"
#include <WiFi.h>
#include "tombol.h"

tombol tombolku;

void publish_file()
{
  File file = SPIFFS.open("/test.json", FILE_READ);
  if (!file)
  {
    if(DEBUG) Serial.println("There was an error opening the file for read");
    return;
  }
  else
  {
    if(DEBUG) Serial.println("Publishing data using temp file");
  }
  char *data = (char *)heap_caps_malloc(file.size(), MALLOC_CAP_8BIT);

  int i = 0;
  while (file.available())
  {
    data[i++] = file.read();
  }
  if(DEBUG) Serial.println(String(i) + " bytes read");
  // String data_truncate = String(data).substring(0,file.size());
  // panjang topic 7 Board_1_data
  // MQTT_MAX_HEADER_SIZE 5 + 2 = 7;
  if (my_client.setMaxPacketSize(file.size()))
  {
    if(DEBUG) Serial.printf("Size Payload %d and size %d", file.size(), my_client.getMaxPacketSize());
    delay(10);
    my_client.publish("Board_1", String(data).substring(0, file.size() - 2));
    delay(10);
    if(DEBUG) Serial.println("Done publish.");
  }
}
void send_suhu_rfid()
{
  if ((rfid_data_on_hold == "") && (temperature_data_on_hold == ""))
    return;

  String response;
  array_data_suhu_rfid = json_doc_data_suhu_rfid.to<JsonArray>();

  formattedDate = MyRTC.get_unixtime();
  // Unix time ke datetime string
  if (formattedDate == 0)
    return;

  time_t unix_t = formattedDate + (7 * 3600);

  char buf[20];
  struct tm *myinfo;
  myinfo = gmtime(&unix_t);
  strftime(buf, sizeof(buf), "%Y-%m-%d %X", myinfo);
  human_datetime = buf;
  human_datetime += "+07";

  number_data_rfid++;
  write_nvs(number_data_rfid);

  array_data_suhu_rfid.add(cfg.client_device); // Perangkat temperature
  array_data_suhu_rfid.add(number_data_rfid);  // no urut data
  array_data_suhu_rfid.add(human_datetime);    // dtime  string
  array_data_suhu_rfid.add(rfid_data_on_hold); // rfid
  array_data_suhu_rfid.add(temperature_data_on_hold);

  publish_suhu_rfid_object();
}
void publish_suhu_rfid_object()
{
  const size_t CAPACITY = JSON_OBJECT_SIZE(10);
  StaticJsonDocument<CAPACITY> doc;

  // create an object
  JsonObject object = doc.to<JsonObject>();

  object["id"] = number_send_id;
  object["dev_id"] = array_data_suhu_rfid[0];
  object["msg_id"] = array_data_suhu_rfid[1];
  object["time_stamp"] = array_data_suhu_rfid[2];
  object["rfid"] = array_data_suhu_rfid[3];
  object["value"] = array_data_suhu_rfid[4];

  String data;
  serializeJson(object, data);

  if(DEBUG) Serial.println("PUBLISH RFID & SUHU");
  if(DEBUG) Serial.println(data);
  if(DEBUG) Serial.println(topic);

  // testtopic = 9, header = 7
  my_client.setMaxPacketSize(data.length() + 16);
  my_client.publish(topic, data);

  Log.add_log("/test.json", data);

  number_send_id += 1;
  if (number_data_rfid == -1)
    number_data_rfid = 1;
  MyLCD.temperature_ok = false;
  MyLCD.rfid_ok = false;
  MyLCD.val_rfid = "";
  MyLCD.val_suhu = "";
  MyLCD.onsend_data = true;
}

bool load_config_file()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Load config start...");

  bool success = SPIFFS.begin();
  if (!success)
  {
    Serial.println("Error mounting the file system");
    return false;
  }
  // Open config file for writing.
  Serial.println("Load config suhu..");
  File configFile = SPIFFS.open("/config_suhu.txt", "r");

  if (!configFile.available())
  {
    Serial.println("GAGAL");
    return false;
  }

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, configFile);
  delay(50);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  const String doc_ssid = doc["ssid"];
  const String doc_pass = doc["password"];
  const String doc_broker_IP = doc["broker"];
  const String doc_mqtt_user = doc["mqtt_user"];
  const String doc_mqtt_pass = doc["mqtt_pass"];
  const String doc_client_device = doc["client_device"];
  const String doc_topic = doc["topic"];
  const int doc_port = doc["mqtt_port"];
  const int doc_hardware = doc["Hardware"];
  const int doc_idx = doc["idx"];

  cfg.ssid = doc_ssid;     // Enter SSID here
  cfg.password = doc_pass; // Enter Password here

  doc_broker_IP.toCharArray(cfg.broker_IP, sizeof(cfg.broker_IP));

  Serial.printf("DEVICE: %s IDX: %d PORT: %d MD_HARDW: %d TOPIC %s", doc_client_device, doc_idx, doc_port, doc_hardware, doc_topic);
  Serial.println();
  // Serial.println(doc_client_device);
  // Serial.println(doc_idx);
  // Serial.println(doc_port);
  // Serial.println(doc_topic);

  cfg.client_device = doc_client_device;

  cfg.mqtt_user = doc_mqtt_user;
  cfg.mqtt_pass = doc_mqtt_pass;
  cfg.mqtt_port = doc_port;

  Serial.println("CONF PORT");
  Serial.println(cfg.mqtt_port);

  local_IP[0] = doc["local_ip"][0];
  local_IP[1] = doc["local_ip"][1];
  local_IP[2] = doc["local_ip"][2];
  local_IP[3] = doc["local_ip"][3];

  gateway[0] = doc["gateway"][0];
  gateway[1] = doc["gateway"][1];
  gateway[2] = doc["gateway"][2];
  gateway[3] = doc["gateway"][3];

  subnet[0] = doc["subnet"][0];
  subnet[1] = doc["subnet"][1];
  subnet[2] = doc["subnet"][2];
  subnet[3] = doc["subnet"][3];

  hardware = doc_hardware;
  topic = doc_topic;

  // OLED_Device = doc_client_device;

  Serial.println("SSID: " + cfg.ssid + " PASS:" + cfg.password + " IP BROKER: " + cfg.broker_IP + " USER: " + cfg.mqtt_user + " PASS: " +
                 cfg.mqtt_pass + " PORT: " + (String)my_client.getMqttServerPort());

  my_client.setMqttServer(cfg.ssid.c_str(), cfg.password.c_str(), cfg.broker_IP, cfg.mqtt_user.c_str(), cfg.mqtt_pass.c_str(), cfg.mqtt_port);
  my_client.enable_MQTT = true;

  configFile.close();

  SPIFFS.end();
  Serial.println("LOAD SPIFF OK");
  return true;
}
void mqtt_broker_on_check()
{
  /*
  if (!my_client.enable_MQTT)
  {
    MyLCD.mqtt_status = false;
    return;
  }
  */

  if (my_client.isWifiConnected())
  {
    MyLCD.wifi_is_connected = true;
    MyLCD.val_rssi = my_client.RSSI;

    if (!my_client.isMqttConnected())
      MyLCD.mqtt_status = false;
    else
      MyLCD.mqtt_status = true;
  }

  else
  {
    MyLCD.mqtt_status = false;
    MyLCD.wifi_is_connected = false;
  }
}

void rtemperature()
{

  ProximityValue = analogRead(portPin);
  // Serial.printf("Proxymity value %d\n",ProximityValue);
  //  if ((ProximityValue > 1280) && (ProximityValue < 2500))
  if ((ProximityValue > 1280) && (ProximityValue < 2500) && (Temperature.active))
  {
    Temperature.temp_finish = false;
    MyLCD.temperature_ok = false;
    // 20 cman

    do
    {
      Temperature.loop_melopero();
    } while (!Temperature.temp_finish);

    if (Temperature.temp_finish)
    {
      object_detected = true;
      MyLCD.temperature_ok = true;
      // Serial.print("Object detected...");
      // Serial.println(ProximityValue);
      Serial.print("SUHU...");
      Serial.println(Temperature.rata_rata_suhu);
      MyLCD.val_suhu = Temperature.rata_rata_suhu;
      Temperature.temp_finish = false;
    }

    // MyLCD.idle = false;
    proxymity_ondetect = true;
  }
  else
  {
    object_detected = false;
    proxymity_ondetect = false;
  }
}
void rfid()
{
  RFIDReader.loop();
  // RFIDReader.RFID_Read_Callback();
}
void rmqtt()
{
  my_client.loop();
  mqtt_broker_on_check();
}

void rdraw()
{
  MyLCD.loop();
}

int freq = 2000;
int channel = 0;
int resolution = 8;

void Play_MarioUW()
{
  for (int thisNote = 0; thisNote < (sizeof(MarioUW_note) / sizeof(int)); thisNote++)
  {
    int noteDuration = 1000 / MarioUW_duration[thisNote]; // convert duration to time delay
    ledcWriteTone(channel, MarioUW_note[thisNote] * 8);
    int pauseBetweenNotes = noteDuration * 1.80;
    delay(pauseBetweenNotes);
  }
}

void Play_MarioUW_1()

{
  for (int thisNote = 0; thisNote < (sizeof(MarioUW_note1) / sizeof(int)); thisNote++)
  {
    int noteDuration = 1000 / MarioUW_duration1[thisNote]; // convert duration to time delay
    ledcWriteTone(channel, MarioUW_note1[thisNote] * 8);
    int pauseBetweenNotes = noteDuration * 1.80;
    delay(pauseBetweenNotes);
  }
}

void Play_MarioUW_2()

{
  buzzer_on_progress = true;
  for (int thisNote = 0; thisNote < (sizeof(MarioUW_note2) / sizeof(int)); thisNote++)
  {
    int noteDuration = 1000 / MarioUW_duration2[thisNote]; // convert duration to time delay

    ledcWriteTone(channel, MarioUW_note2[thisNote] * 8);
    int pauseBetweenNotes = noteDuration * 1.80;
    delay(pauseBetweenNotes);
  }
  buzzer_on_progress = false;
}

void read_serial()
{
  while (Serial.available() > 1)
  {
    char cmd = Serial.read();
    char unixt[10];
    if (cmd == 'T')
    {
      long datetimeunix = 0;
      datetimeunix = Serial.readString().toInt();
      MyRTC.set_rtc(datetimeunix);
      Serial.println("Set RTC from serial OK");
    }

    if (cmd == 'Z')
    {
      Log.removeFile(SPIFFS, "/test.json");
      number_data_rfid = 0;
      write_nvs(number_data_rfid);
    }

    if (cmd == 'R')
    {
      Log.readFile(SPIFFS, "/test.json");
    }
    if (cmd == '1')
      my_client.enableDebuggingMessages(true);
    if (cmd == '0')
      my_client.enableDebuggingMessages(false);
    if (cmd == 'P')
      publish_file();
  }
}
void rbuzzer()
{
  read_serial();
  digitalWrite(leddbg, HIGH);

  if (object_detected)
  {
    Play_MarioUW_1();
    pinMode(leddbg, OUTPUT);
    digitalWrite(leddbg, LOW);
    delay(100);
    digitalWrite(leddbg, HIGH);
  }

  if (RFIDReader.RFID_detect)
  {
    Play_MarioUW_2();
  }
}

void rbutton()
{
   tombolku.loop();
}

// Task utama
// Fungsi callback ini harus ada selama menggunakan ESPMQTTClient
// Edit library ReactduinoAPI
// Reactduino::Reactduino(react_callback cb) : _setup(cb), _onConnectionEstablished(cb) //tambahan callback buat mqtt
void app_onConnectionEstablished()
{
  if (my_client.enable_MQTT && my_client.isWifiConnected() && my_client.isMqttConnected())
  {
    my_client.subscribe(topic, [](const String &payload){
    if(DEBUG) Serial.println(payload);
   });

  
    my_client.subscribe("Board_1", [](const String &payload){
    if(payload == "Get File")
    {
      publish_file();
    }
    if(payload == "Delete File")
    {
      Log.removeFile(SPIFFS, "/test.json");
      number_data_rfid = 0;
      write_nvs(number_data_rfid);
    }
    });
  }
}

void read_nvs()
{
  // Initialize NVS
  err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
  /*****************************************/

  // Open
  printf("\n");
  printf("Opening Non-Volatile Storage (NVS) handle to Read only... ");
  nvs_handle_t my_handle;
  err = nvs_open("storage", NVS_READONLY, &my_handle);
  if (err != ESP_OK)
  {
    printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
  }
  else
  {
    printf("Done\n");

    /* Read from NVS:     */
    printf("Reading length of first User Message from NVS ... ");

    err = nvs_get_i32(my_handle, "Nomor:", &no_urut_data);
    switch (err)
    {
    case ESP_OK:
      number_data_rfid = no_urut_data;
      printf("Done\n");
      printf("Nomor data = %d\n", no_urut_data);
      break;
    case ESP_ERR_NVS_NOT_FOUND:
      printf("The value is not initialized yet!\n");
      break;
    default:
      printf("Error (%s) reading!\n", esp_err_to_name(err));
    }
    // Close
    nvs_close(my_handle);
  }
}
void write_nvs(unsigned long idx)
{
  // Open
  printf("\n");
  printf("Opening Non-Volatile Storage (NVS) handle to Write ... ");
  nvs_handle_t my_handle;
  err = nvs_open("storage", NVS_READWRITE, &my_handle);
  if (err != ESP_OK)
  {
    printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
  }
  else
  {
    printf("Done\n");
    // Write value into the NVS
    printf("Updating the length of first User Message in NVS ... ");

    no_urut_data = idx;
    err = nvs_set_i32(my_handle, "Nomor:", no_urut_data);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

    // Commit written value.
    // After setting any values, nvs_commit() must be called to ensure changes are written
    // to flash storage. Implementations may write to storage at other times,
    // but this is not guaranteed.
    printf("Committing updates in NVS ... ");
    err = nvs_commit(my_handle);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

    // Close
    nvs_close(my_handle);
  }
}

bool DEV_RESET = false;
void app_setup()
{
  Serial.begin(115200);
  Temperature.setup_melopero();
  MyLCD.setup();

  if (load_config_file())
  {
    read_nvs();
    RFIDReader.setup();
    Serial.println("ON RESET OK");
    // MyLCD.clear_oled();
  }

  bool ok = SPIFFS.begin();

  if (ok)
  {
    // Log.readFile(SPIFFS, "/test.json");
    my_client.enableDebuggingMessages(true);
    Serial.printf("SPIFF tersedia %d \n", SPIFFS.totalBytes() - SPIFFS.usedBytes());
  }

  tombolku.setup();

  object_detected = false;
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output

  // BUZZER INIT
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(buzzer, channel);
  
  DEV_RESET = true;
}

/*
 * I/O monitoring callback for the push button
 */

void app_main()
{
  if(!DEV_RESET) app_setup();
  app_onConnectionEstablished();

  //  Timing reactduino dalam mikrosecond, 1000000 = 1 Second = 1000 ms

  r_buzzer = app.repeat(1000, rbuzzer);
  r_rfid = app.repeat(1000, rfid);
  r_button = app.repeat(100000, rbutton);
  r_temperature = app.repeat(200000, rtemperature);
  r_mqtt = app.repeat(500000, rmqtt);
  r_draw = app.repeat(1000000, rdraw);
}

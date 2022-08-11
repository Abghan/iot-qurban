// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License
//
// Modified by Syed Razwanul Haque (Nabil), https://www.github.com/Nabilphysics
// for
// json data saving to ESP32 SPIFFS
//https://arduinojson.org/v6/doc/if(DEBUG) Serialization/

//Beberapa perubahan konversi dari ArduinoJSON5 ke 6

#include "logging.h"
#include "JRD100.h"

#define FORMAT_SPIFFS_IF_FAILED true
#define DEBUG true

void logging::appendFile(fs::FS &fs, const char * path, const char * message){
   Serial.printf("Appending to file: %s\r\n", path);

   File file = fs.open(path, FILE_APPEND);
   if(!file){
      Serial.println("− failed to open file for appending");
   }
   if(file.print(message)){
      Serial.println("− message appended");
   } else {
      Serial.println("− append failed");
   }
}

bool logging::add_log(String Filename, String data)
{
        String insert = data + "\r\n";
        appendFile(SPIFFS, Filename.c_str(),insert.c_str());
        return true;
}

void logging::removeFile(fs::FS &fs, const char * path)
{
  Serial.printf("BACA file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("-GAGAL BACA");
    return;
  }

  Serial.println("HAPUS file:");
  if(file.available()) {
    SPIFFS.remove(path);
    SPIFFS.begin();
  }
}

void logging::readFile(fs::FS &fs, const char * path) 
{
  Serial.printf("BACA file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("-GAGAL BACA");
    return;
  }

  Serial.println("- BACA DARI file:");
  while (file.available()) {
    Serial.write(file.read());
  }
}

void logging::listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  if(DEBUG) Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    if(DEBUG) Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    if(DEBUG) Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      if(DEBUG) Serial.print("  DIR : ");
      if(DEBUG) Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      if(DEBUG) Serial.print("  FILE: ");
      if(DEBUG) Serial.print(file.name());
      if(DEBUG) Serial.print("\tSIZE: ");
      if(DEBUG) Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

#ifdef TEST
void logging::setup() {
  // Initialize if(DEBUG) Serial port
  Serial.begin(115200);
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    if(DEBUG) Serial.println("SPIFFS Mount Failed");
    return;
  }
  while (!Serial) continue;
  
  //SPIFFS.remove("/hello.txt");
  // Memory pool for JSON object tree.
  //
  // Inside the brackets, 200 is the size of the pool in bytes.
  // Don't forget to change this value to match your JSON document.
  // Use arduinojson.org/assistant to compute the capacity.
  //StaticJsonBuffer<400> jsonBuffer;

  //const size_t bufferSize = JSON_OBJECT_SIZE(3);
  //DynamicJsonBuffer jsonBuffer(bufferSize);

  DynamicJsonDocument jsonBuffer(1024);
  // StaticJsonBuffer allocates memory on the stack, it can be
  // replaced by DynamicJsonBuffer which allocates in the heap.
  //
  // DynamicJsonBuffer  jsonBuffer(200);

  // Create the root of the object tree.
  //
  // It's a reference to the JsonObject, the actual bytes are inside the
  // JsonBuffer with all the other nodes of the object tree.
  // Memory is freed when jsonBuffer goes out of scope.
  //JsonObject& root = jsonBuffer.createObject();
  root =  jsonBuffer.to<JsonObject>();
  // Add values in the object
  //
  // Most of the time, you can rely on the implicit casts.
  // In other case, you can do root.set<long>("time", 1351824120);
  root["sensor"] = "gps";
  root["time"] = 1351824120;

  // Add a nested array.
  //
  // It's also possible to create the array separately and add it to the
  // JsonObject but it's less efficient.
  JsonArray d1 = root.createNestedArray("d1");
  for (int k = 0; k < 750; k++) {
    d1.add(4000 + k);
  }

  JsonArray d2 = root.createNestedArray("d2");
  for (int k = 0; k < 750; k++) {
    d2.add(k);
  }

  //This will save entire json data to SPIFFS
  File file = SPIFFS.open("/test.json", "w+");
  //root.printTo(file);
  serializeJson(root, file);
  file.close();

  // List the File
  listDir(SPIFFS, "/", 0);



  if(DEBUG) Serial.println("Reading JSON from SPIFFS: ");
  readFile(SPIFFS, "/test.json");
  
  //root.printTo(if(DEBUG) Serial);
  if(DEBUG) Serial.println();
  if(DEBUG) Serial.println();
  if(DEBUG) Serial.print("Array Size: ");
  if(DEBUG) Serial.print(d1.size() + d2.size());
  if(DEBUG) Serial.print(" SPIFFS totalBytes: ");
  if(DEBUG) Serial.print(SPIFFS.totalBytes());
  if(DEBUG) Serial.print(" SPIFFS usedBytes: ");
  if(DEBUG) Serial.print(SPIFFS.usedBytes());
  if(DEBUG) Serial.print("  Max Alloc Heap: ");
  if(DEBUG) Serial.print(ESP.getMaxAllocHeap());
  if(DEBUG) Serial.print("  Free Heap: ");
  if(DEBUG) Serial.print(ESP.getFreeHeap());
  if(DEBUG) Serial.print("  Total Heap: ");
  if(DEBUG) Serial.println(ESP.getHeapSize());

  //root.prettyPrintTo(if(DEBUG) Serial);
  // This prints:
  // {
  //   "sensor": "gps",
  //   "time": 1351824120,
  //   "data": [
  //     48.756080,
  //     2.302038
  //   ]
  // }
}
#endif

void logging::loop() {
  // not used in this example
}

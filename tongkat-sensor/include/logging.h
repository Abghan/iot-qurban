#ifndef logging_H
#define logging_H

#include "FS.h"
#include "SPIFFS.h"
#include "ArduinoJson.h"

class logging
{
  private:
    void appendFile(fs::FS &fs, const char * path, const char * message);
  public:
    JsonObject root;
    void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
    void readFile(fs::FS &fs, const char * path); 
    void removeFile(fs::FS &fs, const char * path); 
    bool add_log(String Filename, String data);
    //void setup();
    void loop();
};
#endif
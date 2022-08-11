#ifndef JRD100_H
#define JRD100_H

#include <Arduino.h>
#include "StrTools.h"
#include "LCD.h"

//extern HardwareSerial Ser1;
extern HardwareSerial Ser2;
extern lcd MyLCD;

class JRD100
{
private:
    bool in_progress =  false;
    StrTools str;
    String serial_str = "";
    String MyData = "";

    unsigned char req_single_inv[7] = {0xBB, 0x00, 0x22, 0x00, 0x00, 0x22, 0x7E};
    unsigned char req_set_baud[9] = {0xBB, 0x00, 0x11, 0x00, 0x02, 0x00, 0xC0, 0xD3, 0x7E};
    uint8_t Mask[12];
    void buffer_on_check();
public:
    bool RFID_hold = false;
    bool RFID_detect = false;
    bool enable_scan = false;
    String current_EPC="";
    String epc_dec="";
    void RFID_Read_Callback();
    void scan_tag();
    void setup();
    void loop();
};
#endif
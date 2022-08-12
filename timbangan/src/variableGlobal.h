#ifndef _VARIABLE_GLOBAL_
#define _VARIABLE_GLOBAL_
#include "Arduino.h"
//#include <iostream>

extern String rfid_send;
extern String weight_send;
extern bool Buzz;
extern bool Buzz_weight;

extern unsigned int tahun;
extern unsigned int bulan;
extern unsigned int hari;
extern unsigned int jam;
extern unsigned int menit;
extern unsigned int detik;

extern bool sendData_mqtt;
extern bool sendData_mqtt_dummy;
extern bool lock_state;
extern bool send_ok;
extern bool send_ok_mqtt;
extern bool cek_param;
extern bool state_mqtt;
extern bool send_ok_weight;
extern unsigned long Msg_id;
extern bool save_data;

extern unsigned long ULong_EPC1;
extern unsigned long ULong_EPC2;
extern unsigned long ULong_EPC3;
extern unsigned long ULong_EPC4;
extern unsigned long ULong_EPC5;

#endif
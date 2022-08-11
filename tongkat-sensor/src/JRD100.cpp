#include "JRD100.h"
#include "LCD.h"
#include "Wire.h"


#define jrd_enable 13 // active high
#define rfid_led 32

HardwareSerial Ser2(2);
lcd MyLCD;

byte serial_buf2[25];

uint8_t response[24];

// BB 00 B6 00 02 09 C4 85 7E
uint8_t set_power[9] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x09, 0xC4, 0x85, 0x7E}; // 25 dbm
uint8_t continus_wave[8] = {0xBB, 0x00, 0xB0, 0x00, 0x01, 0xFF, 0xB0, 0x7E};
uint8_t command9[16] = {0xBB, 0x00, 0x39, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x08, 0x4D, 0x7E};
uint8_t disable_FH[8] = {0xBB, 0x00, 0xAD, 0x00, 0x01, 0x00, 0xAE, 0x7E};

#define baru
#ifdef baru
void JRD100::buffer_on_check()
{

  if ((response[0] == 0xBB) && (response[1] == 0x01) && (response[2] == 0xFF) && (response[7] == 0x7E))
  {
    in_progress = false;
    RFID_detect = false;
    return;
  }

  if ((response[0] == 0xBB) && (response[1] == 0x02) && (response[23] == 0x7E))
  {

    // Serial.println("DATA OK");
    memcpy(Mask, &response[8], 12);
    MyData = str.PrintHex8(response, sizeof(response));
    digitalWrite(rfid_led, HIGH);
    String epc = MyData.substring(16, 40);
    if (epc != "")
      RFID_detect = true;
    Serial.println("EPC:" + epc);
    // if (current_EPC != epc)
    current_EPC = epc;

    MyData = "";
    serial_str = "";
    in_progress = false;
    // digitalWrite(rfid_led, LOW);

    String epc_hex = "";
    epc_hex = current_EPC.substring(16, 24);
    Serial.println("EPC:" + epc_hex);
    Serial.println();

    unsigned long ULong_EPC = 0x00000000;
    ULong_EPC = str.StrHextoLong(epc_hex.c_str());
    Serial.println(ULong_EPC, HEX);
    char buf[10] = {0};

    ultoa(ULong_EPC, buf, 10);
    epc_dec = buf;

    if (epc_dec == "0")
    {
      RFID_hold = false;
      current_EPC = "";
      RFID_detect = false;
      return;
    }

    Serial.print("ID:");
    Serial.println(epc_dec);

    MyLCD.rfid_ok = true;
    MyLCD.val_rfid = epc_dec;
    
    RFID_hold = false;
    RFID_detect = true;
  }
  else
    in_progress = false;
}

int data_not_valid = 0;
void JRD100::RFID_Read_Callback()
{
  while (!Ser2.available()) return;
  while ((Ser2.available() > 0) && (!in_progress))
  {

    memset(response, 0, sizeof(response));

    uint8_t single_byte = Ser2.read();
    if (single_byte == 0xBB)
    {
      in_progress = true;
      response[0] = 0xBB;

      int idx = 1;
      do
      {
        single_byte = Ser2.read();
        response[idx] = single_byte;
        idx = idx + 1;
      } while (single_byte != 0x7E);

      data_not_valid = idx;
    }
    if (single_byte == 0x7E)
    {
      if(data_not_valid == 8) 
      {
        memset(response, 0, sizeof(response));
        in_progress = false;
        return;
      }

      for (int i = 0; i < 24; i++)
        Serial.printf("%02X", response[i]);
      Serial.println();

      buffer_on_check();
      memset(response, 0, sizeof(response));
    }
  }
}
#endif

#ifdef OLD
void JRD100::RFID_Read_Callback()
{
  // uint8_t DATA_I_NUB = 0;
  // if (!Ser2.available()) return;
  while (Ser2.available() > 0) //&& (in_progress == false))
  {
    serial_str = "";
    in_progress = true;

    // Get header
    char c = Ser2.read();

    if ((byte)c == 0xBB)
    {
      serial_str += c;

      do
      {
        // Get type
        c = Ser2.read();
        serial_str += c;
      } while ((byte)c != 0x7E);

      if ((byte)c == 0x7E)
      {

        serial_str += c;

        if (serial_str.length() <= 9)
        {
          Serial.println("err");
          serial_str = "";
          in_progress = false;
          return;
        }

        if (serial_str.length() == 25)
        {
          for (int i = 0; i < 25; i++)
            serial_buf2[i] = 0;
          memcpy((char *)serial_buf2, serial_str.c_str(), 25);
          serial_str = "";

          current_EPC = "";
          MyData = "";

          if ((serial_buf2[1] == 0x01) && (serial_buf2[2] == 0xFF) && (serial_buf2[3] == 0x00))
          {
            Serial.println("2 ");
            for (int i = 0; i < 25; i++)
              serial_buf2[i] = 0;
            in_progress = false;
            serial_str = "";
            return;
          }

          if ((serial_buf2[1] == 0x02) && (serial_buf2[2] == 0x22) && (serial_buf2[3] == 0x00))
          {
            // Copy data EPC from single inventory
            memcpy(Mask, &serial_buf2[8], 12);
            MyData = str.PrintHex8(serial_buf2, sizeof(serial_buf2));
            digitalWrite(rfid_led, HIGH);
            String epc = MyData.substring(16, 40);

            if (epc != "")
              RFID_detect = true;

            Serial.println("EPC:" + epc);
            if (current_EPC != epc)
              current_EPC = epc;

            MyData = "";
            serial_str = "";
            digitalWrite(rfid_led, LOW);
            in_progress = false;
          }
        }

        else
        {
          in_progress = false;
          serial_str = "";
        }
      }
    }
  }
}
#endif

void JRD100::setup()
{
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("JRD-100 EPC-Gen2 Setup");
  //Ser2.begin(115200, SERIAL_8N1, 16, 17);
  Ser2.begin(115200, SERIAL_8N1, 16, 17);//16.17
  // delay(500);
  // if(Ser2.availableForWrite()) Ser2.write(set_power, 9);
  // delay(500);

  //
  // Ser2.write(req_set_baud,9);
  // delay(500);
  // Ser2.write(req_set_baud,9);

  // Ser2.begin(19200, SERIAL_8N1, 16, 17);
  // delay(1000);
  // Ser2.write(req_set_baud,9);
  // delay(1000);

  // in_progress = false;
}

void JRD100::scan_tag()
{
  if ((!in_progress) && (enable_scan))
  {
    //for (int i = 0; i < 10; i++)
    //{
      if (Ser2.availableForWrite())
      {
        //Ser2.flush();
        // Serial.println("w");
        RFID_detect = false;
        Ser2.write(req_single_inv, 7);
        delay(10);
        RFID_Read_Callback();
      }
    //}
    //enable_scan = false;
  }
}
void JRD100::loop()
{
  if (enable_scan)
  {
    //MyLCD.draw_rfid("ON SCAN..");
    scan_tag();
  }
}

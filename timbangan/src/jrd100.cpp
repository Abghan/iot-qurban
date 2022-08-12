#include "jrd100.h"
#include "variableGlobal.h"
#include "StrTools.h"

HardwareSerial myPort(1);
String rfid_send;
byte datarx0[250];
char rx_index0 = 0;
char next1 = 0;
bool Buzz = false;

unsigned long ULong_EPC1 = 0x00;
unsigned long ULong_EPC2 = 0x00;
unsigned long ULong_EPC3 = 0x00;
unsigned long ULong_EPC4 = 0x00;
unsigned long ULong_EPC5 = 0x00;

StrTools str;

void JRD100::beginJRD100()
{
    myPort.begin(115200);
}

bool JRD100::send_cmd(void) // INVENTORY = 'BB 00 22 00 00 22 7E'
{
    char message[] = {0xBB, 0x00, 0x03, 0x00, 0x01, 0x00, 0x04, 0X7E};
    // BB 00 22 00 00 22 7e     single inventory
    myPort.write(0XBB);
    myPort.write(0X00);
    myPort.write(0X22);
    myPort.write(0X00);
    myPort.write(0X00);
    myPort.write(0X22);
    myPort.write(0X7E);
    return true;
}

bool JRD100::measure_jrd100(void)
{
    while (myPort.available() > 0)
    {
        char inChar1 = myPort.read();
        if (inChar1 == 0xBB)
        {
            next1 == 1;
        }
        else
        {
            if (inChar1 == 0x7E)
            {
                BIsResultCompleted1 = 1;
                next1 = 0;
            }

            if (inChar1 != 0x7E && rx_index0 <= 200)
            {
                datarx0[rx_index0] = inChar1;
                rx_index0++;
            }
        }

        if (BIsResultCompleted1 == 1)
        {
            BIsResultCompleted1 = 0;
            rx_index0 = 0;
            if (datarx0[1] != 0xFF)
            {
                if (datarx0[0] == 0x02 && datarx0[1] == 0x22)
                {
                    int crc_cek = (int)(datarx0[0] + datarx0[1] + datarx0[2] + datarx0[3]);

                    for (int i = 0; i < datarx0[3]; i++)
                    {
                        crc_cek += (int)datarx0[i + 4];
                    }

                    if (byte(crc_cek) == (datarx0[datarx0[3] + 4]))
                    {
                        char buff_send_epc[10];
                        sprintf(buff_send_epc, "%02X%02X%02X%02X", datarx0[15], datarx0[16], datarx0[17], datarx0[18]);
                        String epc = String(buff_send_epc);

                        ULong_EPC5 = ULong_EPC4;
                        ULong_EPC4 = ULong_EPC3;
                        ULong_EPC3 = ULong_EPC2;
                        ULong_EPC2 = ULong_EPC1;
                        // ULong_EPC1 = strtol(epc.c_str(), NULL, 32);
                        ULong_EPC1 = str.StrHextoLong(epc.c_str());

                        //char buf[10] = {0};
                        //ultoa(ULong_EPC1, buf, 10);
                        //rfid_send = (String)buf;
                        rfid_send = (String)ULong_EPC1;
                        Serial.println(ULong_EPC1);
                        Serial.println(epc);
                        //Serial.println(buf);

                        trig_dataID = true;
                        Buzz = true;
                        myPort.flush();
                    }
                }
                memset(datarx0, 0, 250);
                return true;
            }
            memset(datarx0, 0, 250);
        }
    }

    return false;
}

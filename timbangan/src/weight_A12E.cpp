#include <weight_A12E.h>
#include "HardwareSerial.h"
#include "variableGlobal.h"

HardwareSerial MySerial2(2);
char next=0;
unsigned char BIsResultCompleted=0;
int conting=0;
char datarx[13];
char rx_index=0;

String weight_send;
bool lock_state=false;
bool Buzz_weight;


void Weight_A12E::begin()
{
    MySerial2.begin(9600);
}

bool Weight_A12E::measure(void)
{
    while(MySerial2.available())
    {
        char inChar = (char)MySerial2.read();

        if(next==0)
        {
            if(inChar=='w')
            {
                next=1;
                datarx[0]= inChar;
            }
            else
            {
                next=0;
            }
        }

        else if(next==1)
        {
            if(inChar=='g' || inChar=='n' || inChar=='w' || inChar=='t')
            {
                next=2;  
                datarx[1]= inChar;
            } 
            else
            {
                next=0;
            }

        }

        else if(next==2)
        {
            if(BIsResultCompleted==0)
            {    
                conting++; 
                if(conting==11)
                {
                    conting=0;           
                    next=0; 
                    rx_index=0;
                    BIsResultCompleted=1; 
                    //MySerial2.flush();                  
                }      

                else if (rx_index<=13) 
                {    
                    datarx[rx_index+2] = inChar;
                    rx_index++;
                }    
            }
            else
            {
                next=0;
            }

        }
    }

    if(lock_state==false)
    {
        if(BIsResultCompleted==1)
        {
            BIsResultCompleted=0;  
            uint32_t man = ((datarx[2]-'0')*10000)+((datarx[3]-'0')*1000)+((datarx[4]-'0')*100)+((datarx[5]-'0')*10)+((datarx[6]-'0'));
            if(man<=0 || man >=9999)
            {
                man=0;
                weight_send="0.0";
                Buzz_weight=false;
            }
            else
            {
                if(weight_send==(String(man) + '.' + String(datarx[8])))
                {
                    
                }
                else
                {
                    weight_send=String(man) + '.' + String(datarx[8]);
                    Buzz_weight=true;
                }
            }
            memset(datarx,0,sizeof(datarx));
            MySerial2.flush();
            return true; 
        }
    }
    return false;
}



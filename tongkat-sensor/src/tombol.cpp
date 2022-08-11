#include "tombol.h"
#include "Arduino.h"
#include "temperature.h"
#include "LCD.h"
#include "JRD100.h"
#include "EspMQTTClient.h"


extern void send_suhu_rfid();
extern String temperature_data_on_hold;
extern String rfid_data_on_hold;
extern EspMQTTClient my_client;
extern JRD100 RFIDReader;
extern temperature Temperature;
extern lcd MyLCD;

int my_laststate = LOW;
int tbl_currentState;
int tombol1cnt = 0;
const uint8_t buttonPin[4] = {0, 15, 23, 19};
const uint8_t buttonCount = 4;
uint8_t pressCount = 0;
bool buttonPressedFlag[4];
bool DEBUG = false;

void button1();
void button2();
void button3();
void button4();

typedef void (*functionPtrs)(void);
functionPtrs functions[4] = {&button1, &button2, &button3, &button4};

uint8_t button1_press_count = 0;
void button1() //- untuk mengaktifkan termoscan
{
    button1_press_count++;
    if (button1_press_count == 1)
    {

        if(DEBUG) Serial.println("Thermoscan active");
        MyLCD.draw_notif("          "," SUHU ON  ","          ");
        Temperature.active = true;
        MyLCD.btn_suhu = true;
    }

    else if (button1_press_count >= 2)
    {
        if(DEBUG) Serial.println("Thermoscan Not Active");
        MyLCD.draw_notif("          "," SUHU OFF ","          ");
        Temperature.active = false;
        button1_press_count = 0;
        MyLCD.btn_suhu = false;
        temperature_data_on_hold = "";
    }
}
uint8_t button2_press_count = 0;
void button2() //- untuk mengaktifkan RFID
{

    button2_press_count++;

    if (button2_press_count == 1)
    {
        if(DEBUG) Serial.println("RFID active");
        MyLCD.draw_notif("          "," RFID ON  ","          ");
        RFIDReader.enable_scan = true;
        MyLCD.btn_rfid = true;
    }

    else if (button2_press_count >= 2)
    {
        if(DEBUG) Serial.println("RFID Not Active");
        MyLCD.draw_notif("          "," RFID OFF ","          ");
        RFIDReader.enable_scan = false;
        button2_press_count = 0;
        MyLCD.btn_rfid = false;
        rfid_data_on_hold = "";
    }
}
uint8_t button3_press_count = 0;
void button3() // Send data suhu dan rfid
{
    button3_press_count++;

    if (button3_press_count == 1)
    {
        if(DEBUG) Serial.println("Send data suhu & RFID");
        MyLCD.draw_notif("          "," SEND DATA"," TO SERVER");
        rfid_data_on_hold = RFIDReader.epc_dec;
        temperature_data_on_hold = Temperature.rata_rata_suhu;

        send_suhu_rfid();

        RFIDReader.epc_dec = "";
        Temperature.rata_rata_suhu = "";

        button3_press_count = 0;
    }
}
uint8_t button4_press_count = 0;
void button4() //-untuk disable/enable MQTT
{

    button4_press_count++;

    if (button4_press_count == 1)
    {
        if(DEBUG) Serial.println("CLEAR DATA");
        MyLCD.draw_notif("          ","CLEAR DATA","          ");
        //my_client.enable_MQTT = true;
        rfid_data_on_hold = RFIDReader.epc_dec = "";
        temperature_data_on_hold = Temperature.rata_rata_suhu = "";
        RFIDReader.enable_scan = false;
        Temperature.active = false;
        
        MyLCD.val_rfid = "";
        MyLCD.val_suhu = "";
        MyLCD.rfid_ok = false;
        MyLCD.temperature_ok = false;

        button4_press_count = 0;
    }
    /*
    else if (button4_press_count >= 2)
    {
        //Serial.println("Disable MQTT");
        //my_client.enable_MQTT = false;
        button4_press_count = 0;
    }
    */
}

void tombol::do_stuff()
{

    uint8_t i;
    for (i = 0; i < buttonCount; ++i)
    {
        if (buttonPressedFlag[i])
        {
            buttonPressedFlag[i] = false; // Clear the flag to ensure the action only happens once
            functions[i]();               // Calls one of the 2 functions depending on which button was pressed
        }
    }
}

void tombol::read_button()
{
#define buttonPressed LOW // When the button is pressed the input will be low, this is to remove the confusion this migth cause.

    uint32_t currentMillis = millis();        // Millis times uses to debounce the button
    static uint32_t lastMillis[buttonCount];  // Start of the debounce timeout for each button
    const uint32_t bounceTimeout = 100;        // Debounce time in milliseconds
    bool currentButtonState[buttonCount];     // Holds the current state of each button
    static bool lastButtonState[buttonCount]; // Holds the previous debounced state of the button
    uint8_t i;

    for (i = 0; i < buttonCount; ++i)
    {
        currentButtonState[i] = digitalRead(buttonPin[i]); // Reads the current state of each button and saves the result
        if (lastButtonState[i] != currentButtonState[i])
        { // Checks to see if each button has been pressed or released, at this point each button has not been debounced
            if (currentMillis - lastMillis[i] >= bounceTimeout)
            {                                               // Checks to see if the state of each button has been stable for at least bounceTimeout duration
                lastButtonState[i] = currentButtonState[i]; // At this point the button has been debounced, so save the last state
                if (currentButtonState[i] == buttonPressed)
                {                                // The button might have been pressed or released, this make sure only presses are acted on, not releases
                    buttonPressedFlag[i] = true; // Button press has been detected and debounced, set a flag to indicate to the next function that some action can be taken
                }
            }
        }
        else
        {
            lastMillis[i] = currentMillis; // Saves the current value of millis in last millis so the debounce timer for each button starts from current millis
        }
    }
}

void tombol::setup()
{
    for (uint8_t i = 0; i < buttonCount; ++i)
    {
        pinMode(buttonPin[i], INPUT_PULLUP); // Make the button pins inputs with the internal pull up resistor enabled.
    }
}

void tombol::loop()
{
    read_button();
    do_stuff();
}

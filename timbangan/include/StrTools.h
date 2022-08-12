#ifndef StrTools_H
#define StrTools_H
#include "Arduino.h" 

class StrTools 
{
    private: 
    public:
        void array_to_string(byte array[], unsigned int len, char buffer[]);
        String PrintHex8(uint8_t *data, uint8_t my_length); // prints 8-bit data in hex with leading zeroes
        void PrintHex16(uint16_t *data, uint8_t length); // prints 16-bit data in hex with leading zeroes
        byte csum(byte *data_byte, int data_length);
};
#endif
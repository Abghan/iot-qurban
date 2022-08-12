#ifndef StrTools_H
#define StrTools_H
#include "Arduino.h" 

class StrTools 
{
    private: 
        byte nibble(char c);
    public:
        void array_to_string(byte array[], unsigned int len, char buffer[]);
        String PrintHex8(uint8_t *data, uint8_t my_length); // prints 8-bit data in hex with leading zeroes
        void PrintHex16(uint16_t *data, uint8_t length); // prints 16-bit data in hex with leading zeroes
        byte csum(byte *data_byte, int data_length);
        void hexCharacterStringToBytes(byte *byteArray, const char *hexString);
        void dumpByteArray(const byte * byteArray, const byte arraySize);
        unsigned long StrHextoLong(const char* str_hex_value);
};
#endif
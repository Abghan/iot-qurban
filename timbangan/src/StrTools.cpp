#include "StrTools.h"
//#include "JRD100.h"



void StrTools::array_to_string(byte array[], unsigned int len, char buffer[])
{
   for (unsigned int i = 0; i < len; i++)
   {
      byte nib1 = (array[i] >> 4) & 0x0F;
      byte nib2 = (array[i] >> 0) & 0x0F;
      buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
      buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
   }
   buffer[len*2] = '\0';
}
String StrTools::PrintHex8(uint8_t *data, uint8_t my_length) // prints 8-bit data in hex with leading zeroes
{
      String my_return="";
      char tmp[my_length*2];
      byte my_data[my_length];
      int i;
      
      for (i=0; i<my_length; i++) 
      { 
          my_data[i] = data[i];
          //sprintf(tmp, "%.2X",data[i]); 
          
          //Serial.print(tmp); 
          //Serial.print(" ");
      }

      for (i=0; i<(my_length*2); i++) tmp[i]= 0xFF;  
      array_to_string(my_data,my_length,tmp);
      for (i=0; i<my_length; i++) my_data[i]= 0xFF;
      //Serial.println("");
      return tmp; 
}
void StrTools::PrintHex16(uint16_t *data, uint8_t length) // prints 16-bit data in hex with leading zeroes
{
        char tmp[16];
        for (int i=0; i<length; i++)
        { 
          sprintf(tmp, "0x%.4X",data[i]); 
          Serial.print(tmp); Serial.print(" ");
        }
}
byte StrTools::csum(byte *data_byte, int data_length)
{
  int pindah = data_length - 1;
  int sum = 0;
    for (int i = 0; i < pindah ; i++) {
        sum += data_byte[i];
    }
    
    byte CheckSum8_2s_Complement = 256 - sum;
    return CheckSum8_2s_Complement; 
}

void StrTools::hexCharacterStringToBytes(byte *byteArray, const char *hexString)
{
  bool oddLength = strlen(hexString) & 1;

  byte currentByte = 0;
  byte byteIndex = 0;

  for (byte charIndex = 0; charIndex < strlen(hexString); charIndex++)
  {
    bool oddCharIndex = charIndex & 1;

    if (oddLength)
    {
      // If the length is odd
      if (oddCharIndex)
      {
        // odd characters go in high nibble
        currentByte = nibble(hexString[charIndex]) << 4;
      }
      else
      {
        // Even characters go into low nibble
        currentByte |= nibble(hexString[charIndex]);
        byteArray[byteIndex++] = currentByte;
        currentByte = 0;
      }
    }
    else
    {
      // If the length is even
      if (!oddCharIndex)
      {
        // Odd characters go into the high nibble
        currentByte = nibble(hexString[charIndex]) << 4;
      }
      else
      {
        // Odd characters go into low nibble
        currentByte |= nibble(hexString[charIndex]);
        byteArray[byteIndex++] = currentByte;
        currentByte = 0;
      }
    }
  }
}

void StrTools::dumpByteArray(const byte * byteArray, const byte arraySize)
{

for (int i = 0; i < arraySize; i++)
{
  //Ser1.print("0x");
  //if (byteArray[i] < 0x10)
    //Ser1.print("0");
  //Ser1.print(byteArray[i], HEX);
  //Ser1.print(", ");
}
  //Ser1.println();
}

byte StrTools::nibble(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';

  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;

  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;

  return 0;  // Not a valid hexadecimal character
}

unsigned long StrTools::StrHextoLong(const char* str_hex_value)
{
  byte byteArray[4] = {0};


  hexCharacterStringToBytes(byteArray,str_hex_value);

  unsigned long val = 0;
  val += byteArray[0] << 24;
  val += byteArray[1] << 16;
  val += byteArray[2] << 8;
  val += byteArray[3];
  
  return val;
}


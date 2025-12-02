#include "utils.h"
#include "globals.h"
#include <CRC.h> // robtillaart/CRC@^1.0.3 wersja biblioteki dołączona do projektu
////////////////////////////////////////////////////////////////////////////////////////
// calculateCRC8
////////////////////////////////////////////////////////////////////////////////////////
/* Function to calculate CRC8 checksum
   data: pointer to data array
   length: length of data array
   returns CRC8 checksum
*/
uint8_t calculateCRC8(uint8_t *data, size_t length) {
    // Używamy standardowego wielomianu CRC-8 (0x07), init 0x00, bez inwersji
    // Parametry: (Polynomial, Initial Value, Final XOR, RefIn, RefOut)
    CRC8 crc(0x07, 0x00, 0x00, false, false);
    
    crc.reset();
    for (size_t i = 0; i < length; i++) {
        crc.add(data[i]);
    }
    return crc.calc();
}
////////////////////////////////////////////////////////////////////////////////
// print hex 8 
////////////////////////////////////////////////////////////////////////////////
/* Function to print 8-bit data in hexadecimal format
   data: 8-bit data to print
*/
void PrintHex8(uint8_t data) {
  if (data < 0x10) {
    Serial.print(F("0"));

  }
  Serial.print(data, HEX);
}
////////////////////////////////////////////////////////////////////////////////
// hex char to int
////////////////////////////////////////////////////////////////////////////////
/* Function to convert hexadecimal character to integer
   c: hexadecimal character (0-9, A-F, a-f)
   returns integer value or -1 if invalid character
*/
int hexCharToInt(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return -1;
}
////////////////////////////////////////////////////////////////////////////////
// Clear Serial Buffer
//////////////////////////////////////////////////////////////////////////////
/* Function to clear Serial and mySerial buffers
*/
void clearSerialmySerialBuffer() {
  while (Serial.available()>0) {
    Serial.read();
  }
  while (mySerial.available()>0)
  {
    mySerial.read();
  }
}
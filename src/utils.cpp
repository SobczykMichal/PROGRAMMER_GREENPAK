#include "utils.h"
#include "menu.h"
#include "memory.h"
#include "automatic.h"
#include "globals.h"
#include <Arduino.h>
#include <CRC.h>
////////////////////////////////////////////////////////////////////////////////////////
// calculateCRC8
////////////////////////////////////////////////////////////////////////////////////////
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
void PrintHex8(uint8_t data) {
  if (data < 0x10) {
    Serial.print(F("0"));

  }
  Serial.print(data, HEX);
}
////////////////////////////////////////////////////////////////////////////////
// hex char to int
////////////////////////////////////////////////////////////////////////////////
int hexCharToInt(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return -1;
}
////////////////////////////////////////////////////////////////////////////////
// Clear Serial Buffer
//////////////////////////////////////////////////////////////////////////////
void clearSerialmySerialBuffer() {
  while (Serial.available()>0) {
    Serial.read();
  }
  while (mySerial.available()>0)
  {
    mySerial.read();
  }
  
}
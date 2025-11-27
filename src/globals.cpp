
#include <Arduino.h>
#include "globals.h"
#include <SoftwareSerial.h>

// Store nvmData in PROGMEM to save on RAM
const char nvmString0[]  PROGMEM = "00000000000000000000000000000000";
const char nvmString1[]  PROGMEM = "000000000000F0030000000000000000";
const char nvmString2[]  PROGMEM = "000000000000000000003FF0FF000000";
const char nvmString3[]  PROGMEM = "000000000000000000000FFEFDFFFDDE";
const char nvmString4[]  PROGMEM = "EFFFDCFFFE0000000000000000000000";
const char nvmString5[]  PROGMEM = "00000000000000000000000000000000";
const char nvmString6[]  PROGMEM = "00303000303030300000308000000000";
const char nvmString7[]  PROGMEM = "00000000000000000000000000000000";
const char nvmString8[]  PROGMEM = "BAF8BA6E2F1422300C00000000000000";
const char nvmString9[]  PROGMEM = "00000000000000000000000000780000";
const char nvmString10[] PROGMEM = "0100002004FFFD000002010000020001";
const char nvmString11[] PROGMEM = "00000201000002000100000201000002";
const char nvmString12[] PROGMEM = "00010000020001000000010100000000";
const char nvmString13[] PROGMEM = "00000000000000000000000000000000";
//                               ↓↓ 0 1 2 3 4 5 6 7 8 9 a b c d e f
const char nvmString14[] PROGMEM = "00000000000000000000000000000000";
//                               ↑↑ 0 1 2 3 4 5 6 7 8 9 a b c d e f
const char nvmString15[] PROGMEM = "000000000000000000000000000000A5";
// Store eepromData in PROGMEM to save on RAM
const char eepromString0[]  PROGMEM = "00000000000000000000000000000000";
const char eepromString1[]  PROGMEM = "00000000000000000000000000000000";
const char eepromString2[]  PROGMEM = "00000000000000000000000000000000";
const char eepromString3[]  PROGMEM = "00000000000000000000000000000000";
const char eepromString4[]  PROGMEM = "00000000000000000000000000000000";
const char eepromString5[]  PROGMEM = "00000000000000000000000000000000";
const char eepromString6[]  PROGMEM = "00000000000000000000000000000000";
const char eepromString7[]  PROGMEM = "00000000000000000000000000000000";
const char eepromString8[]  PROGMEM = "00000000000000000000000000000000";
const char eepromString9[]  PROGMEM = "00000000000000000000000000000000";
const char eepromString10[] PROGMEM = "00000000000000000000000000000000";
const char eepromString11[] PROGMEM = "00000000000000000000000000000000";
const char eepromString12[] PROGMEM = "00000000000000000000000000000000";
const char eepromString13[] PROGMEM = "00000000000000000000000000000000";
const char eepromString14[] PROGMEM = "00000000000000000000000000000000";
extern const char eepromString15[] PROGMEM = "00000000000000000000000000000000";
extern const char* const nvmString[16] PROGMEM = {
  nvmString0,
  nvmString1,
  nvmString2,
  nvmString3,
  nvmString4,
  nvmString5,
  nvmString6,
  nvmString7,
  nvmString8,
  nvmString9,
  nvmString10,
  nvmString11,
  nvmString12,
  nvmString13,
  nvmString14,
  nvmString15
};
extern const char* const eepromString[16] PROGMEM = {
  eepromString0,
  eepromString1,
  eepromString2,
  eepromString3,
  eepromString4,
  eepromString5,
  eepromString6,
  eepromString7,
  eepromString8,
  eepromString9,
  eepromString10,
  eepromString11,
  eepromString12,
  eepromString13,
  eepromString14,
  eepromString15
};

int slave_address = 0x00;
bool device_present[16] = {false}; 
char wybor=0; // wybor trybu
uint8_t buffer_seria[256];  // Bufor na 256 znaków hex (czyli 128 bajtów)
bool change_address = false; // flaga zmiany adresu
uint8_t CRC8fromSerial = 0; // wynik CRC-8
uint16_t offsetAddress = 0; // offset do zapisu nowego adresu
int8_t
// RX, TX
SoftwareSerial mySerial(11, 12);
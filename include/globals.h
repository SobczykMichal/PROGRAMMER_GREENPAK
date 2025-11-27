#pragma once
#include <Arduino.h>
#include <SoftwareSerial.h>
#define NVM_CONFIG 0x02
#define EEPROM_CONFIG 0x03
#define VDD 2 // GreenPAK VDD
#define NVM_SLAVE_ADDR_PAGE 12
#define NVM_SLAVE_ADDR_BYTE 10
#define NVM_SLAVE_ADDR_OFFSET (NVM_SLAVE_ADDR_PAGE * 16 + NVM_SLAVE_ADDR_BYTE)

// Store nvmData in PROGMEM to save on RAM
extern const char nvmString0[]  PROGMEM;
extern const char nvmString1[]  PROGMEM;
extern const char nvmString2[]  PROGMEM;
extern const char nvmString3[]  PROGMEM;
extern const char nvmString4[]  PROGMEM;
extern const char nvmString5[]  PROGMEM;
extern const char nvmString6[]  PROGMEM;
extern const char nvmString7[]  PROGMEM;
extern const char nvmString8[]  PROGMEM;
extern const char nvmString9[]  PROGMEM;
extern const char nvmString10[] PROGMEM;
extern const char nvmString11[] PROGMEM;
extern const char nvmString12[] PROGMEM;
extern const char nvmString13[] PROGMEM;
//                               ↓↓ 0 1 2 3 4 5 6 7 8 9 a b c d e f
extern const char nvmString14[] PROGMEM;
//                               ↑↑ 0 1 2 3 4 5 6 7 8 9 a b c d e f
extern const char nvmString15[] PROGMEM;
// Store eepromData in PROGMEM to save on RAM
extern const char eepromString0[]  PROGMEM;
extern const char eepromString1[]  PROGMEM;
extern const char eepromString2[]  PROGMEM;
extern const char eepromString3[]  PROGMEM;
extern const char eepromString4[]  PROGMEM;
extern const char eepromString5[]  PROGMEM;
extern const char eepromString6[]  PROGMEM;
extern const char eepromString7[]  PROGMEM;
extern const char eepromString8[]  PROGMEM;
extern const char eepromString9[]  PROGMEM;
extern const char eepromString10[] PROGMEM;
extern const char eepromString11[] PROGMEM;
extern const char eepromString12[] PROGMEM;
extern const char eepromString13[] PROGMEM;
extern const char eepromString14[] PROGMEM;
extern const char eepromString15[] PROGMEM;
extern const char* const nvmString[16] PROGMEM;
extern const char* const eepromString[16] PROGMEM;

extern int slave_address;
extern bool device_present[16]; 
extern char wybor; // wybor trybu
extern uint8_t buffer_seria[256];  // Bufor na 256 znaków hex (czyli 128 bajtów)
extern bool change_address; // flaga zmiany adresu
extern uint8_t CRC8fromSerial; // wynik CRC-8
extern uint16_t offsetAddress; // offset do zapisu nowego adresu
extern int8_t lastOperationStatus; // status ostatniej operacji
// RX, TX
extern SoftwareSerial mySerial;

#include <Arduino.h>
#include <Wire.h>
#include <stdlib.h>
#include <string.h>
#include <EEPROM.h>

#define NVM_CONFIG 0x02
#define EEPROM_CONFIG 0x03
#define VDD 2

int count = 0;
uint8_t slave_address = 0x00;
bool device_present[16] = {false};
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
const char eepromString15[] PROGMEM = "00000000000000000000000000000000";

const char* const nvmString[16] PROGMEM = {
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

const char* const eepromString[16] PROGMEM = {
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

void requestSlaveAddress();
char requestNVMorEeprom();
char requestSERIALorMEMEPROMorMEMFLASH();
char query(String queryString);
void PrintHex8(uint8_t data);
int readChip(char NVMorEEPROM);
int eraseChip(char NVMorEEPROM);
int writeChip(char NVMorEEPROM, char SERIALorMEM);
void ping();
int ackPolling(int addressForAckPolling);
void powercycle();
uint8_t hexCharToInt(char hexChar);
void save_to_EEPROM(char NVMorEEPROM, uint8_t*data, size_t rozmiar);
char requestUpdateEEPROM();
////////////////////////////////////////////////////////////////////////////////
// setup 
////////////////////////////////////////////////////////////////////////////////
void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  Wire.setClock(400000);
  Serial.begin(115200);
  pinMode(VDD, OUTPUT);  // This will be the GreenPAK's VDD
  digitalWrite(VDD, HIGH);
  delay(100);
  Serial.println(F("DEBUGOWANIE"));
}

////////////////////////////////////////////////////////////////////////////////
// loop 
////////////////////////////////////////////////////////////////////////////////
void loop() {
  slave_address = 0x00;
  char NVMorEEPROM = 0;
  char SERIALorMEM = 0;
  //Serial.println(F("Clearing serial buffer..."));
  while (Serial.available() > 0) Serial.read();
  char selection = query("\nMENU: r = read, e = erase, w = write, p = ping\n");

  switch (selection)
  {
    case 'r': 
        Serial.println(F("Reading chip!"));
        requestSlaveAddress();
        NVMorEEPROM = requestNVMorEeprom();
        readChip(NVMorEEPROM);
        // Serial.println(F("Done Reading!"));
        break;
    case 'e': 
        Serial.println(F("Erasing Chip!"));
        requestSlaveAddress();
        NVMorEEPROM = requestNVMorEeprom();
        
        if (eraseChip(NVMorEEPROM) == 0) {
          // Serial.println(F("Done erasing!"));
        } else {
          Serial.println(F("Erasing did not complete correctly!"));
        }
        delay(100);
        ping();
        break;
    case 'w': 
        Serial.println(F("Writing Chip!"));
        requestSlaveAddress();
        NVMorEEPROM = requestNVMorEeprom();
        SERIALorMEM = requestSERIALorMEMEPROMorMEMFLASH();
        if (eraseChip(NVMorEEPROM) == 0) {
          // Serial.println(F("Done erasing!"));
        } else {
          Serial.println(F("Erasing did not complete correctly!"));
        }

        ping();

        if (writeChip(NVMorEEPROM, SERIALorMEM) == 0) {
          // Serial.println(F("Done writing!"));
        } else {
          Serial.println(F("Writing did not complete correctly!"));
        }

        ping();

        readChip(NVMorEEPROM);
        // Serial.println(F("Done Reading!"));
        break;
    case 'p': 
        Serial.println(F("Pinging!"));
        ping();
        // Serial.println(F("Done Pinging!"));
        break;
    default:
        break;
  }
}
////////////////////////////////////////////////////////////////////////////////
// hex char to int
////////////////////////////////////////////////////////////////////////////////
uint8_t hexCharToInt(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return 0; // lub lepiej: obsłuż błąd
}
////////////////////////////////////////////////////////////////////////////////
// request slave address 
////////////////////////////////////////////////////////////////////////////////
void requestSlaveAddress() {
  ping();

  while(1) {
    //Serial.println(F("Clearing serial buffer..."));
    while (Serial.available() > 0) Serial.read();
    char mySlaveAddress = query("Submit slave address, 0-F: ");
    slave_address = hexCharToInt(mySlaveAddress);
    //Check for a valid slave address
    if (device_present[slave_address] == false)
    {
      Serial.println(F("You entered an incorrect slave address. Submit slave address, 0-F: "));
      continue;
    }
    else {
      PrintHex8(slave_address);
      Serial.println();
      return;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// request NVM or EEPROM 
////////////////////////////////////////////////////////////////////////////////
char requestNVMorEeprom() {
  while (1)
  {
    //Serial.println(F("Clearing serial buffer..."));
    while (Serial.available() > 0) Serial.read();    
    char selection = query("MENU: n = NVM, e = EEPROM: ");

    switch (selection)
    {
      case 'n':
          Serial.println(F("NVM"));
          return 'n';
      case 'e':
          Serial.println(F("EEPROM"));
          return 'e';
      default:
          Serial.println(F("Invalid selection. You did not enter \"n\" or \"e\"."));
          continue;
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
// request SERIAL or MEMORY
////////////////////////////////////////////////////////////////////////////////
char requestSERIALorMEMEPROMorMEMFLASH() {
  while (1)
  {
    //Serial.println(F("Clearing serial buffer..."));
    while (Serial.available() > 0) Serial.read();
    char selection = query("MENU: s = SERIAL, m = MEMORY: ");
    char mem_selection;
    char mem_choice = 0;
    
    switch (selection)
    {
      case 's':
          Serial.println(F("SERIAL"));
          return 's';
      case 'm':
          //Serial.println(F("Clearing serial buffer..."));
          while (Serial.available() > 0) Serial.read();
          Serial.println(F("MEMORY"));
          mem_selection = query("MENU: a = ARDUINO EEPROM MEMORY, f = FLASH ARDUINO MEMORY\n");
          switch (mem_selection)
          {
            case 'a':
                Serial.print(F("Using ARDUINO EEPROM MEMORY"));
                Serial.println();                
                mem_choice = 'a';
                break;
            case 'f':
                Serial.print(F("Using FLASH ARDUINO MEMORY"));
                Serial.println();                
                mem_choice = 'f';
                break;
            default:
                Serial.println(F("Invalid selection. You did not enter \"a\" or \"f\"."));
                continue;
          }
          return mem_choice;
      default:
          Serial.println(F("Invalid selection. You did not enter \"s\" or \"m\"."));
          continue;
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
// request update EEPROM
////////////////////////////////////////////////////////////////////////////////
char requestUpdateEEPROM() {
  while (1)
  {
    //Serial.println(F("Clearing serial buffer..."));
    while (Serial.available() > 0) Serial.read();
    char selection = query("MENU: u = update ARDUINO EEPROM with written data, i = ignore update\n");

    switch (selection)
    {
      case 'u':
          Serial.print(F("Update EEPROM"));
          Serial.println();
          return 'u';
      case 'i':
          Serial.print(F("Ignore Update"));
          Serial.println();
          return 'i';
      default:
          Serial.println(F("Invalid selection. You did not enter \"u\" or \"n\"."));
          continue;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// query 
////////////////////////////////////////////////////////////////////////////////
char query(String queryString) {
  Serial.println();

  Serial.print(queryString);
  while (1) {
    if (Serial.available() > 0) {
      char firstChar = Serial.read();
      return firstChar;
    }
  }
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
// readChip 
////////////////////////////////////////////////////////////////////////////////
int readChip(char NVMorEEPROM) {
  int control_code = slave_address << 3;

  if (NVMorEEPROM == 'n')
  {
    control_code |= NVM_CONFIG;
  }
  else if (NVMorEEPROM == 'e')
  {
    control_code |= EEPROM_CONFIG;
  }

  for (int i = 0; i < 16; i++) {
    Wire.beginTransmission(control_code);
    Wire.write(i << 4);
    Wire.endTransmission(false);
    delay(10);
    Wire.requestFrom(control_code, 16);

    while (Wire.available()) {
      PrintHex8(Wire.read());
    }
    Serial.println();
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// eraseChip 
////////////////////////////////////////////////////////////////////////////////
int eraseChip(char NVMorEEPROM) {
  int control_code = slave_address << 3;
  int addressForAckPolling = control_code;

  for (uint8_t i = 0; i < 16; i++) {
    Serial.print(F("Erasing page: 0x"));
    PrintHex8(i);
    Serial.print(F(" "));

    Wire.beginTransmission(control_code);
    Wire.write(0xE3);

    if (NVMorEEPROM == 'n')
    {
      Serial.print(F("NVM "));
      Wire.write(0x80 | i);
    }
    else if (NVMorEEPROM == 'e')
    {
      Serial.print(F("EEPROM "));
      Wire.write(0x90 | i);
    }
    
/* To accommodate for the non-I2C compliant ACK behavior of the Page Erase Byte, we've removed the software check for an I2C ACK
 *  and added the "Wire.endTransmission();" line to generate a stop condition.
 *  - Please reference "Issue 2: Non-I2C Compliant ACK Behavior for the NVM and EEPROM Page Erase Byte"
 *    in the SLG46824/6 (XC revision) errata document for more information. */

//    if (Wire.endTransmission() == 0) {
//      Serial.print(F("ack "));
//    } 
//    else { 
//      Serial.print(F("nack "));  
//      return -1;
//    }

    Wire.endTransmission();

    if (ackPolling(addressForAckPolling) == -1)
    {
      return -1;
    } else {
      Serial.print(F("ready "));
      delay(100);
    }
    Serial.println();
  }

  powercycle();
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// writeChip 
////////////////////////////////////////////////////////////////////////////////
int writeChip(char NVMorEEPROM, char SERIALorMEM) {
  int control_code = 0x00;
  int addressForAckPolling = 0x00;
  bool NVM_selected = false;
  bool EEPROM_selected = false;
  bool  data_from_SERIAL = false;
  bool  data_from_MEMORY = false;
  bool  data_from_ARDUINO_EEPROM = false;
  bool  data_from_ARDUINO_FLASH = false;
  uint8_t buffer_seria[256];  // Bufor na 256 znaków hex (czyli 128 bajtów)
  char updateSelection;
  if (NVMorEEPROM == 'n')
  {
    // Serial.println(F("Writing NVM"));
    // Set the slave address to 0x00 since the chip has just been erased
    slave_address = 0x00;
    // Set the control code to 0x00 since the chip has just been erased
    control_code = 0x00;
    control_code |= NVM_CONFIG;
    NVM_selected = true;
    addressForAckPolling = 0x00;
  }
  else if (NVMorEEPROM == 'e')
  {
    // Serial.println(F("Writing EEPROM"));
    control_code = slave_address << 3;
    control_code |= EEPROM_CONFIG;
    EEPROM_selected = true;
    addressForAckPolling = slave_address << 3;
  }

  Serial.print(F("Control Code: 0x"));
  PrintHex8(control_code);
  Serial.println();
  if (SERIALorMEM == 's')
  {
    Serial.println(F("Getting data from SERIAL input"));
    data_from_SERIAL = true;
  }
  else if (SERIALorMEM == 'a')
  {
    Serial.println(F("Getting data from ARDUINO EEPROM MEMORY"));
    data_from_MEMORY = true;
    data_from_ARDUINO_EEPROM = true;
  }
  else if (SERIALorMEM == 'f')
  {
    Serial.println(F("Getting data from ARDUINO FLASH MEMORY"));
    data_from_MEMORY = true;
    data_from_ARDUINO_FLASH = true;
  }
  if (data_from_SERIAL)
  {
    //Serial.println(F("PETLA if data from serial"));
    
    int index = 0;
    char c;
    uint8_t zmienna1;
    uint8_t zmienna2;
      bool first = true;  // Flaga do śledzenia, czy aktualny znak jest parzysty czy nieparzysty
    while (index < 256) {
      if (Serial.available()) {
        c = Serial.read();

        //avoid new lines signs
        if (c == '\n' || c == '\r') continue;

        // convert char to int
        uint8_t val = hexCharToInt(c);

        if (first) {
          zmienna1 = val << 4;  // MSB
          first = false;
        } else {
          zmienna2 = val; // LSB
          buffer_seria[index] = zmienna1 | zmienna2; // Combine MSB and LSB
          //Serial.print(F("PRZYPISANE DO BUFFERA: ")); debugowanie
          //Serial.println(buffer_seria[index], HEX); debugowanie
          index++;
          first = true;
        }
      }
    }
  //Serial.println(sizeof(buffer_seria));
  updateSelection = requestUpdateEEPROM();
    }
    // http://www.gammon.com.au/progmem

    // Serial.println(F("New NVM data:"));
    if(data_from_MEMORY)
    {
      if(data_from_ARDUINO_FLASH)
      {
    for (size_t i = 0; i < 16; i++)
    {
      // Pull current page NVM from PROGMEM and place into buffer
      char buffer [64];
      if (NVM_selected)
      {
        char * ptr = (char *) pgm_read_word (&nvmString[i]);
        strcpy_P(buffer, ptr);
      }
      else if (EEPROM_selected)
      {
        char * ptr = (char *) pgm_read_word (&eepromString[i]);
        strcpy_P(buffer, ptr);
      }

      for (size_t j = 0; j < 16; j++)
      {
        String temp = (String)buffer[2 * j] + (String)buffer[(2 * j) + 1];
        long myNum = strtol(&temp[0], NULL, 16);
        buffer_seria[i * 16 + j] = (uint8_t) myNum;
      }
    }
    }
    if (data_from_ARDUINO_EEPROM)
    {
    for (size_t i = 0; i < 16; i++)
    {
      for (size_t j = 0; j < 16; j++)
      {
        size_t address = i * 16 + j;
        uint8_t value = EEPROM.read(address);
        buffer_seria[i * 16 + j] = value;
      }
    }
    //Serial.println();
  }
    if (NVM_selected)
    {
      while(1) {
        //Serial.println(F("Clearing serial buffer..."));
        while (Serial.available() > 0) Serial.read();
        char newSA = query("Enter new slave address 0-F: ");
        int temp = hexCharToInt(newSA);
        
        if (temp < 0 || temp > 15)
        {
          Serial.println(temp);
          Serial.println(F(" is not a valid slave address."));
          continue;
        }
        else 
        {
          slave_address = temp;
          Serial.print(F("0x"));
          PrintHex8(slave_address);
          Serial.println();
          break;
        }
      }
      buffer_seria[12*16+10] = slave_address;
    }
  }
    // Write each byte of data_array[][] array to the chip
    for (int i = 0; i < 16; i++) {
      Wire.beginTransmission(control_code);
      Wire.write(i << 4);

      PrintHex8(i);
      Serial.print(F(" "));
      
      for (int j = 0; j < 16; j++) {
        if(data_from_MEMORY){
          Wire.write(buffer_seria[i*16 + j]);
          PrintHex8(buffer_seria[i*16 + j]);
        }
        if(data_from_SERIAL){
          Wire.write(buffer_seria[i*16 + j]);
          PrintHex8(buffer_seria[i*16 + j]);
        }
      }
      
      if (Wire.endTransmission() == 0) {
        Serial.print(F(" ack "));
      } else {
        Serial.print(F(" nack\n"));
        Serial.println(F("Oh No! Something went wrong while programming!"));
        return -1;
      }

      if (ackPolling(addressForAckPolling) == -1)
      {
        return -1;
      } else {
        Serial.println(F("ready"));
        delay(100);
      }
    }

  if (updateSelection == 'u') {
    save_to_EEPROM(NVMorEEPROM, buffer_seria, sizeof(buffer_seria));
  }
    powercycle();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// ping 
////////////////////////////////////////////////////////////////////////////////
void ping() {
  delay(100);
  for (int i = 0; i < 16; i++) {
    Wire.beginTransmission(i << 3);

    if (Wire.endTransmission() == 0) {
      Serial.print(F("device 0x"));
      PrintHex8(i);
      Serial.println(F(" is present"));
      device_present[i] = true;
    } else {
      device_present[i] = false; 
    }
  }
  delay(100);
}

////////////////////////////////////////////////////////////////////////////////
// ack polling 
////////////////////////////////////////////////////////////////////////////////
int ackPolling(int addressForAckPolling) {
    int nack_count = 0;
    while (1) {
      Wire.beginTransmission(addressForAckPolling);
      if (Wire.endTransmission() == 0) {
        return 0;
      }
      if (nack_count >= 1000)
      {
        Serial.println(F("Geez! Something went wrong while programming!"));
        return -1;
      }
      nack_count++;
      delay(1);
    }
}

////////////////////////////////////////////////////////////////////////////////
// power cycle 
////////////////////////////////////////////////////////////////////////////////
void powercycle() {
  Serial.println(F("Power Cycling!"));
  digitalWrite(VDD, LOW);
  delay(500);
  digitalWrite(VDD, HIGH);
  // Serial.println(F("Done Power Cycling!"));
}
////////////////////////////////////////////////////////////////////////////////
// save to EEPROM
////////////////////////////////////////////////////////////////////////////////
void save_to_EEPROM(char NVMorEEPROM, uint8_t*data, size_t rozmiar) {
  boolean success=false;
  if(NVMorEEPROM == 'n') {
    Serial.println(F("Saving NVM data to EEPROM..."));
    // Copy NVM data to EEPROM data array
    for (size_t address = 0; address < rozmiar; address++) {
        EEPROM.update(address, data[address]);
      }
      success = true;
  }
  if(NVMorEEPROM == 'e') {
    Serial.println(F("Saving EEPROM data to EEPROM..."));
    for (size_t address = 0; address < rozmiar; address++) {
    EEPROM.update(address+rozmiar, data[address]);
    }
      success = true;
  }
if(success) Serial.println(F("Done Saving to EEPROM!"));
else Serial.println(F("Error Saving to EEPROM!"));
}
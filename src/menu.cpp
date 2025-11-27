#include "menu.h"
#include "memory.h"
#include "automatic.h"
#include "globals.h"
#include "utils.h"
#include <Arduino.h>
////////////////////////////////////////////////////////////////////////////////
// request slave address 
////////////////////////////////////////////////////////////////////////////////
char requestSlaveAddress() {
  ping();

  while(1) {
    clearSerialmySerialBuffer();
    char mySlaveAddress = query(2);
    if (mySlaveAddress=='q'){
      clearSerialmySerialBuffer();
      Serial.println(F("Back to main menu"));
      return 'q';
    } 
    else slave_address = hexCharToInt(mySlaveAddress);
    //Check for a valid slave address
    if (device_present[slave_address] == false)
    {
      Serial.println(F("You entered an incorrect slave address. Submit slave address, 0-F: "));
      continue;
    }
    else {
      PrintHex8(slave_address);
      Serial.println();
      return 0;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// request NVM or EEPROM 
////////////////////////////////////////////////////////////////////////////////
char requestNVMorEeprom() {
  while (1)
  {
    clearSerialmySerialBuffer();
    char selection = query(3);

    switch (selection)
    {
      case 'n':
          Serial.println(F("NVM"));
          delay(10);
          return 'n';
      case 'e':
          Serial.println(F("EEPROM"));
          delay(10);
          return 'e';
      case 'q':
          Serial.println(F("Back to main menu"));
          clearSerialmySerialBuffer();
          return 'q';
      default:
          Serial.println(F("Invalid selection. You did not enter \"n\" or \"e\"."));
          continue;
    }
  }
}
char requestGPAKorArduino(){
  while(1){
    clearSerialmySerialBuffer();
    char selection = query(8);

    switch (selection){
      case 'g':
        Serial.println(F("GreenPAK"));
        delay(10);
      return 'g';
      case 'a':
        Serial.println(F("ARDUINO"));
        delay(10);
      return 'a';
      case 'q':
        Serial.println(F("Back to main menu"));
        clearSerialmySerialBuffer();
      return 'q';
      default:
        Serial.println(F("Invalid selection. You did not enter \"g\" \"a\" ."));
      continue;
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
// request ARDUINO EEPROM or FLASH
////////////////////////////////////////////////////////////////////////////////
char requestARDU_EEPROMorFLASH() {
  char mem_selection;
  char mem_choice = 0; 
  while(1){
  mem_selection = query(5);
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
        clearSerialmySerialBuffer();
        Serial.println(F("Invalid selection. You did not enter \"a\" or \"f\"."));
        continue;
  }
  return mem_choice;
  }
}

////////////////////////////////////////////////////////////////////////////////
// request SERIAL or MEMORY
////////////////////////////////////////////////////////////////////////////////
char requestSERIALorMEM() {
  while (1)
  {
    clearSerialmySerialBuffer();
    char selection = query(4);
    switch (selection)
    {
      case 's':
          clearSerialmySerialBuffer();
          Serial.println(F("SERIAL"));
          return 's';
      case 'm':
          clearSerialmySerialBuffer();
          Serial.println(F("MEMORY"));
          return 'm';
      case 'q' :
          clearSerialmySerialBuffer();
          Serial.println(F("Back to main menu"));
          return 'q';
          
      default:
          clearSerialmySerialBuffer();
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
    clearSerialmySerialBuffer();
    char selection = query(6);

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
          Serial.println(F("Invalid selection. You did not enter \"u\" or \"i\"."));
          continue;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// query 
////////////////////////////////////////////////////////////////////////////////
char query(uint8_t which_menu) {
  Serial.println();
switch (which_menu)
{
case 1:
  Serial.println(F("\nMENU: r = read, e = erase, w = write, p = ping, a = automatic mode"));
  break;
case 2:
  Serial.println(F("Submit slave address, 0-F:"));
  break;
case 3:
  Serial.println(F("MENU: n = NVM, e = EEPROM:"));
  break;
case 4:
  Serial.println(F("MENU: s = SERIAL, m = MEMORY:"));
  break;
case 5:
  Serial.println(F("MENU: a = ARDUINO EEPROM MEMORY, f = FLASH ARDUINO MEMORY"));
  break;
case 6:
  Serial.println(F("MENU: u = update, i = ignore:"));
  break;
case 7:
  Serial.println(F("Enter a new slave address, 0-F:"));
  break;
case 8:
  Serial.println(F("MENU: g = GREENPAK, A= ARDUINO:"));
  break;
default:
  break;
}
  while (1) {
    if (Serial.available() > 0) {
      char firstChar = Serial.read();
      return firstChar;
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
// select mode
////////////////////////////////////////////////////////////////////////////////

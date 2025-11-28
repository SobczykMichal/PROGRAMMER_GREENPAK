#include "menu.h"
#include "memory.h"
#include "automatic.h"
#include "globals.h"
#include "utils.h"
#include <Arduino.h>
////////////////////////////////////////////////////////////////////////////////
// request slave address 
////////////////////////////////////////////////////////////////////////////////
/* Function to request slave address
   returns 'q' if user wants to quit to main menu
   otherwise sets global slave_address variable and returns 0
*/
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
/* Function to request NVM or EEPROM
   returns 'n' for NVM or 'e' for EEPROM
*/
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
////////////////////////////////////////////////////////////////////////////////
//// request GPAK or ARDUINO
////////////////////////////////////////////////////////////////////////////////
/* Function to request GreenPAK or Arduino
   returns 'g' for GREENPAK or 'a' for ARDUINO
*/
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
/* Function to request Arduino memory type
   returns 'a' for ARDUINO EEPROM or 'f' for FLASH
*/
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
/* Function to request data source type
   returns 's' for SERIAL or 'm' for MEMORY
*/
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
/* Function to request whether to update Arduino EEPROM
   returns 'u' for update or 'i' for ignore
*/
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
/* Function to display menu based on which_menu parameter
   and wait for user input
   which_menu: menu identifier to display
   returns the character input by the user
*/
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
// StatusOperation
////////////////////////////////////////////////////////////////////////////////
/* Function to display status of the last operation
   status: 0 for success, negative values for different errors
*/
void StatusOperation(int8_t status){
  if(status==0){
    Serial.println(F("Operation completed successfully."));
    mySerial.println(F("OK"));
  }
  else if(status==-1){
    Serial.println(F("Operation failed due to communication error."));
    mySerial.println(F("E1"));
  }
  else if(status==-2){
    Serial.println(F("Operation failed due to invalid address."));
    mySerial.println(F("E2"));
  }
  else if(status==-3){
    Serial.println(F("Operation failed due to invalid memory type."));
    mySerial.println(F("E3"));
  }
  else if(status==-4){
    Serial.println(F("Operation failed due to invalid source type."));
    mySerial.println(F("E4"));
  }
  else if(status==-5){
    Serial.println(F("Operation failed due to invalid storage type."));
    mySerial.println(F("E5"));
  }
  else if(status==-6){
    Serial.println(F("Operation failed due to invalid source type."));
    mySerial.println(F("E6"));
  }
  else if(status==-7){
    Serial.println(F("Operation failed due to error updating Arduino EEPROM."));
    mySerial.println(F("E7"));
  }  
  else if(status==-8){
    Serial.println(F("Operation failed due to invalid new address."));
    mySerial.println(F("E8"));
  }
  else if(status==-9){
    Serial.println(F("Operation failed. Wrong parameter or something else."));
    mySerial.println(F("E9"));
  }
  else if(status==-10){
    Serial.println(F("Erasing failed"));
    mySerial.println(F("E10"));
  }
  else if(status==-11){
    Serial.println(F("Error reading from GreenPAK. Different data than buffer."));
    mySerial.println(F("E11"));
  }
    else if(status==-12){
    Serial.println(F("Operation failed due to error during programming or ACK."));
    mySerial.println(F("E12"));
  }
}
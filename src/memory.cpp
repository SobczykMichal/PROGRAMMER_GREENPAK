#include "memory.h"
#include "automatic.h"
#include "globals.h"
#include "utils.h"
#include "menu.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
////////////////////////////////////////////////////////////////////////////////
// readProgram
////////////////////////////////////////////////////////////////////////////////
/* Function to read data from GreenPAK NVM/EEPROM or from Arduino EEPROM/Flash
   NVMorEEPROM: 'n' for NVM, 'e' for EEPROM
    CheckOrRead: 15 for check, 16 for read and print
    GPAKorArdu: 'g' for GreenPAK, 'a' for Arduino
    ARDU_FLASHorEEPROM: 'a' for Arduino EEPROM, 'f' for Arduino Flash
    returns 0 if OK or -11 if data incorrect during checking
*/
int readProgram(char NVMorEEPROM, uint8_t CheckOrRead, char GPAKorArdu, char ARDU_FLASHorEEPROM) { 
  Serial.println(F("Read program"));
  int control_code = slave_address << 3;
  int index = 0;
  if (NVMorEEPROM == 'n')
  {
    offsetAddress=0;// NVM starts from address 0 in Arduino EEPROM
    control_code |= NVM_CONFIG;
  }
  else if (NVMorEEPROM == 'e')
  {
    offsetAddress=256; // EEPROM starts from address 256 in Arduino EEPROM
    control_code |= EEPROM_CONFIG;
  }

  if((NVMorEEPROM == 'n' || NVMorEEPROM == 'e') && GPAKorArdu == 'g'){
    bool checkState=true;
    for (uint8_t i = 0; i < CheckOrRead; i++) {
      Wire.beginTransmission(control_code);
      Wire.write(i << 4);
      Wire.endTransmission(false);
      delay(10);
      Wire.requestFrom(control_code, 16);
      uint8_t gpakVal=0; // wartość z GreenPAK
      while (Wire.available()) {
        if(CheckOrRead == 15){ // check data
          gpakVal= Wire.read();
          if (buffer_seria[index]== gpakVal){
            index++;
            checkState=true;
            continue;
          } 
          else{
            /*Serial.println(F("Incorrect data! Blad na idneksie:")); // debug
            Serial.println(index); // debug
            Serial.print(F("Wartosc greenpaka: ")); // debug
            PrintHex8(gpakVal); // debug
            Serial.println(); // debug
            Serial.print(F("Wartosc z buffer_seria: ")); // debug
            PrintHex8(buffer_seria[index]); // debug
            index++;
            Serial.println();*/
            checkState=false;
            break;
          }  
        }
        if (CheckOrRead == 16){ // read and print
          PrintHex8(Wire.read());
        }
      }
      if (CheckOrRead == 16){
        Serial.println();
      } 
    }
    if (CheckOrRead == 15){
      if(checkState){
        //Serial.println(F("OK")); // debug
        //mySerial.println(F("OK"));
        return 0;
      }
      else {
       //Serial.println(F("E")); // debug
        //mySerial.println(F("E"));
        return -11;
      }
    }
  }
  else if(GPAKorArdu == 'a'){
    if (ARDU_FLASHorEEPROM== 'a'){
      for (size_t i = 0; i < 16; i++)
      {
        for (size_t j = 0; j < 16; j++)
        {
          size_t address = i * 16 + j;
          uint8_t value = EEPROM.read(address+offsetAddress);
          buffer_seria[i * 16 + j] = value;
          PrintHex8(buffer_seria[i * 16 + j]);
        }
        Serial.println();
      }
    }
    else if(ARDU_FLASHorEEPROM == 'f'){
      for (size_t i = 0; i < 16; i++){
        // Pull current page NVM from PROGMEM and place into buffer
        char buffer [33];
        if (NVMorEEPROM== 'n'){
          char * ptr = (char *) pgm_read_word (&nvmString[i]);
          strcpy_P(buffer, ptr);
        }
        else if (NVMorEEPROM == 'e'){
          char * ptr = (char *) pgm_read_word (&eepromString[i]);
          strcpy_P(buffer, ptr);
        }

        for (size_t j = 0; j < 16; j++)
        {
          int val= (hexCharToInt(buffer[2 * j])<<4)+ hexCharToInt(buffer[(2 * j) + 1]);
          buffer_seria[i * 16 + j] = val;
          PrintHex8(buffer_seria[i * 16 + j]);
        }
        Serial.println();
      }
      
      return 0;
    } 
  }
  return 0;
}
////////////////////////////////////////////////////////////////////////////////
// eraseChip 
////////////////////////////////////////////////////////////////////////////////
/* Function to erase GreenPAK NVM/EEPROM
   NVMorEEPROM: 'n' for NVM, 'e' for EEPROM
   returns 0 if OK or -11 if something wrong during erasing
*/
int eraseChip(char NVMorEEPROM) {
  Serial.println(F("Erase chip"));
  uint8_t control_code = slave_address << 3;
  uint8_t addressForAckPolling = control_code;

  for (uint8_t i = 0; i < 16; i++) {
    if(selectionMode == 'm'){
      Serial.print(F("Erasing page: 0x"));
      PrintHex8(i);
      Serial.print(F(" "));
    }
    Wire.beginTransmission(control_code);
    Wire.write(0xE3);
    if (NVMorEEPROM == 'n')
    {
      if (selectionMode == 'm'){
        Serial.print(F("NVM ")); //print only in manual mode
      } 
      Wire.write(0x80 | i);
    }
    else if (NVMorEEPROM == 'e')
    {
      if (selectionMode == 'm'){
        Serial.print(F("EEPROM ")); //print only in manual mode
      } 
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
      return -11;
    } else {
      if(selectionMode == 'm'){
        Serial.print(F("ready ")); // print only in manual mode
      } 
      delay(100);
    }
    if(selectionMode == 'm'){
      Serial.println(); // print only in manual mode
    }  
  }
  powercycle();
  return 0;
}
////////////////////////////////////////////////////////////////////////////////
// ping 
////////////////////////////////////////////////////////////////////////////////
/* Function to ping all possible addresses (0-15) on the I2C bus
   and mark which devices are present in device_present[] array
*/
void ping() {
  delay(100);
  for (uint8_t i = 0; i < 16; i++) {
    Wire.beginTransmission(i << 3);

    if (Wire.endTransmission() == 0) {
     // if(wybor == 'm'){ // print only in manual mode
        Serial.print(F("D 0x"));
        PrintHex8(i);
        Serial.println();
        //Serial.println(F(" is present"));
     // }
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
/* Function to perform ACK polling on the given address
   addressForAckPolling: I2C address to poll
   returns 0 if ACK received or -12 if timeout occurs
*/
int ackPolling(int addressForAckPolling) {
    uint8_t nack_count = 0;
    while (1) {
      Wire.beginTransmission(addressForAckPolling);
      if (Wire.endTransmission() == 0) {
        return 0;
      }
      if (nack_count >= 1000)
      {
        Serial.println(F("Geez! Something went wrong while programming!"));
        return -12;
      }
      nack_count++;
      delay(1);
    }
}

////////////////////////////////////////////////////////////////////////////////
// power cycle 
////////////////////////////////////////////////////////////////////////////////
/* Function to power cycle the GreenPAK device
*/
void powercycle() {
  Serial.println(F("Power Cycling!")); // print only in manual mode
  digitalWrite(VDD, LOW);
  delay(500);
  digitalWrite(VDD, HIGH);
  // Serial.println(F("Done Power Cycling!"));
}
////////////////////////////////////////////////////////////////////////////////
// save to EEPROM
////////////////////////////////////////////////////////////////////////////////
/* Function to save data to Arduino EEPROM
   NVMorEEPROM: 'n' for NVM, 'e' for EEPROM
   data: pointer to data array to be saved
   rozmiar: size of data array
   returns true if OK or false if verification fails
*/
bool save_to_EEPROM(char NVMorEEPROM, uint8_t*data, size_t rozmiar) {
  boolean success=false;
  if(NVMorEEPROM == 'n') {
    if(selectionMode == 'm') Serial.println(F("Saving NVM data to EEPROM...")); // print only in manual mode
    // Copy NVM data to EEPROM data array
     offsetAddress = 0; //brak offsetu do zapisu programu do nvm
    for (size_t address = 0; address < rozmiar; address++) {
        EEPROM.update(address+ offsetAddress, data[address]);
      }
      success = true;
  }
  else if(NVMorEEPROM == 'e') {
    if(selectionMode == 'm') Serial.println(F("Saving EEPROM data to EEPROM...")); // print only in manual mode
    offsetAddress=256; // offset do zapisu programu do eeprom
    for (size_t address = 0; address < rozmiar; address++) {
    EEPROM.update(address+offsetAddress, data[address]);
    }
      success = true;
  }
  else{
    Serial.println(F("4 error przy save to eeprom"));
    Serial.println(F("ERROR! WRONG PARAMETER!"));
    success=false;
  }
//Verification
  for (size_t i = 0; i < rozmiar; ++i) {
    if (EEPROM.read(offsetAddress + i) != data[i]) {
      if (selectionMode == 'm') {
        Serial.print(F("VERIFY FAIL at address "));
        Serial.println(offsetAddress + i);
      }
      success = false;
      break;
    }
  }

  if (success) {
    if (selectionMode == 'm') Serial.println(F("Done Saving to EEPROM! (verified)"));
    return true;
  } else {
    return false;
  }
}
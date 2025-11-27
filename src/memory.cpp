#include "memory.h"
#include "automatic.h"
#include "globals.h"
#include "utils.h"
#include "menu.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
////////////////////////////////////////////////////////////////////////////////
// readChip 
////////////////////////////////////////////////////////////////////////////////
int readProgram(char NVMorEEPROM, uint8_t CheckOrRead, char GPAKorArdu, char ARDU_FLASHorEEPROM) {
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

  //for (uint8_t i = 0; i < 16; i++) {
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
            Serial.println(F("Incorrect data! Blad na idneksie:"));
            Serial.println(index); // debug
            Serial.print(F("Wartosc greenpaka: ")); // debug
            PrintHex8(gpakVal); // debug
            Serial.println(); // debug
            Serial.print(F("Wartosc z buffer_seria: ")); // debug
            PrintHex8(buffer_seria[index]); // debug
            index++;
            Serial.println();
            checkState=false;
            break;
            //return -1;
          }  
        }
        if (CheckOrRead == 16){ // read and print
          PrintHex8(Wire.read());
        }
      }
      if (CheckOrRead == 16) Serial.println();
    }
    if (CheckOrRead == 15){
      if(checkState){
        Serial.println(F("OK"));
        return 0;
      }
      else {
        Serial.println(F("E"));
        return -1;
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
}
////////////////////////////////////////////////////////////////////////////////
// eraseChip 
////////////////////////////////////////////////////////////////////////////////
int eraseChip(char NVMorEEPROM) {
  /*
  int control_code = slave_address << 3;
  int addressForAckPolling = control_code;
*/
  uint8_t control_code = slave_address << 3;
  uint8_t addressForAckPolling = control_code;

  for (uint8_t i = 0; i < 16; i++) {
    if(wybor == 'm'){
      Serial.print(F("Erasing page: 0x"));
      PrintHex8(i);
      Serial.print(F(" "));
    }
    Wire.beginTransmission(control_code);
    Wire.write(0xE3);

    if (NVMorEEPROM == 'n')
    {
      if (wybor == 'm') Serial.print(F("NVM ")); //print only in manual mode
      Wire.write(0x80 | i);
    }
    else if (NVMorEEPROM == 'e')
    {
      if (wybor == 'm') Serial.print(F("EEPROM ")); //print only in manual mode
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
      if(wybor == 'm') Serial.print(F("ready ")); // print only in manual mode
      delay(100);
    }
    if(wybor == 'm') Serial.println(); // print only in manual mode 
  }

  powercycle();
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// writeChip 
////////////////////////////////////////////////////////////////////////////////
//int writeChip(char NVMorEEPROM, char SERIALorMEM, char ARDU_FLASHorEEPROM) {
int writeChip(char NVMorEEPROM, char SERIALorMEM, char ARDU_FLASHorEEPROM, char updateSelection, uint8_t new_address) {
  uint8_t control_code = 0x00;
  uint8_t addressForAckPolling = 0x00;

  bool NVM_selected = false;
  bool EEPROM_selected = false;
  bool  data_from_SERIAL = false;
  bool  data_from_MEMORY = false;
  bool  data_from_ARDUINO_EEPROM = false;
  bool  data_from_ARDUINO_FLASH = false;

  if (NVMorEEPROM == 'n')
  {
    // Serial.println(F("Writing NVM")); // Display for user
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
    // Serial.println(F("Writing EEPROM with ADDRESS")); // Display for user
    // Serial.println(slave_address);
    control_code = slave_address << 3;
    control_code |= EEPROM_CONFIG;
    EEPROM_selected = true;
    addressForAckPolling = slave_address << 3;
  }
  else{
    Serial.println(F("ERROR! WRONG PARAMETER!"));
    return -1;
  }

  if(wybor == 'm'){
    Serial.print(F("Control Code: 0x")); // print only in manual mode
    PrintHex8(control_code);
    Serial.println();
  }
  if (SERIALorMEM == 's')
  {
    //if(wybor == 0) Serial.println(F("Getting data from SERIAL input")); // print only in manual mode
    Serial.println(F("Getting data from SERIAL input")); // print only in manual mode
    data_from_SERIAL = true;
    clearSerialBuffer();
  }
  else if (SERIALorMEM == 'm')
  {
    if(wybor == 'm') Serial.println(F("Getting data from MEMORY")); // print only in manual mode
    data_from_MEMORY = true;

    if (ARDU_FLASHorEEPROM == 'a'){
      //Serial.println(F("Using ARDUINO EEPROM MEMORY")); // Display for user
      data_from_ARDUINO_EEPROM = true;
    }
    else if (ARDU_FLASHorEEPROM == 'f'){
      //Serial.println(F("Using FLASH ARDUINO MEMORY")); // Display for user
      data_from_ARDUINO_FLASH = true;
    }
    else{
    Serial.println(F("ERROR! WRONG PARAMETER!"));
    return -1;
    }
  }
  else{
    Serial.println(F("ERROR! WRONG PARAMETER!"));
    return -1;
  }
  if (data_from_SERIAL)
  {
    uint16_t index = 0;
    char c=0;
    uint8_t zmienna1=0;
    uint8_t zmienna2=0;
    bool first = true;  // Flag to indicate if we are reading the first nibble
    while (index < 256) {
      if (Serial.available()) {
        c = Serial.read();
        if (c == ' ') {
          if (first) {
            continue;
          }
          else {
            // If we were in the middle of a byte, finalize it
            buffer_seria[index] = zmienna1>>4; // Only MSB but reconvert to LSB
            index++;
            first = true;
            continue;
          }
        }
        //avoid new lines signs
        if (c == '\n' || c == '\r') continue;

        // convert char to int
        int val = hexCharToInt(c);
        if (val == -1) {
           // Character is not a hex digit (e.g., tab, garbage). Ignore it.
           continue; 
        }
        // -------------------------
        if (first) {
          zmienna1 = val << 4;  // MSB
          first = false;
        } else {
           zmienna2 = val; // LSB
          buffer_seria[index] = zmienna1 | zmienna2; // Combine MSB and LSB
          index++;
          first = true;
        }
      }
    }
    CRC8fromSerial = calculateCRC8(buffer_seria, 256);
    Serial.print(F("CRC8: "));
    Serial.println(CRC8fromSerial, HEX);
    }
    clearSerialBuffer();
    // Serial.println(F("New NVM data:")); // Display for user
    if(data_from_MEMORY)
    {
      if(data_from_ARDUINO_FLASH)
      {
    for (size_t i = 0; i < 16; i++)
    {
      // Pull current page NVM from PROGMEM and place into buffer
      char buffer [33];
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
        int val= (hexCharToInt(buffer[2 * j])<<4)+ hexCharToInt(buffer[(2 * j) + 1]);
        buffer_seria[i * 16 + j] = val;
      }
    }
    }
    if (data_from_ARDUINO_EEPROM)
    {
      if (NVM_selected)
      {
        offsetAddress = 0;
      }
      else if (EEPROM_selected)
      {
        offsetAddress = 256;
      }
    for (size_t i = 0; i < 16; i++)
    {
      for (size_t j = 0; j < 16; j++)
      {
        size_t address = i * 16 + j;
        uint8_t value = EEPROM.read(address+offsetAddress);
        buffer_seria[i * 16 + j] = value;
      }
    }
    //Serial.println();
  }
    }
    if (NVM_selected)
    {
      while(1) {
        delay(10);
        clearSerialBuffer();
        int temp=0;
        if(wybor == 'm'){
        char newSA = query(7);
        if(newSA == 'x'){
          temp=slave_address;
          change_address = true;
        }
        else if(newSA == 'p') change_address=false;
        else if(newSA == 'q') return -1;
        else{
          change_address = true;
          temp = hexCharToInt(newSA);
        }
        if (newSA=='q'){
          Serial.println(F("Back to main menu"));
          return -1;
        } 
        }
        if(wybor == 'a'){
          temp = new_address;
        }
        if (temp < 0 || temp > 15)
        {
          if(wybor == 'm'){
            Serial.println(temp);
            Serial.println(F(" is not a valid slave address."));
          }
          continue;
      }
        else 
        {
          slave_address = temp;
          if(wybor == 'm'){ // print only in manual mode
            Serial.print(F("0x"));
            PrintHex8(slave_address);
            Serial.println();
          }
          break;
        }
      }

      if (change_address == true) buffer_seria[NVM_SLAVE_ADDR_OFFSET] = slave_address;
    }
    // Write each byte of data_array[][] array to the chip
    for (uint8_t i = 0; i < 16; i++) {
      Wire.beginTransmission(control_code);
      Wire.write(i << 4);
      if(wybor == 'm'){
        PrintHex8(i);
        Serial.print(F(" "));
      }
      for (uint8_t j = 0; j < 16; j++) {
          Wire.write(buffer_seria[i*16 + j]);
          if(wybor == 'm') PrintHex8(buffer_seria[i*16 + j]); // print only in manual mode
      }
      
      if (Wire.endTransmission() == 0) {
        if(wybor == 'm') Serial.print(F(" ack ")); // print only in manual mode
      } else {
        if(wybor == 'm'){ // print only in manual mode
          Serial.print(F(" nack\n"));
          Serial.println(F("Oh No! Something went wrong while programming!"));
        }
          return -1;
      }

      if (ackPolling(addressForAckPolling) == -1)
      {
        Serial.println(F("Oh No! Something went wrong with ack polling!"));
        return -1;
      } else {
        if(wybor == 'm') Serial.println(F("ready")); // print only in manual mode
        delay(100);
      }
    }

  if (updateSelection == 'u' ) {
    if (!(save_to_EEPROM(NVMorEEPROM, buffer_seria, sizeof(buffer_seria)))) {
    if (wybor=='m')  Serial.println(F("Error updating Arduino EEPROM!"));
      return -1;
    }
  }
  else if (updateSelection == 'i'){
    if (wybor=='m') Serial.println(F("Ignore updating to Arduino EEPROM"));
  }
  else return -1;
    powercycle();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// ping 
////////////////////////////////////////////////////////////////////////////////
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
  if(wybor == 'm') Serial.println(F("Power Cycling!")); // print only in manual mode
  digitalWrite(VDD, LOW);
  delay(500);
  digitalWrite(VDD, HIGH);
  // Serial.println(F("Done Power Cycling!"));
}
////////////////////////////////////////////////////////////////////////////////
// save to EEPROM
////////////////////////////////////////////////////////////////////////////////
bool save_to_EEPROM(char NVMorEEPROM, uint8_t*data, size_t rozmiar) {
  boolean success=false;
  if(NVMorEEPROM == 'n') {
    if(wybor == 'm') Serial.println(F("Saving NVM data to EEPROM...")); // print only in manual mode
    // Copy NVM data to EEPROM data array
     offsetAddress = 0; //brak offsetu do zapisu programu do nvm
    for (size_t address = 0; address < rozmiar; address++) {
        EEPROM.update(address+ offsetAddress, data[address]);
      }
      success = true;
  }
  else if(NVMorEEPROM == 'e') {
    if(wybor == 'm') Serial.println(F("Saving EEPROM data to EEPROM...")); // print only in manual mode
    offsetAddress=256; // offset do zapisu programu do eeprom
    for (size_t address = 0; address < rozmiar; address++) {
    EEPROM.update(address+offsetAddress, data[address]);
    }
      success = true;
  }
  else{
    Serial.println(F("ERROR! WRONG PARAMETER!"));
    success=false;
  }
  //if(success){
 //   if(wybor == 'm') Serial.println(F("Done Saving to EEPROM!")); // print only in manual mode
//  }
//  else Serial.println(F("Error Saving to EEPROM!"));
//Weryfikacja zapisu
  for (size_t i = 0; i < rozmiar; ++i) {
    if (EEPROM.read(offsetAddress + i) != data[i]) {
      if (wybor == 'm') {
        Serial.print(F("VERIFY FAIL at address "));
        Serial.println(offsetAddress + i);
      }
      success = false;
      break;
    }
  }

  if (success) {
    if (wybor == 'm') Serial.println(F("Done Saving to EEPROM! (verified)"));
    return true;
  } else {
    Serial.println(F("Error Saving to EEPROM!"));
    return false;
  }
}
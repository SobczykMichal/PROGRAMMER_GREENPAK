#include "automatic.h"
#include "globals.h"
#include "utils.h"
#include "memory.h"
#include "menu.h"
#include <Arduino.h>
int automatic_mode() {
  char command[8]= {0}; // 7 znaków + null terminator
  uint8_t index = 0;
  uint8_t new_int_addr=0;
  ///// Display for user, comment for final version to avoid unnecessary output
  //Serial.println(F("Enter full command e. g. w1nma2u"));
  //Serial.println(F("w- write, 1- slave address, n/e- NVM/EEPROM, m-MEMORY/SERIAL, a/f- ARDUINO EEPROM/FLASH, new address (0-F), u/i- update/ignore"));

  // Czekaj aż odbierzesz dokładnie 7 znaków lub nacisniesz przycisk
  while (index < 7) {

    // --- Obsługa przycisku z programowaniem NVM z eepromu arduino ---
    int reading = digitalRead(buttonPin);
    if (reading != lastStableState) {
      lastChangeTime = millis();
    }
    if ((millis() - lastChangeTime) > debounceDelay) {
      if (reading != currentState) {
        currentState = reading;
        if (currentState == LOW) {
          Serial.println("Przycisk wciśnięty -> tryb automatyczny");
          // Znajdź pierwszy dostępny adres
          for (uint8_t i = 0; i < 16; i++) {
            if (device_present[i]) {
              slave_address = i;
              break;
            }
          }
          eraseChip('n');
          change_address=false;
           return writeChip('n', 'm', 'a', 'i',0);
        }
      }
    }
    lastStableState = reading;
    // ------------------------------------------------------------

    if (Serial.available() > 0 || mySerial.available() > 0) {
      
      char c=0;
      if(Serial.available() > 0){
        c = Serial.read();
      }
      else if(mySerial.available() > 0){
        c = mySerial.read();
      }
      command[index] = c;
      index++;
      //Serial.println(command[index]); // Debug: print each received character
      //delay(5);
      if (c == '\n' || c == '\r') {
      index--; // Ignore new line characters
      }
      else{
        if(command[0]=='m'){
          wybor='m'; // go to manual mode
          clearSerialmySerialBuffer();
          return 0;
        }
        else if(command[0]=='e' && index ==3){
          index=7; // end reading after 3 characters
        } 
        else if (command[0] == 'r' && index == 5){
          index=7; // end reading after 5 characters
        }
        else if (command[0] != 'm' && command[0] != 'e' && command[0] != 'r' && command[0] != 'w'){
          Serial.println(F("ERROR! Wrong first command!"));
          index=0;
          clearSerialmySerialBuffer();
          return -1;
        }
      }
    }
  }
  clearSerialmySerialBuffer();
  // Parse command
  char cmd = command[0];
  char addr = command[1];
  char memType = command[2];
  char source = command[3];
  char storage = command[4];
  char newaddr = command[5];
  char updateEEPROM = command[6];

    // Debug: Display logs
  /*
  Serial.println(F("Odebrane znaki:"));
  Serial.print(F("cmd: ")); Serial.println(cmd);
  Serial.print(F("addr: ")); Serial.println(addr);
  Serial.print(F("memType: ")); Serial.println(memType);
  Serial.print(F("source: ")); Serial.println(source);
  Serial.print(F("storage: ")); Serial.println(storage);
  Serial.print(F("newaddr: ")); Serial.println(newaddr);
  Serial.print(F("updateEEPROM: ")); Serial.println(updateEEPROM);
  */
  //Check commands
  if ( (addr >= '0' && addr <= '9') ||   // 1. check if addr is a digit (between '0' and 'f')
     (addr >= 'a' && addr <= 'f') ||   
     (addr == 'x') ){ // 2. OR if it is letter 'x'
     }
     else{
      //Serial.println(F("ERROR! Wrong addres parameter!"));
      //mySerial.println(F("ERROR! Wrong addres parameter!"));
      return -2;
     }
  if(memType!= 'n' && memType!='e'){
    //Serial.println(F("ERROR! Wrong memory type parameter!"));
    //mySerial.println(F("ERROR! Wrong memory type parameter!"));
    return -3;
  }
  if (cmd == 'w')
  {
    if (source != 's' && source != 'm'){
      //Serial.println(F("ERROR! Wrong source parameter!"));
      //mySerial.println(F("ERROR! Wrong source parameter!"));
      return -4;
    }
    if (storage != 'a' && storage != 'f'){
      //Serial.println(F("ERROR! Wrong storage parameter!"));
      //mySerial.println(F("ERROR! Wrong storage parameter!"));
      return -5;
    }
  }
  if (cmd == 'r' ){
    if (source != 'g' && source != 'a'){
      //Serial.println(F("ERROR! Wrong source parameter!"));
      //mySerial.println(F("ERROR! Wrong source parameter!"));
      return -6;
    }
    if (storage != 'a' && storage != 'f'){
     // Serial.println(F("ERROR! Wrong storage parameter!"));
     // Serial.println(F("ERROR! If source is GreenPAK, storage parameter is ignored but write 'a' or 'f'."));
      //mySerial.println(F("ERROR! Wrong storage parameter!"));
      //mySerial.println(F("ERROR! If source is GreenPAK, storage parameter is ignored but write 'a' or 'f'."));
      return -7;
    }
  }
  if(cmd =='w'){
  if ( (newaddr >= '0' && newaddr <= '9') ||   // 1. check if newaddr is a digit (between '0' and 'f')
    (newaddr >= 'a' && newaddr <= 'f') ||   
    (newaddr == 'x' || 'p') ){ // 2. OR if it is letter 'x' or 'p' (no change)
    //correct command
  }
  else{
    //Serial.println(F("ERROR! Wrong new addres parameter!"));
   // mySerial.println(F("ERROR! Wrong new addres parameter!"));
    return -8;
  }
  if(updateEEPROM != 'u' && updateEEPROM != 'i'){
    //Serial.println(F("ERROR! Wrong update Eeprom parameter!"));
   // mySerial.println(F("ERROR! Wrong update Eeprom parameter!"));
    return -9;
  }
  }
  clearSerialmySerialBuffer();
  if(addr == 'x'){// check available address
    for (uint8_t i = 0; i < 16; i++) {
          if(device_present[i] == true) {
            slave_address = i;
            //Serial.print(slave_address); // debug
            break;
          }
    }
  } 
  else slave_address = hexCharToInt(addr);
  //Check for a valid slave address
  if (device_present[slave_address] == false)
  {
    //Serial.println(F("You entered an incorrect slave address. Submit slave address, 0-F: "));
   // mySerial.println(F("You entered an incorrect slave address. Submit slave address, 0-F: "));
    return -2;
  }
  else {
    //PrintHex8(slave_address); // debug
    //Serial.println(); // debug
  }
  if(cmd=='w'){
    if(newaddr == 'x'){
      new_int_addr = slave_address;
      change_address = true;
      //Serial.print(F("New slave address same like current slave address: ")); // Display for user, comment for final version to avoid unnecessary output
      //Serial.print(new_int_addr); 
    } 
    else if (newaddr == 'p'){
      //Serial.print(F("Brak zmiany adresu: ")); // Display for user, comment for final version to avoid unnecessary output
      change_address = false;
    } 
    else {
      change_address = true;
      new_int_addr = hexCharToInt(newaddr);
      //Serial.print(F("New slave address: "));  // Display for user, comment for final version to avoid unnecessary output
      //Serial.print(new_int_addr); 
    }
      if (eraseChip(memType) == 0) {
        Serial.println(F("Done erasing!"));
       // mySerial.println(F("Done erasing!"));
       
      } else {
        //Serial.println(F("Erasing did not complete correctly!"));
        //mySerial.println(F("Erasing did not complete correctly!"));
        return -10;
      }
      ping();
      writeChip(memType, source, storage, updateEEPROM, new_int_addr);
      return readProgram(memType, 15, 'g', storage);
  }
  else if (cmd =='r'){
    Serial.println(F("Reading chip!"));
    mySerial.println(F("Reading chip!"));
    return readProgram(memType, 16, source, storage);
  }
  else if (cmd =='e'){
    Serial.println(F("Erasing chip!"));
    mySerial.println(F("Erasing chip!"));
    return eraseChip(memType);
  }
  else{
    //Serial.println(F("ERROR! Wrong first command!"));
    //mySerial.println(F("ERROR! Wrong first command!"));
    return -13;
  }
}
#include "automatic.h"
#include "globals.h"
#include "utils.h"
#include "memory.h"
#include "menu.h"
#include <Arduino.h>
void automatic_mode() {
  char command[8]= {0}; // 7 znaków + null terminator
  uint8_t index = 0;
  uint8_t new_int_addr=0;
  //Serial.println(F("Enter full command e. g. w1nma2u"));
  //Serial.println(F("w- write, 1- slave address, n/e- NVM/EEPROM, m-MEMORY/SERIAL, a/f- ARDUINO EEPROM/FLASH, new address (0-F), u/i- update/ignore"));

  // Czekaj aż odbierzesz dokładnie 7 znaków
  while (index < 7) {
    if (Serial.available() > 0) {
      char c = Serial.read();
      command[index] = c;
      //Serial.println(command[index]); // Debug: print each received character
      //delay(5);
      index++;
      if (c == '\n' || c == '\r') {
      index--; // Ignore new line characters
      }            
      if(command[0]=='m'){
        wybor='m';
        clearSerialBuffer();
        return;
      }
      else if(command[0]=='e' && index ==3){
        index=7; // zakończ wczytywanie po 3 znakach
      } 
      else if (command[0] == 'r' && index == 5){
        index=7; // zakończ wczytywanie po 5 znakach
      }
    }

  }
  clearSerialBuffer();
  // Przypisanie do zmiennych
  char cmd = command[0];
  char addr = command[1];
  char memType = command[2];
  char source = command[3];
  char storage = command[4];
  char newaddr = command[5];
  char updateEEPROM = command[6];

    // Wyświetlenie
  /*
  Serial.println(F("Odebrane znaki:"));
  Serial.print(F("cmd: ")); Serial.println(cmd);
  Serial.print(F("addr: ")); Serial.println(addr);
  Serial.print(F("memType: ")); Serial.println(memType);
  Serial.print(F("source: ")); Serial.println(source);
  Serial.print(F("storage: ")); Serial.println(storage);
  Serial.print(F("newaddr: ")); Serial.println(newaddr);
  Serial.print(F("updateEEPROM: ")); Serial.println(updateEEPROM);*/
  //Check commands
  if (cmd != 'w' && cmd != 'e' && cmd != 'r'){
    Serial.println(F("ERROR! Wrong command!"));
    return;
  }
  if ( (addr >= '0' && addr <= '9') ||   // 1. Sprawdź, czy addr jest cyfrą (między '0' a 'f')
     (addr >= 'a' && addr <= 'f') ||   // 2. LUB czy jest literą 'x'
     (addr == 'x') ){
      //correct command
     }
     else{
      Serial.println(F("ERROR! Wrong addres parameter!"));
      return;
     }
  if(memType!= 'n' && memType!='e'){
    Serial.println(F("ERROR! Wrong memory type parameter!"));
    return;
  }
  if (cmd == 'w')
  {
    ///tutaj daj ze  tylko dla w i r czyli write i read
    if (source != 's' && source != 'm'){
      Serial.println(F("ERROR! Wrong source parameter!"));
      return;
    }
    if (storage != 'a' && storage != 'f'){
      Serial.println(F("ERROR! Wrong storage parameter!"));
      return;
    }
  }
  if (cmd == 'r' ){
    if (source != 'g' && source != 'a'){
      Serial.println(F("ERROR! Wrong source parameter!"));
      return;
    }
    if (storage != 'a' && storage != 'f'){
      Serial.println(F("ERROR! Wrong storage parameter!"));
      Serial.println(F("ERROR! If source is GreenPAK, storage parameter is ignored but write 'a' or 'f'."));
      return;
    }
  }
  if(cmd =='w'){
  if ( (newaddr >= '0' && newaddr <= '9') ||   // 1. Sprawdź, czy newaddr jest cyfrą (między '0' a 'f')
    (newaddr >= 'a' && newaddr <= 'f') ||   // 2. LUB czy jest literą 'x'
    (newaddr == 'x' || 'p') ){
    //correct command
  }
  else{
    Serial.println(F("ERROR! Wrong new addres parameter!"));
    return;
  }
  if(updateEEPROM != 'u' && updateEEPROM != 'i'){
    Serial.println(F("ERROR! Wrong update Eeprom parameter!"));
    return;
  }
  }
  clearSerialBuffer();
  // check address
  if(addr == 'x'){
    for (uint8_t i = 0; i < 16; i++) {
          if(device_present[i] == true) {
            slave_address = i;
            //Serial.print(slave_address);
            break;
          }
    }
  } 
  else slave_address = hexCharToInt(addr);
    //Check for a valid slave address
    if (device_present[slave_address] == false)
    {
      Serial.println(F("You entered an incorrect slave address. Submit slave address, 0-F: "));
      return;
    }
    else {
      //PrintHex8(slave_address); // pozniej zakomentowac 
      //Serial.println();
    }
    if(cmd=='w'){
  if(newaddr == 'x'){
    new_int_addr = slave_address;
    change_address = true;
    //Serial.print(F("New slave address same like current slave address: "));
    //Serial.print(new_int_addr);
  } 
  else if (newaddr == 'p'){
    //Serial.print(F("Brak zmiany adresu: "));
    change_address = false;
  } 
  else {
    change_address = true;
    new_int_addr = hexCharToInt(newaddr);
    //Serial.print(F("New slave address: "));  
    //Serial.print(new_int_addr);
  }
    if (eraseChip(memType) == 0) {
     Serial.println(F("Done erasing!"));
    } else {
      Serial.println(F("Erasing did not complete correctly!"));
    }
    ping();
    sprawdzenie=writeChip(memType, source, storage, updateEEPROM, new_int_addr); 
    if (sprawdzenie== 0) {
    //if (writeChip(memType, source, storage, updateEEPROM, new_int_addr)== 0) {
      // Serial.println(F("Done writing!"));
      Serial.println(F("OK"));
    } else {
      Serial.println(F("E"));
      //Serial.println(F("Writing did not complete correctly!"));
    }
    //Serial.println(sprawdzenie);
  }
  else if (cmd =='r'){
    Serial.println(F("Reading chip!"));
    sprawdzenie=readProgram(memType, 16, source, storage);
    // Serial.println(F("Done Reading!"));
  }
  else if (cmd =='e'){
    Serial.println(F("Erasing chip!"));
    sprawdzenie=eraseChip(memType);
  }
}
char select_mode(){
    while (1) {
    if (Serial.available() > 0) {
      char firstChar = Serial.read();
      if (firstChar == 'a' || firstChar == 'A') {
        //Serial.println(F("Automatic Write Mode Selected"));
        clearSerialBuffer();
        return 'a';
      }
      else if (firstChar == 'm' || firstChar == 'M') {
        Serial.println(F("Manual Mode Selected"));
        clearSerialBuffer();
        return 'm';
      }
      else {
        Serial.println(F("Invalid selection. You did not enter \"a\" or \"m\"."));
        clearSerialBuffer();
        continue;
      }
    }
  }
}
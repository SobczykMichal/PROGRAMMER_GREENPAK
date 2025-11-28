#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include "globals.h"
#include "menu.h"
#include "automatic.h"
#include "memory.h"
#include "utils.h"

////////////////////////////////////////////////////////////////////////////////
// setup 
////////////////////////////////////////////////////////////////////////////////
void setup() {
  mySerial.begin(9600);    // start komunikacji na SoftwareSerial
  Wire.begin(); // join i2c bus (address optional for master)
  Wire.setClock(400000);
  Serial.begin(115200);
  pinMode(VDD, OUTPUT);  // This will be the GreenPAK's VDD
  digitalWrite(VDD, HIGH);
  delay(100);
  Serial.println(F("Set mode: a = automatic writing, m = manual.First opening in automatic mode."));
  wybor='a'; //  default mode is automatic
  pinMode(buttonPin, INPUT_PULLUP); // aktywacja wewnętrznego pull-up
}

////////////////////////////////////////////////////////////////////////////////
// loop 
////////////////////////////////////////////////////////////////////////////////
void loop() {
  slave_address = 0x00;
  char NVMorEEPROM = 0;
  char SERIALorMEM = 0;
  char ARDU_FLASHorEEPROM = 0;
  char updateSelection = 0;
  char GPorARDU=0;
  clearSerialmySerialBuffer();
  if(wybor == 'a'){
    ping();
    lastOperationStatus= automatic_mode();
    StatusOperation(lastOperationStatus);
  }
  else
  {
  delay(20);
  clearSerialmySerialBuffer();
  char selection = query(1);

  switch (selection)
  {
    case 'r': 
        Serial.println(F("Reading chip!"));
        mySerial.println(F("Reading chip!")); // Informacja na SoftwareSerial
        if(requestSlaveAddress() =='q') break; 
        NVMorEEPROM = requestNVMorEeprom();// wykonuj to jesli request inny niz q
        if (NVMorEEPROM == 'q'){
          clearSerialmySerialBuffer();
          break;
        }
        clearSerialmySerialBuffer();
        GPorARDU = requestGPAKorArduino();
        if (GPorARDU == 'q'){
          clearSerialmySerialBuffer();
          break;
        }
        clearSerialmySerialBuffer();
        if(GPorARDU == 'a')ARDU_FLASHorEEPROM = requestARDU_EEPROMorFLASH();
        else ARDU_FLASHorEEPROM = 0; //jeśli GreenPAK to pomin
        if (ARDU_FLASHorEEPROM == 'q'){
          clearSerialmySerialBuffer();
          break;
        }
        clearSerialmySerialBuffer();
        readProgram(NVMorEEPROM, 16, GPorARDU, ARDU_FLASHorEEPROM);
        // Serial.println(F("Done Reading!"));
        break;
    case 'e': 
        Serial.println(F("Erasing Chip!"));
        if(requestSlaveAddress()== 'q'){
          break;
        }
        NVMorEEPROM = requestNVMorEeprom(); // wykonuj to jesli request inny niz q
        if (NVMorEEPROM == 'q'){
          clearSerialmySerialBuffer();
          break;
        }
        else{
          if (eraseChip(NVMorEEPROM) == 0) {
          // Serial.println(F("Done erasing!"));
          } 
          else {
            Serial.println(F("Erasing did not complete correctly!"));
          }
          delay(100);
          ping();
        }
        break;
    case 'w': 
        Serial.println(F("Writing Chip!"));
          if(requestSlaveAddress()=='q') break;;
          NVMorEEPROM = requestNVMorEeprom();
          clearSerialmySerialBuffer();
          if(NVMorEEPROM=='q') break;
          //SERIALorMEM = requestSERIALorMEM();
          //clearSerialmySerialBuffer();
          //if(SERIALorMEM=='q') break;
          if(requestSERIALorMEM()=='q') break;
          if(SERIALorMEM == 'm'){
            //ARDU_FLASHorEEPROM=requestARDU_EEPROMorFLASH();
            if(requestARDU_EEPROMorFLASH()=='q') break;
            clearSerialmySerialBuffer();
            updateSelection='i';
          }
          if(SERIALorMEM == 's'){
            updateSelection = requestUpdateEEPROM();
            clearSerialmySerialBuffer();
          }       
          if (eraseChip(NVMorEEPROM) == 0) {
            // Serial.println(F("Done erasing!"));
          } else {
            Serial.println(F("Erasing did not complete correctly!"));
          }

          ping();
          writeChip(NVMorEEPROM, SERIALorMEM, ARDU_FLASHorEEPROM, updateSelection, 0);
          //sprawdzenie=writeChip(NVMorEEPROM, SERIALorMEM, ARDU_FLASHorEEPROM, updateSelection, 0);
          //Serial.println(F("Sprawdzam wartosc writa"));
          //Serial.println(sprawdzenie);
          //if (sprawdzenie== 0) {
            // Serial.println(F("Done writing!"));
          //} else {
          ///  Serial.println(F("Writing did not complete correctly!"));
          //}

        ping();

        readProgram(NVMorEEPROM, 15, 'g', ARDU_FLASHorEEPROM);
        // Serial.println(F("Done Reading!"));
        break;
    case 'p': 
        Serial.println(F("Pinging!"));
        ping();
        // Serial.println(F("Done Pinging!"));
        break;
    case 'a':
        wybor='a';
        clearSerialmySerialBuffer();
        break;
    default:
        clearSerialmySerialBuffer();
        Serial.println(selection);
        Serial.println(F("Invalid selection. You did not enter \"r\", \"e\", \"w\", \"p\", \"a\"."));
        break;
  }
    StatusOperation(lastOperationStatus);
  }
}
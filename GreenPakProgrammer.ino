/*
  WAŻNE!!!! W ZAKŁADCE FILE->PREFERNCES ZMIEN SKETCHBOOK LOCATION (LOKALIZACJĘ SZKICOWNIKA) NA KATALOG W KTORYM UMIESZCZONY JEST TEN PROJEKT.
  !!! ZAPAMIĘTAJ LUB ZAPISZ POPRZEDNI ADRES ABY WRÓCIĆ DO POPRZEDNICH USTAWIEŃ PO SKOŃCZONYM PROGAMOWANIU !!!
  PRZY WYBORZE BOARDS (PŁYTKI) ROZWIŃ ZAKŁADKĘ BOARD->PROGRAMATORGEENPACK(IN SKETCHBOOK)->ARDUINO NANO(LUB WYBIERZ TĄ, KTÓRĄ CHCESZ ZAPROGRAMOWAĆ)
  JEST TO BARDZO ISTOTNE DLA POPRAWNEGO DZIAŁANIA PEŁNEJ FUNKCJONALNOŚCI PROGRAMATORA, PONIEWAŻ ZMIENIONY ZOSTAŁ ROZMIAR BUFFORA W KOMUNIKACJI UART
  !!! PO SKOŃCZENIU PROGRAMOWANIA WRÓC DO POPRZEDNIEJ LOKALIZACJI SKETCHBOOKA (SZKICOWNIKA). !!!
*/
/*
  Projekt: Porgoramator do Grennpacków
  Opis: Jest to projekt do obsługi GreenPaków posiada takie funkcje jak odczyt, zapis i wyczyszczenie pamięci GreenPacka.
        Poruszać się można w dwóch trybach: automatycznym- jedna komenda zawierająca wiele rozkazów lub przycisk do programowania pamięci NVM GreenPacka z pamięci Arduino EEPROM, manulanym- każda komenda jeden rozkaz.
        OPIS ROZKAZÓW:
        a-uruchom tryb automatyczny (domyślnie po uruchomieniu w trybie automatycznym).
        m- uruchom tryb manulany.
        r- read, odczytywanie pamięci.
        e- erase, czyszczenie pamięci.
        w - write, zapis danych do pamięci.
        p- ping, sprawdzenie aktywnych adresów greenpaków (tylko w trybie manualnym).
        <0,f>U{x} - wpisywanie adresu greenpacka z którym chcemy się skomunikować. x- pierwszy adres automatycznie odczytany.
        <0,f>U{x,p} - wpisywanie nowego adresu greenpacka po zapisie pamięci NVM, x-pozostawienie automatycznie odczytanego adresu, p-pozostawienie adresu ustawionego z wpisywanego programu.
        n- wybor pamieci NVM greenpacka- non volatile memory (pamięć do wgrania programu greenpacka).
        e- wybor pamieci EEPROM greeenpacka.
        m- memory, wybor zrodla danych do zapisu pamięci GreenPacka, pamięć Arduino.
        s- serial, wybor zrodla danych do zapisu pamięci GreenPacka, zapis danych z serial monitora (komunikacja UART).
        f- Arduino FLASH, dalszy wybór źródła danych z pamięci Arduino.
        a- Arduino EEPROM, dalszy wybór źródła danych z pamięci Arduino ('a' jeszcze jedno znaczenie w trybie read).
        i- ignore, zignoruj zapis nowo wprowadzonego programu z serial monitora do EEPROMu Arduino.
        u- update, uaktualnij zapis nowo wprowadzonego programu z serial monitora do EEPROMu Arduino.
        g- Greenpack, wybór Greenpacka, urządzenia, z którego chcesz odczytać pamięć.
        a- Arduino, wybór Arduino, urządzenia, z którego chcesz odczytać pamięć.
        Przykłady komend w trybie automatucznym:
        wxnmfxi - write, x-pierwszy odczytany adres greenpacka, n-zapis do NVM, m-dane z pamieci arduino, f-dane z pamiec flash, x-pozostaw pierwszy odczytany adres greenpacka po programowaniu, i- zignoruj updateeeprom Arduino
        exn- erase, x-pierwszy odczytany adres greenpacka, n- wyczyscp pamiec NVM
        r1nga- read, 1-odczyt greenpacka o Adresie 0x01, n- odczyt pamięci, g-odczyt z Greenpacka
        WAŻNE: w trybie autmatycznym komenda zapisu musi skladac sie z 7 znaków, czyszczenia z 3, odczytu z 5.
               Jeśli komenda nie będzie korzystała z wszystkich rozkazów i tak trzeba wpisać tyle znaków do poprawnego formatu komendy.
               ZAPIS:[w][addr/x][n/e][m/s][f/a][new addr/x/p][i/u]
               ODCZYT:[r][addr/x][n/e][a/g][f/a]
               CZYSZCZENIE: [e][addr/x][n/e]
*/
#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include "globals.h"
#include "menu.h"
#include "automatic.h"
#include "memory.h"
#include "utils.h"
//#include "HardwareSerial.h"
////////////////////////////////////////////////////////////////////////////////
// setup 
////////////////////////////////////////////////////////////////////////////////
void setup() {
  mySerial.begin(9600);  //SoftwareSerial initialization
  Wire.begin(); 
  Wire.setClock(400000); // Ustawienie prędkości I2C na 400kHz
  Serial.begin(115200);
  pinMode(VDD, OUTPUT);  // This will be the GreenPAK's VDD
  digitalWrite(VDD, HIGH);
  pinMode(buttonPin, INPUT_PULLUP); //button pin initialization
  delay(100);
  Serial.println(F("Set mode: a = automatic writing, m = manual.First opening in automatic mode."));
  selectionMode='a'; //  default mode is automatic
  Serial.println(SERIAL_RX_BUFFER_SIZE);
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
  if(selectionMode == 'a'){ //automatic mode
    ping();
    lastOperationStatus= automatic_mode();
    StatusOperation(lastOperationStatus);
  }
  else // manual mode
  {
  delay(20);
  clearSerialmySerialBuffer();
  char selection = query(1);

  switch (selection)
  {
    case 'r': 
        Serial.println(F("Reading chip!")); // Display for user
        mySerial.println(F("Reading chip!")); 
        if(requestSlaveAddress() =='q') break; 
        NVMorEEPROM = requestNVMorEeprom();
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
        lastOperationStatus = readProgram(NVMorEEPROM, 16, GPorARDU, ARDU_FLASHorEEPROM);
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
          lastOperationStatus = eraseChip(NVMorEEPROM);
          if (lastOperationStatus == 0) {
          // Serial.println(F("Done erasing!")); // Display for user
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
          SERIALorMEM = requestSERIALorMEM();
          clearSerialmySerialBuffer();
          if(SERIALorMEM == 'q') break;       
          if(SERIALorMEM == 'm'){
            ARDU_FLASHorEEPROM=requestARDU_EEPROMorFLASH();
            if(ARDU_FLASHorEEPROM=='q') break;
            clearSerialmySerialBuffer();
            updateSelection='i';
          }
          if(SERIALorMEM == 's'){
            updateSelection = requestUpdateEEPROM();
            clearSerialmySerialBuffer();
          }
          lastOperationStatus = eraseChip(NVMorEEPROM);
          if (lastOperationStatus == 0) {
            // Serial.println(F("Done erasing!"));
          } else {
            Serial.println(F("Erasing did not complete correctly!"));
          }
        ping();
        lastOperationStatus =  writeChip(NVMorEEPROM, SERIALorMEM, ARDU_FLASHorEEPROM, updateSelection, 0);
        ping();
        lastOperationStatus = readProgram(NVMorEEPROM, 15, 'g', ARDU_FLASHorEEPROM);
        // Serial.println(F("Done Reading!")); // Display for user
        break;
    case 'p': 
        Serial.println(F("Pinging!"));
        ping();
        // Serial.println(F("Done Pinging!")); // Display for user
        break;
    case 'a':
        selectionMode='a';
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
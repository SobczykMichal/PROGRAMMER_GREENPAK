#include <Arduino.h>
#include <Wire.h>
#include <stdlib.h>
#include <string.h>
#include <EEPROM.h>

#define NVM_CONFIG 0x02
#define EEPROM_CONFIG 0x03
#define VDD 2

#define NVM_SLAVE_ADDR_PAGE 12
#define NVM_SLAVE_ADDR_BYTE 10
#define NVM_SLAVE_ADDR_OFFSET (NVM_SLAVE_ADDR_PAGE * 16 + NVM_SLAVE_ADDR_BYTE)

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
int slave_address = 0x00;
bool device_present[16] = {false};
char wybor=0;
uint8_t buffer_seria[256];  // Bufor na 256 znaków hex (czyli 128 bajtów)
bool change_address = false;
int sprawdzenie=0;

char select_mode();
void automatic_mode();
char requestSlaveAddress();
char requestNVMorEeprom();
char requestSERIALorMEM();
char requestGPAKorArduino();
char query(uint8_t which_menu);
void PrintHex8(uint8_t data);
int readProgram(char NVMorEEPROM, uint8_t CheckOrRead, char GPAKorArdu, char ARDU_FLASHorEEPROM);
int eraseChip(char NVMorEEPROM);
int writeChip(char NVMorEEPROM, char SERIALorMEM, char ARDU_FLASHorEEPROM, char updateSelection, uint8_t new_address);
void ping();
int ackPolling(int addressForAckPolling);
void powercycle();
int hexCharToInt(char hexChar);
bool save_to_EEPROM(char NVMorEEPROM, uint8_t*data, size_t rozmiar);
char requestUpdateEEPROM();
char requestARDU_EEPROMorFLASH();
void clearSerialBuffer();
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
  //Serial.println(F("DEBUGOWANIE"));
  Serial.println(F("Set mode: a = automatic writing, m = manual"));
  wybor=select_mode();
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
  clearSerialBuffer();
  if(wybor == 'a'){
    ping();
    automatic_mode();
  }
  else
  {
  char selection = query(1);

  switch (selection)
  {
    case 'r': 
        Serial.println(F("Reading chip!"));
        if(requestSlaveAddress() =='q') break; 
        NVMorEEPROM = requestNVMorEeprom();// wykonuj to jesli request inny niz q
        if (NVMorEEPROM == 'q'){
          clearSerialBuffer();
          break;
        }
        clearSerialBuffer();
        GPorARDU = requestGPAKorArduino();
        if (GPorARDU == 'q'){
          clearSerialBuffer();
          break;
        }
        clearSerialBuffer();
        if(GPorARDU == 'a')ARDU_FLASHorEEPROM = requestARDU_EEPROMorFLASH();
        else ARDU_FLASHorEEPROM = 0; //jeśli GreenPAK to pomin
        if (ARDU_FLASHorEEPROM == 'q'){
          clearSerialBuffer();
          break;
        }
        clearSerialBuffer();
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
          clearSerialBuffer();
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
          clearSerialBuffer();
          if(NVMorEEPROM=='q') break;
          SERIALorMEM = requestSERIALorMEM();
          clearSerialBuffer();
          if(SERIALorMEM=='q') break;
          if(SERIALorMEM == 'm'){
            ARDU_FLASHorEEPROM=requestARDU_EEPROMorFLASH();
            clearSerialBuffer();
            updateSelection='i';
          }
          if(SERIALorMEM == 's'){
            updateSelection = requestUpdateEEPROM();
            clearSerialBuffer();
          }       
          if (eraseChip(NVMorEEPROM) == 0) {
            // Serial.println(F("Done erasing!"));
          } else {
            Serial.println(F("Erasing did not complete correctly!"));
          }

          ping();
          sprawdzenie=writeChip(NVMorEEPROM, SERIALorMEM, ARDU_FLASHorEEPROM, updateSelection, 0);
          Serial.println(F("Sprawdzam wartosc writa"));
          Serial.println(sprawdzenie);
          if (sprawdzenie== 0) {
            // Serial.println(F("Done writing!"));
          } else {
            Serial.println(F("Writing did not complete correctly!"));
          }

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
        clearSerialBuffer();
        break;
    default:
        break;
  }
  }
}
////////////////////////////////////////////////////////////////////////////////
// hex char to int
////////////////////////////////////////////////////////////////////////////////
int hexCharToInt(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return -1;
}
////////////////////////////////////////////////////////////////////////////////
// request slave address 
////////////////////////////////////////////////////////////////////////////////
char requestSlaveAddress() {
  ping();

  while(1) {
    clearSerialBuffer();
    char mySlaveAddress = query(2);
    if (mySlaveAddress=='q'){
      clearSerialBuffer();
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
    clearSerialBuffer();
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
          clearSerialBuffer();
          return 'q';
      default:
          Serial.println(F("Invalid selection. You did not enter \"n\" or \"e\"."));
          continue;
    }
  }
}
char requestGPAKorArduino(){
  while(1){
    clearSerialBuffer();
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
        clearSerialBuffer();
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
        clearSerialBuffer();
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
    clearSerialBuffer();
    char selection = query(4);
    switch (selection)
    {
      case 's':
          clearSerialBuffer();
          Serial.println(F("SERIAL"));
          return 's';
      case 'm':
          clearSerialBuffer();
          Serial.println(F("MEMORY"));
          return 'm';
      case 'q' :
          clearSerialBuffer();
          Serial.println(F("Back to main menu"));
          return 'q';
          
      default:
          clearSerialBuffer();
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
    clearSerialBuffer();
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
  Serial.println(F("\nMENU: r = read, e = erase, w = write, p = ping"));
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
////////////////////////////////////////////////////////////////////////////////
// tryb automatyczny 
////////////////////////////////////////////////////////////////////////////////
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
      index++;
      if(command[0]=='m'){
        wybor='m';
        clearSerialBuffer();
        return;
      }
      else if(command[0]=='e' && index ==3){
        index=7; // zakończ wczytywanie po 3 znakach
        //c=Serial.read();
        //command[index]=c;
        //index++;
        //c=Serial.read();
        //command[index]=c;
        //index=7; // zakończ wczytywanie po 3 znakach
      } 
      else if (command[0] == 'r' && index == 5){
        index=7; // zakończ wczytywanie po 5 znakach
      }
    if (c == '\n' || c == '\r') {
      index--; // Ignore new line characters
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
    Serial.println(sprawdzenie);
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
int readProgram(char NVMorEEPROM, uint8_t CheckOrRead, char GPAKorArdu, char ARDU_FLASHorEEPROM) {
  int control_code = slave_address << 3;
  int index = 0;
  if (NVMorEEPROM == 'n')
  {
    control_code |= NVM_CONFIG;
  }
  else if (NVMorEEPROM == 'e')
  {
    control_code |= EEPROM_CONFIG;
  }

  //for (uint8_t i = 0; i < 16; i++) {
  if((NVMorEEPROM == 'n' || NVMorEEPROM == 'e') && GPAKorArdu == 'g'){
  for (uint8_t i = 0; i < CheckOrRead; i++) {
    Wire.beginTransmission(control_code);
    Wire.write(i << 4);
    Wire.endTransmission(false);
    delay(10);
    Wire.requestFrom(control_code, 16);

    while (Wire.available()) {
      if(CheckOrRead == 15){
        if (buffer_seria[index]== Wire.read()) index++;
        else{
          Serial.println(F("Incorrect data!"));
          return -1;
        }  
      }
      if (CheckOrRead == 16){
        PrintHex8(Wire.read());
      }
    }
    if (CheckOrRead == 16) Serial.println();
  }
  if (CheckOrRead == 15) Serial.println(F("OK"));
  return 0;
  }
  else if(GPAKorArdu == 'a'){
    if (ARDU_FLASHorEEPROM== 'a'){
      for (size_t i = 0; i < 16; i++)
      {
        for (size_t j = 0; j < 16; j++)
        {
          size_t address = i * 16 + j;
          uint8_t value = EEPROM.read(address);
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
  /*
  int control_code = 0x00;
  int addressForAckPolling = 0x00;
  */
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
     Serial.println(F("Writing EEPROM with ADDRESS"));
     Serial.println(slave_address);
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
      //Serial.println(F("Using ARDUINO EEPROM MEMORY"));
      data_from_ARDUINO_EEPROM = true;
    }
    else if (ARDU_FLASHorEEPROM == 'f'){
      //Serial.println(F("Using FLASH ARDUINO MEMORY"));
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
    //Serial.println(F("PETLA if data from serial")); debugowanie
    
    uint16_t index = 0;
    char c=0;
    uint8_t zmienna1=0;
    uint8_t zmienna2=0;
    bool first = true;  // Flaga do śledzenia, czy aktualny znak jest parzysty czy nieparzysty
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
            //Serial.print(F("PRZYPISANE DO BUFFERA (SPACE): "));// debugowanie
            //Serial.println(buffer_seria[index], HEX);// debugowanie
            index++;
            first = true;
            continue;
          }
        }
        //avoid new lines signs
        if (c == '\n' || c == '\r') continue;

        // convert char to int
        int val = hexCharToInt(c);

        if (first) {
          zmienna1 = val << 4;  // MSB
          first = false;
        } else {
           zmienna2 = val; // LSB
          buffer_seria[index] = zmienna1 | zmienna2; // Combine MSB and LSB
          //Serial.print(F("PRZYPISANE DO BUFFERA: "));// debugowanie
          //Serial.println(buffer_seria[index], HEX);// debugowanie
          index++;
          first = true;
        }
      }
    }
    }
    clearSerialBuffer();
    // Serial.println(F("New NVM data:"));
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
    }
    if (NVM_selected) /// ZMIANA 
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
        //temp = hexCharToInt(newSA);
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
  uint16_t base=0; // base address in EEPROM 
  if(NVMorEEPROM == 'n') {
    if(wybor == 'm') Serial.println(F("Saving NVM data to EEPROM...")); // print only in manual mode
    // Copy NVM data to EEPROM data array
    base=0;
    for (size_t address = 0; address < rozmiar; address++) {
        EEPROM.update(address+ base, data[address]);
      }
      success = true;
  }
  else if(NVMorEEPROM == 'e') {
    if(wybor == 'm') Serial.println(F("Saving EEPROM data to EEPROM...")); // print only in manual mode
    base=rozmiar;
    for (size_t address = 0; address < rozmiar; address++) {
    EEPROM.update(address+base, data[address]);
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
    if (EEPROM.read(base + i) != data[i]) {
      if (wybor == 'm') {
        Serial.print(F("VERIFY FAIL at address "));
        Serial.println(base + i);
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

////////////////////////////////////////////////////////////////////////////////
// Clear Serial Buffer
//////////////////////////////////////////////////////////////////////////////
void clearSerialBuffer() {
  while (Serial.available()>0) {
    Serial.read();
  }
}
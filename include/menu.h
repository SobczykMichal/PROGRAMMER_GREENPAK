#pragma once
#include <Arduino.h>
enum class MemoryScope {
    NonVolatileOnly,  // Tylko NVM i EEPROM
    AllIncludingRam   // NVM, EEPROM oraz RAM
};
char requestSlaveAddress();
int8_t requestNewSlaveAddress();
char requestNVMorEEPROMorRAM(MemoryScope scope);
char requestSERIALorMEM();
char requestUpdateEEPROM();
char requestARDU_EEPROMorFLASH();
char requestGPAKorArduino();
char query(uint8_t which_menu);
void StatusOperation(int8_t status);
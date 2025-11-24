#pragma once
#include <Arduino.h>
char requestSlaveAddress();
char requestNVMorEeprom();
char requestSERIALorMEM();
char requestUpdateEEPROM();
char requestARDU_EEPROMorFLASH();
char requestGPAKorArduino();
char query(uint8_t which_menu);
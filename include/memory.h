#pragma once
#include <Arduino.h>
int readProgram(char NVMorEEPROM, uint8_t CheckOrRead, char GPAKorArdu, char ARDU_FLASHorEEPROM);
int eraseChip(char NVMorEEPROM);
int writeChip(char NVMorEEPROM, char SERIALorMEM, char ARDU_FLASHorEEPROM, char updateSelection, uint8_t new_address);
void ping();
int ackPolling(int addressForAckPolling);
void powercycle();
bool save_to_EEPROM(char NVMorEEPROM, uint8_t*data, size_t rozmiar);
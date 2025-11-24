#pragma once
#include <Arduino.h>

uint8_t calculateCRC8(uint8_t *data, size_t length);
int hexCharToInt(char c);
void PrintHex8(uint8_t data);
void clearSerialBuffer();
////////////////////////////////////////////////////////////////////////////////
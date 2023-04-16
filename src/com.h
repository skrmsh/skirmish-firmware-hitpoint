/*
Skirmish ESP32 Firmware

ESPNOW Communication driver

Copyright (C) 2023 Ole Lange
*/

#pragma once
#include <stdint.h>

void comInit(void (*cb)(const uint8_t *data));

void comGotHit(uint8_t hpMode, uint8_t pid, uint16_t sid);
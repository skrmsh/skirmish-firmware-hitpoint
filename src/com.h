/*
Skirmish ESP32 Firmware

ESPNOW Communication driver

Copyright (C) 2023 Ole Lange
*/

#pragma once
#include <stdint.h>

#define CMD_SYS_INIT 0x01
#define CMD_HIT_VALID 0x02
#define CMD_GOT_HIT 0x03

void comInit(void (*cb)(const uint8_t *data));

void comGotHit(uint8_t hpMode, uint8_t pid, uint16_t sid);
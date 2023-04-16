/*
Skirmish ESP32 Firmware

Hitpoint driver - Header file

Copyright (C) 2023 Ole Lange
*/

#pragma once

#include <Arduino.h>

// Hitpoint Animations
#define HP_ANIM_SOLID 0
#define HP_ANIM_BLINK 1
#define HP_ANIM_ROTATE 2
#define HP_ANIM_BREATHE 3

// I2C Commands
#define HP_CMD_SELECT_ANIM 0x01
#define HP_CMD_SET_ANIM_SPEED 0x02
#define HP_CMD_SET_COLOR 0x03

void hitpointInit();

bool hitpointEventTriggered();
uint32_t hitpointReadShotRaw(uint8_t addr);

void hitpointSelectAnimation(uint8_t addr, uint8_t animation);
void hitpointSetAnimationSpeed(uint8_t addr, uint8_t speed);
void hitpointSetColor(uint8_t addr, uint8_t r, uint8_t g, uint8_t b);

uint8_t getPIDFromShot(uint32_t shot);
uint16_t getSIDFromShot(uint32_t shot);
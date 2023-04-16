#include <Arduino.h>
#include <conf.h>
#include <hitpoint.h>
#include <log.h>

void setup() {
    logInit();
    logInfo("Logging Initialized, welcome!");
    hitpointInit();

    // Breathing to indicate SYSSTATE_UNINIT
    hitpointSetColor(HP_ADDR, 233, 30, 99);
    hitpointSelectAnimation(HP_ADDR, HP_ANIM_BREATHE);
    hitpointSetAnimationSpeed(HP_ADDR, 2);
}

void loop() {
    // put your main code here, to run repeatedly:
}
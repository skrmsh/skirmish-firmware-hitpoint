#include <Arduino.h>
#include <conf.h>
#include <hitpoint.h>
#include <log.h>

void setup() {
    logInit();
    logInfo("Logging Initialized, welcome!");
    hitpointInit();

    hitpointSetColor(HP_ADDR, 255, 0, 255);
    hitpointSelectAnimation(HP_ADDR, HP_ANIM_BLINK);
    hitpointSetAnimationSpeed(HP_ADDR, 75);
}

void loop() {
    // put your main code here, to run repeatedly:
}
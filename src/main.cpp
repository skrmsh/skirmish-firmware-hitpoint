#include <Arduino.h>
#include <com.h>
#include <conf.h>
#include <hitpoint.h>
#include <log.h>

#define SYSSTATE_UNINIT 0
#define SYSSTATE_IDLE 1
#define SYSSTATE_COOLDOWN 2

uint8_t currentState = SYSSTATE_UNINIT;
uint8_t hitpointMode = 0b000; // TODO: Read this from IO on init

uint32_t cooldownUntil = 0;
uint8_t color_r = 0;
uint8_t color_g = 0;
uint8_t color_b = 0;

uint32_t recentHits[64];
uint8_t recentHitsIdx = 0;

uint32_t shotSignature(uint8_t pid, uint16_t sid) {
    uint32_t pack = (pid << 16) | (sid & 0xffff);
    uint8_t checksum = pack % 0xff;
    return (checksum << 24) | pack;
}

void handleComData(const uint8_t *data) {
    uint8_t cmd = data[0];
    uint8_t mode = data[1];
    if (mode != hitpointMode) {
        return; // Not for me
    }
    if (cmd == CMD_SYS_INIT) {
        logInfo("CMD: SYS_INIT");
        currentState = SYSSTATE_IDLE;
        color_r = data[2];
        color_g = data[3];
        color_b = data[4];
        hitpointSetColor(HP_ADDR, color_r, color_g, color_b);
        hitpointSelectAnimation(HP_ADDR, HP_ANIM_SOLID);
        hitpointSetAnimationSpeed(HP_ADDR, 1);
        logDebug("-> Color: #%02x%02x%02x", color_r, color_g, color_b);
    } else if (cmd == CMD_HIT_VALID) {
        // Check if the hit was at this hitpoint
        logInfo("CMD: HIT_VALID");
        uint8_t pid = data[2];
        uint16_t sid = data[3] | (data[4] << 8);
        uint32_t shot = shotSignature(pid, sid);
        for (uint8_t i = 0; i < (sizeof(recentHits) / sizeof(recentHits[0])); i++) {
            // shot in recent hits
            if (recentHits[i] == shot) {
                logDebug("-> Was recently hit by this show!");
                cooldownUntil = millis() + (data[5] * 1000);
                currentState = SYSSTATE_COOLDOWN;

                hitpointSetColor(HP_ADDR, 255, 255, 255);
                hitpointSelectAnimation(HP_ADDR, HP_ANIM_BLINK);
                hitpointSetAnimationSpeed(HP_ADDR, 15);

                logDebug("-> Cooling down for %d seconds.", data[5]);
                break;
            }
        }
    }
}

void setup() {
    logInit();
    logInfo("Logging Initialized, welcome!");
    hitpointInit();

    // Breathing to indicate SYSSTATE_UNINIT
    hitpointSetColor(HP_ADDR, 233, 30, 99);
    hitpointSelectAnimation(HP_ADDR, HP_ANIM_BREATHE);
    hitpointSetAnimationSpeed(HP_ADDR, 2);

    // Clearing recent hits
    memset(&recentHits, 0, sizeof(recentHits));

    comInit(&handleComData);
}

bool hitpointEvent = false;
uint32_t now = 0;
uint32_t lastReceivedShot = 0;

void loop() {
    hitpointEvent = hitpointEventTriggered();
    now = millis();

    if (currentState == SYSSTATE_IDLE) {
        if (hitpointEvent) {
            lastReceivedShot = hitpointReadShotRaw(HP_ADDR);
            recentHits[recentHitsIdx] = lastReceivedShot;
            recentHitsIdx = (recentHitsIdx + 1) % (sizeof(recentHits) / sizeof(recentHits[0]));

            comGotHit(hitpointMode, getPIDFromShot(lastReceivedShot), getSIDFromShot(lastReceivedShot));
        }
    } else if (currentState == SYSSTATE_COOLDOWN) {
        // Reset after cooldown
        if (now > cooldownUntil) {
            currentState = SYSSTATE_IDLE;
            hitpointSetColor(HP_ADDR, color_r, color_g, color_b);
            hitpointSelectAnimation(HP_ADDR, HP_ANIM_SOLID);
            hitpointSetAnimationSpeed(HP_ADDR, 1);
        }
    }
}
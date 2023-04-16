/*
Skirmish ESP32 Firmware

ESPNOW Communication driver

Copyright (C) 2023 Ole Lange
*/

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include <com.h>
#include <conf.h>
#include <hitpoint.h>
#include <log.h>

esp_now_peer_info_t broadcastReceiver;
void (*handlerCallback)(const uint8_t *data);

void initESPNow(bool isRetry) {
    if (esp_now_init() == ESP_OK) {
        logInfo("-> ESPNOW Initialised!");
    } else {
        if (!isRetry) {
            logError("-> ESPNOW Init failed! retrying in 1 sec...");
            delay(1000);
            initESPNow(true);
        } else {
            logFatal("-> ESPNOW Init failed! Halting system!");
            hitpointSetColor(HP_ADDR, 255, 0, 0);
            hitpointSelectAnimation(HP_ADDR, HP_ANIM_BLINK);
            hitpointSetAnimationSpeed(HP_ADDR, 50);
            while (1) {
                delay(1000);
            }
        }
    }
}

void onReceive(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
    if (data_len == 6) {
        char macStr[18];
        snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        char dataStr[18];
        snprintf(dataStr, sizeof(dataStr), "%02x,%02x,%02x,%02x,%02x,%02x",
                 data[0], data[1], data[2], data[3], data[4], data[5]);

        logDebug("Received %s from %s", dataStr, macStr);

        handlerCallback(data);
    }
}

void comGotHit(uint8_t hpMode, uint8_t pid, uint16_t sid) {
    const uint8_t peer_addr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    uint8_t data[6] = {
        0x03,
        hpMode,
        pid,
        sid & 0xff,
        (sid >> 8) & 0xff,
        0};
    esp_err_t result = esp_now_send(peer_addr, data, sizeof(data));
    logDebug("Send CMD GOT HIT with result: %d (%d -> OK)", result, ESP_OK);
}

void comInit(void (*cb)(const uint8_t *data)) {
    logInfo("Init: ESPNOW Communication");
    WiFi.mode(WIFI_STA);
    logDebug("-> STA MAC: %s", WiFi.macAddress().c_str());
    esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
    logDebug("-> WiFi Channel set to %d", ESPNOW_CHANNEL);
    initESPNow(false);
    logDebug("-> ESP NOW Init done");

    handlerCallback = cb;
    esp_now_register_recv_cb(onReceive);
    logDebug("-> Recv CB set");

    memset(&broadcastReceiver, 0, sizeof(broadcastReceiver));
    for (int i = 0; i < 6; i++) {
        broadcastReceiver.peer_addr[i] = 0xff;
    }
    broadcastReceiver.channel = ESPNOW_CHANNEL;
    broadcastReceiver.encrypt = 0;

    esp_now_add_peer(&broadcastReceiver);
    logDebug("-> Peer added");
}
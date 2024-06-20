#include "BleCombo.h"
#include <ArduinoJson.h>
#include <string>

/*
Nom du fichier : BLEKeyboard.h
Auteur : Yann Cesbron  (yann@cesbron.org)
Date de création : 4 avril 2023
Licence :  Creative Commons BY-NC-SA
*/

#define MOUSE_MIN_SPEED_DELAY 5 // ms

void initBLEKeyboard();
void setBLEConfig();
void sendKeyToBLE(String key, int type, int keyState);
void changeMacAddress();

TaskHandle_t mouseMovexHandle = NULL;
SemaphoreHandle_t xTaskSemaphore;
int semaphoreDelay = 30000;   // 30 secondes

void mouseMove(uint8_t, uint8_t);
void mouseMoveKillTask();

    struct xParams // passés à la xtask   freertos pour mouse move
{
    uint8_t dir;
    uint8_t pts;
};

std::string extern getBLEdeviceName();   // dans sdcard
std::string extern getBLEmanufacturer(); // dans sdcard
int extern getVbat();                    // dans misc
void extern changeMacAddress();          // dans misc

unsigned long startTimeM;
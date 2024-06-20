/*
Nom du fichier : misc.h
Auteur : Yann Cesbron  (yann@cesbron.org)
Date de création : 4 avril 2023
Licence :  Creative Commons BY-NC-SA
*/

#ifndef MISC_H
#define MISC_H
#include <Arduino.h>
#include <Ticker.h>
#include <ArduinoJson.h>
#include "gpio.h"
#include "esp_system.h"
#include "esp_bt_defs.h"
#include "esp_bt.h"


#define BLINK_TIME 1000               // 1 s
#define POWER_OFF_TIMEOUT 900000      // 15 mn soit 900000
#define DISPLAY_OFF_TIMEOUT 30000     // 30 s
#define DISPLAY_DEFAULT_TIMEOUT 2000 // 2 s

// ------------ fonctions
void reset_all();
void MemoryInfos();
int getVbat();
void initTimers();
void handleTimers();
void blink();
void startDisplayOffTimer();
void startDisplayDefaultTimer();
void PowerOffTimer(bool);
void extern powerOff();                   // dans power
void extern displayRadio(bool);           // dans display
void extern displayDefault();             // dans display
void extern displayOff();                 // dans display
void extern displayMsg(String, int, int); // dans display
uint8_t extern getDeviceId();             // dans sdcard

// timers
Ticker blink_timer(blink, BLINK_TIME);
Ticker power_off_timer(powerOff, POWER_OFF_TIMEOUT, 1);
Ticker display_off_timer(displayOff, DISPLAY_OFF_TIMEOUT, 1);
Ticker display_default_timer(displayDefault, DISPLAY_DEFAULT_TIMEOUT, 1);

#endif
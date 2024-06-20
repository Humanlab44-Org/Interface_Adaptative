/*
Nom du fichier : main.h
Auteur : Yann Cesbron  (yann@cesbron.org)
Date de création : 4 avril 2023
Licence :  Creative Commons BY-NC-SA
*/

#include <Arduino.h>
#include <gpio.h>
#include "arduinoOTA.h"

#define ON true
#define OFF false
void extern initSDcard();      // dans sdCard
bool extern loadSysConf();     // dans sdCard
bool extern loadKeysConf();    // dans sdCard
void extern initDisplay();     // dans display
bool extern initKeysSensor();  // dans keySensor
void extern initWifi();        // dans net
void extern initWebserver();   // dans net
void extern initBLEKeyboard(); // dans BLEkeyboard
void extern initPowerButton(); // dans power
void extern MemoryInfos();     // dans misc

// void extern handleBLEKeyboard(); // TODO  non utilisé
void extern handleKeysSensor();  // dans keySensor
void extern handlePowerButton(); // dans power
void extern initTimers();        // dans misc
void extern handleTimers();
void extern powerOn();
bool extern loadKeysConf();
int extern getRadioMode();  // dans sdcard
void extern initNunchuck(); // dans nunchuck
void extern handleNunchuck(); // dans nunchuck
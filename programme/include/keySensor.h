/*
Nom du fichier : keySensor.h
Auteur : Yann Cesbron  (yann@cesbron.org)
Date de création : 4 avril 2023
Licence :  Creative Commons BY-NC-SA
*/

#include <Wire.h>
#include <ArduinoJson.h>
#include "Adafruit_MPR121.h"
#include "gpio.h"

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif
#define ON true
#define OFF false
#define PRESS_DELAY  20  // ms

bool initKeysSensor();
void handleKeysSensor();
void handleKey(int keyID, bool keyState);
// ADDR tied to GND : 0x5A   (default.. cut the thin track before short another)
// ADDR tied to 3V : 0x5B
// ADDR tied to SDA : 0x5C
// ADDR tied to SCL : 0x5D
#define NB_SENSOR 3
int nbKeySensorFound = 0;
uint16_t keySensorAdr[3] = {0x5A, 0x5B, 0x5C};
Adafruit_MPR121 keySensor[3] = {Adafruit_MPR121(), Adafruit_MPR121(), Adafruit_MPR121()};

#define PRESSED 1
#define RELEASED 0

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched[NB_SENSOR];
uint16_t currtouched[NB_SENSOR];

struct ksc
{
    int runState;     // run = 1
    int bankSelected; //
};
ksc keySensorContext;
int noTmpfileSelected;
String fileSelected;

    int extern nbBanks;              // mis à jour par SDCard.cpp
int extern nbconfKeys;               // mis à jour par SDCard.cpp
char extern confKeysFilenames[][25]; // mis à jour par SDCard.cpp

// ---------------- fonctions
void handleKeySensorContext(int, bool);
void extern sendKey(int banque, int touche, int keyState); // dans SDCard
void extern PowerOffTimer(bool);                           // dans power
String extern getBankName(int);                            // dans SDCard
String extern getProgramName();                            // dans SDCard
bool extern setDefaultCfg(String fileName);                 // dans SDCard
void extern startApp();                                    // dans SDCard
void extern stopApp();                                     // dans SDCard
bool extern setNbKeys(int);                                // dans SDCard
void extern initBeep();                                     // dans misc
void extern beep();                                         // dans misc
void extern displayMsg(String, int, int);               // dans display

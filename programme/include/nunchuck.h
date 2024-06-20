/*
Nom du fichier : nunchuck.h
Auteur : Yann Cesbron  (yann@cesbron.org)
Date de création : 4 avril 2023
Licence :  Creative Commons BY-NC-SA
*/

#include <Wire.h>
#include <ArduinoNunchuk.h>
#include "BleCombo.h"
#define OFFSET_XY 25
#define MOVE_DELAY 25
#define MOVE_MAX_POINTS 5
#define MOVE_MIN_POINTS 1

ArduinoNunchuk nunchuk = ArduinoNunchuk();


int calibValueX, calibValueY, handleNunchuckDelay;
int  oldCbuttonState, oldZbuttonState;
int valueY, valueX, oldValueY, oldValueX;
unsigned long currentTime = 0;
unsigned long previousTime = 0;
static bool isCenter = true;

void handleNunchuck();
void nunchuckTest();
void tri(int[], int);
void extern sendKeyToBLE(String, int , int );
void extern mouseMoveKillTask();
void extern mouseMove(uint8_t, uint8_t);
bool bNunchuck; // present?  retour de l'init de ArduinoNunchuck

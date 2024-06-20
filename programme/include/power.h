/*
Nom du fichier : power.h
Auteur : Yann Cesbron  (yann@cesbron.org)
Date de création : 4 avril 2023
Licence :  Creative Commons BY-NC-SA
*/

#ifndef POWER_H
#define POWER_H

#define ON true
#define OFF false
#include "gpio.h"
#include "OneButton.h"
// The 2. parameter activeLOW is true, because external wiring sets the button to LOW when pressed.
//  In case the momentary button puts the input to HIGH when pressed:
//  The 2. parameter activeLOW is false when the external wiring sets the button to HIGH when pressed.
//  The 3. parameter can be used to disable the PullUp .
//  OneButton button(PIN_INPUT, false, false);
OneButton PowerButton(POWER_BUTTON_PIN, false, false); //  bouton à vcc (3v)
// OneButton PowerButton(POWER_BUTTON_PIN,true); // bouton à GND
bool bwifiState = false;

void initPowerButton();
void handlePowerButton();
void powerOff();
void powerOn();
void disconnectDevice();
void extern displayRadio(bool); // dans display
void extern displayMsg(String, int, int);
void extern displayDefault();
void extern PowerOffTimer(bool); // dans misc
void extern initWifi();          // dans net
void extern switchRadioMode();  // dans sdcard
void extern setNewDeviceID();
void extern disconnectAll(); // dans BleComboKeyBoard
#endif
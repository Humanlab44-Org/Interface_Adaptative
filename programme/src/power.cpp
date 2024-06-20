/*
Nom du fichier : power.ccp
Auteur : Yann Cesbron  (yann@cesbron.org)
Date de création : 4 avril 2023
Licence :  Creative Commons BY-NC-SA
*/

#include "power.h"

void powerOn()
{
    pinMode(POWER_CMD_PIN, OUTPUT);
    //pinMode(POWER_BUTTON_PIN, INPUT_PULLUP); // maintien le enable du regulateur à on
    pinMode(POWER_BUTTON_PIN, INPUT); 
    digitalWrite(POWER_CMD_PIN, HIGH);
    //PowerOffTimer(ON); // dans misc
}

void powerOff()
{
    PowerOffTimer(OFF); // dans misc
    Serial.println(F("Power Off"));
    displayMsg("Power Off ", 0, 0);
    delay(2000);
    digitalWrite(POWER_CMD_PIN, LOW);
}

void initPowerButton()
{
    // link clicks function to be called on event.
    PowerButton.attachClick(displayDefault);
    PowerButton.attachDoubleClick(switchRadioMode);
    PowerButton.attachLongPressStart(powerOff);
    PowerButton.attachMultiClick(disconnectDevice);
}

void handlePowerButton()
{
    PowerButton.tick();
}

void disconnectDevice()
{
    if (PowerButton.getNumberClicks() > 4)
    {
        disconnectAll();
        // setNewDeviceID(); // incremente de 1 l'ID du perif dans le json et l'enregistre sur la carte  + reboot
    }
}
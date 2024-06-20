#include <keySensor.h>

/*
Nom du fichier : keySensor.ccp
Auteur : Yann Cesbron  (yann@cesbron.org)
Date de création : 4 avril 2023
Licence :  Creative Commons BY-NC-SA
*/

/*!
 *  @brief      Initialisation  sensors  type MPR121
 *  https://www.adafruit.com/product/1982
 *  @param      none
 *  @returns    true/false
 */
bool initKeysSensor()
{
    Serial.println(F("-- Initialisation keySensor :"));
    initBeep();
    // hardware keys sensor
    for (int i = 0; i < NB_SENSOR; i++)
    {
        Serial.print(F("MPR121 address: "));
        Serial.print(keySensorAdr[i], HEX);
        if (!keySensor[i].begin(keySensorAdr[i]))
        {
            Serial.println(F(" non trouve ou erreur"));
        }
        else
        {
            Serial.println(F(" trouve!"));
            nbKeySensorFound++;
            beep();
        }
    }
    Serial.print(F("- > nb MPR121: "));
    Serial.println(nbKeySensorFound);
    int nbHardwareKeys = (nbKeySensorFound * 12) - 2; // Nb max de boutons harware
    setNbKeys(nbHardwareKeys);
    Serial.println("");
    // --------------------- variables pour le fonctionnement  selection/validation
    keySensorContext.bankSelected = 0;
    keySensorContext.runState = false;
    noTmpfileSelected = -1;

    if (nbKeySensorFound > 0)
    {
        return (true);
    }
    else
    {
        return (false);
    }
}
/*!
 *  @brief      // Get the currently touched pads (sensor MPR121)
 *  and call misc cmdKey (int key, bool press/release) if key 0 or 1
 *  else for others
 *  call BLEkeyboard sendKey(int key, bool pressed/released) to send key
 *  @param      none
 *  @returns    none
 */
void handleKeysSensor()
{
    bool keyState;
    for (int y = 0; y < nbKeySensorFound; y++) // boucle sur 3 max
    {
        currtouched[y] = keySensor[y].touched();

        for (int i = 0; i < 12; i++)
        {

            if ((currtouched[y] & _BV(i)) && !(lasttouched[y] & _BV(i)))
            {
                Serial.print(i + (y * 12));
                Serial.println(F(" touche"));
                keyState = PRESSED;
                handleKey(i + (y * 12), keyState);
                if (i + (y * 12) < 2)
                {
                    beep();
                }
            }

            if (!(currtouched[y] & _BV(i)) && (lasttouched[y] & _BV(i)))
            {
                Serial.print(i + (y * 12));
                Serial.println(F(" relache"));
                keyState = RELEASED;
                handleKey(i + (y * 12), keyState);
            }
        }
        lasttouched[y] = currtouched[y];
        delay(PRESS_DELAY);
    }
}
/**
 * @brief   // look if the key is cmd or keyboard key
 * @param keyID
 * @param keyState
 */
void handleKey(int keyID, bool keyState)
{
    if (keyID < 2) // touches de commande
    {
        handleKeySensorContext(keyID, keyState);
        // Serial.println(F(" touche de commande"));
    }
    else
    {
        if (keySensorContext.runState == 1) // envoi uniquement en mode run
        {
            sendKey(keySensorContext.bankSelected, keyID - 2, keyState); // dans SDCard
            // Serial.println(F(" touche clavier/pad"));
        }
    }
    PowerOffTimer(ON); // restart le timer powerOFF
}

/**
 * @brief   // manage select & valid keys
 * @param keyID
 * @param keyState
 */
void handleKeySensorContext(int keyID, bool keyState)
{
    if (keyState == PRESSED)
    {
        switch (keySensorContext.runState)
        {
        case 0: // mode select file
            switch (keyID)
            {
            case 0: // selection
                noTmpfileSelected += 1;
                if (noTmpfileSelected >= nbconfKeys)
                {
                    noTmpfileSelected = 0;
                }
                Serial.print(F(" ---------- fichier app ou jeu selectionne: "));
                Serial.print(noTmpfileSelected);
                Serial.print(F(" "));
                Serial.println(confKeysFilenames[noTmpfileSelected]);
                fileSelected = confKeysFilenames[noTmpfileSelected];
                fileSelected = fileSelected.substring(0, fileSelected.lastIndexOf('.'));
                displayMsg("Selection APP/JEU \n" + fileSelected, 0, 0); // TODO    à reprendre avec l'affichage
                break;

            case 1: // validation
                if (noTmpfileSelected >= 0)
                {
                    // changement  nouvelle config
                    if (!setDefaultCfg((String)confKeysFilenames[noTmpfileSelected])) // dans SDCard
                    {                                                                 // erreur read or not exist
                        keySensorContext.runState = 1;                                // force le mode selection de fichier
                        noTmpfileSelected = -1;
                        break;
                    }
                    keySensorContext.bankSelected = 0;
                }
                // noTmpfileSelected = -1;
                keySensorContext.runState = 1;
                startApp(); // envoie la sequence de start // dans SDCard
                Serial.println();
                Serial.println(F(" ------> On passe en mode choix banque: "));
                
                Serial.print(F(" banque en cours: "));
                Serial.print(keySensorContext.bankSelected);
                Serial.print(" ");
                Serial.println(getBankName(keySensorContext.bankSelected));
                displayMsg(getProgramName() + "\n" + getBankName(keySensorContext.bankSelected), 0, 0); // TODO    à reprendre avec l'affichage
                break;
            default:
                break;
            }
            break;
        case 1: // mode run
            switch (keyID)
            {
            case 0: // selection  bank
                keySensorContext.bankSelected += 1;
                if (keySensorContext.bankSelected >= nbBanks)
                {
                    keySensorContext.bankSelected = 0;
                }
                Serial.print(F(" banque de touche selectionnee: "));
                Serial.print(keySensorContext.bankSelected);
                Serial.print(" ");
                Serial.println(getBankName(keySensorContext.bankSelected));
                displayMsg(getProgramName() + "\n" + getBankName(keySensorContext.bankSelected), 0, 0); // TODO    à reprendre avec l'affichage
                break;

            case 1: // validation
                keySensorContext.runState = 0;
                stopApp(); // envoie la sequence de stop // dans SDCard
                Serial.println();
                Serial.println(F(" ------> On passe en mode selection app ou jeu: "));
                fileSelected = confKeysFilenames[noTmpfileSelected]; // TODO    à reprendre avec l'affichage
                fileSelected = fileSelected.substring(0, fileSelected.lastIndexOf('.'));
                Serial.println(fileSelected);
                displayMsg("Selection APP/JEU \n" + fileSelected, 0, 0);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
}
bool isRunning()
{
    if (keySensorContext.runState == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

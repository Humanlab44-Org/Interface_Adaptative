/*
Nom du fichier : nunchuck.ccp
Auteur : Yann Cesbron  (yann@cesbron.org)
Date de création : 4 avril 2023
Licence :  Creative Commons BY-NC-SA
*/

#include "nunchuck.h"

void doNunchu()
{
    static int nbPts = 5;
    nunchuk.update();
    // nunchuckTest();

    if (oldCbuttonState != nunchuk.cButton)
    {
        // sendKeyToBLE(right/left 2/1, type 2, press/release 1/0);
        if (nunchuk.cButton == 1)
        {
            // Serial.print(" right button PRESSED:");
            sendKeyToBLE("2", 2, 1); // envoi de la commande vers bluetooth dans BLEKeyboard
        }
        else
        {
            // Serial.print(" right button RELEASED:");
            sendKeyToBLE("2", 2, 0);
        }
        oldCbuttonState = nunchuk.cButton;
    }

    if (oldZbuttonState != nunchuk.zButton)
    {
        if (nunchuk.zButton == 1)
        {
            // Serial.print(" left button PRESSED:");
            sendKeyToBLE("1", 2, 1);
        }
        else
        {
            // Serial.print("  left button RELEASED:");
            sendKeyToBLE("1", 2, 0);
        }
        oldZbuttonState = nunchuk.zButton;
    }

    if (nunchuk.analogX > calibValueX + OFFSET_XY)
    {
        valueX = nunchuk.analogX; // positif a droite
        valueX = valueX - OFFSET_XY - calibValueX;
        valueX = map(valueX, 0, 70, MOVE_MIN_POINTS, MOVE_MAX_POINTS);
        valueX = min(valueX, 5);
        if (valueX != oldValueX)
        {
            // Serial.print(" droite:");
            // Serial.println(valueX);
            if (!isCenter)
            {
                mouseMoveKillTask();
            }
            mouseMove(2, valueX);
            oldValueX = valueX;
        }
        isCenter = false;
    }
    if (nunchuk.analogX < calibValueX - OFFSET_XY)
    {
        valueX = nunchuk.analogX; // negatif a gauche
        valueX = ((calibValueX - OFFSET_XY) - valueX);
        valueX = map(valueX, 0, 70, MOVE_MIN_POINTS, MOVE_MAX_POINTS);
        valueX = min(valueX, 5);
        if (valueX != oldValueX)
        {
            // Serial.print(" gauche:");
            // Serial.println(valueX);
            if (!isCenter)
            {
                mouseMoveKillTask();
            }
            mouseMove(1, valueX);
            oldValueX = valueX;
        }
        isCenter = false;
    }

    if (nunchuk.analogY > calibValueY + OFFSET_XY)
    {
        valueY = nunchuk.analogY; // positif en haut
        valueY = valueY - OFFSET_XY - calibValueY;
        valueY = map(valueY, 0, 70, MOVE_MIN_POINTS, MOVE_MAX_POINTS);
        valueY = min(valueY, 5);
        if (valueY != oldValueY)
        {
            // Serial.print(" haut:");
            // Serial.println(valueY);
            if (!isCenter)
            {
                mouseMoveKillTask();
            }
            mouseMove(8, valueY);
            oldValueY = valueY;
        }
        isCenter = false;
    }
    if (nunchuk.analogY < calibValueY - OFFSET_XY)
    {
        valueY = nunchuk.analogY; // negatif en bas
        valueY = ((calibValueY - OFFSET_XY) - valueY);
        valueY = map(valueY, 0, 70, MOVE_MIN_POINTS, MOVE_MAX_POINTS);
        valueY = min(valueY, 5);
        if (valueY != oldValueY)
        {
            // Serial.print(" bas:");
            // Serial.println(valueY);
            if (!isCenter)
            {
                mouseMoveKillTask();
            }
            mouseMove(4, valueY);
            oldValueY = valueY;
        }
        isCenter = false;
    }
    // test retour au centre dans la zone offset
    int posX = nunchuk.analogX;
    int posY = nunchuk.analogY;

    if (posX<calibValueX + OFFSET_XY & posX> calibValueX - OFFSET_XY & posY<calibValueY + OFFSET_XY & posY> calibValueY - OFFSET_XY)
    {
        if (!isCenter)
        {
            // Serial.println(" retour au centre");
            mouseMoveKillTask();
            isCenter = true;
        }
    }
}

void nunchuckTest()
{
    // Serial.print(" analogX:");
    // Serial.print(nunchuk.analogX, DEC);
    // Serial.print(" analogY:");
    // Serial.print(nunchuk.analogY, DEC);
    // // Serial.print(" accelX:");
    // // Serial.print(nunchuk.accelX, DEC);
    // // Serial.print(" accelY:");
    // // Serial.print(nunchuk.accelY, DEC);
    // // Serial.print(" accelZ:");
    // // Serial.print(nunchuk.accelZ, DEC);
    // Serial.print(" zButton:");
    // Serial.print(nunchuk.zButton, DEC);
    // Serial.print(" cButton:");
    // Serial.println(nunchuk.cButton, DEC);
}

void initNunchuck()
{
    int valueX[10], valueY[10];
    int sumX = 0, sumY = 0;
    bNunchuck = nunchuk.init(); // dans la lib  retour true/false
    if (bNunchuck)  // seulement si le nunchuck est present
    {
        // 10 lectures
        for (int i = 0; i < 10; i++)
        {
            nunchuk.update();
            valueX[i] = nunchuk.analogX;
            valueY[i] = nunchuk.analogY;
            delay(20);
        }
        // tri croissant
        tri(valueX, 8);
        tri(valueY, 8);
        // enleve les extremes, il en reste 8
        for (int i = 1; i < 9; i++)
        {
            sumX += valueX[i];
            sumY += valueY[i];
        }
        // affiche pour info
        // for (int i = 0; i < 8; i++)
        // {
        //     printf("[%d] => %d %d\n", i, valueX[i], valueY[i]);
        // }
        //  moyenne
        calibValueX = sumX / 8;
        calibValueY = sumY / 8;
        oldCbuttonState = nunchuk.cButton;
        oldZbuttonState = nunchuk.zButton;
        // attention au delay..
        handleNunchuckDelay = MOVE_DELAY;
        Serial.println(F("-- Calibration Nunchuck\n"));
        // Serial.print(" calibValueX:");
        // Serial.print(calibValueX, DEC);
        // Serial.print(" calibValueY:");
        // Serial.println(calibValueY, DEC);
    }
}

void handleNunchuck()
{
    if (bNunchuck)   // seulement si le nunchuck est present
    {
        currentTime = millis();
        if ((currentTime - previousTime) > handleNunchuckDelay)
        {
            previousTime = currentTime;
            // Serial.print(F("nunchuck update"));
            // nunchuckTest();
            doNunchu();
        }
    }
}

void tri(int tableau[], int taille)
{
    int i, j, indice_min, tmp;
    for (i = 0; i < taille - 1; i++)
    {
        indice_min = i;
        for (j = i + 1; j < taille; j++)
        {
            if (tableau[j] < tableau[indice_min])
            {
                indice_min = j;
            }
        }
        if (indice_min != i)
        {
            tmp = tableau[i];
            tableau[i] = tableau[indice_min];
            tableau[indice_min] = tmp;
        }
    }
}
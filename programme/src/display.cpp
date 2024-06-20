/*
Nom du fichier : display.ccp
Auteur : Yann Cesbron  (yann@cesbron.org)
Date de création : 4 avril 2023
Licence :  Creative Commons BY-NC-SA
*/

#include "display.h"

//
// *********************  code pour les tests    A REPRENDRE sous forme d'objet
//

void initDisplay()
{
    Serial.println(F("-- Initialisation afficheur"));
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Boucle infinie afficheur en erreur
    }
    Serial.println();

    // display.display(); // TODO splash  ->> à changer
    // delay(2000);       // Pause for 2 seconds
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);
    display.println(" HumanLab");
    display.println(" Clavier");
    display.println(" Adaptatif");

    // float vbat = (getVbat()*0.001);
    // display.print("Batt :");
    // display.print(vbat,1);
    // display.println("V");

    displayBatt();
    display.display();
    // delay(500); // Pause for 2 seconds
}
/**
 * @brief retourne à l'affichage par defaut à l'issue du display_timer
 *
 */
void displayDefault()
{
    // Serial.println(F("Display mode Bluetooth ou Wifi"));
    startDisplayOffTimer(); // dans misc
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);
    display.print("Mode ");
    if (getRadioMode() == 0)
    {
        display.println("WIFI");
        display.setTextSize(1);
        display.println("IP  192.168.4.1");
        std::string tmp_str = getAPssid();
        display.println("AP  " + String(tmp_str.c_str()));
        tmp_str = getAPpassword();
        display.println("MDP " + String(tmp_str.c_str()));

        display.setTextSize(2);

        displayBatt();
    }
    else
    {
        display.print("BLE");
        if (isConnectedBLE())
        {
            display.println(" *");
        }
        else
        {
            display.println(" ");
        }
        display.println(getBtName());

        displayBatt();
    }
    display.display();
}

void displayMsg(String msg, int row, int col)
{
    // Serial.println(F("display msg"));
    // Serial.println(msg);
    startDisplayDefaultTimer(); // dans misc
    startDisplayOffTimer();     // dans misc
    display.clearDisplay();
    // display.display();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(row, col);         // Start at top-left corner
    display.println(msg);
    // display.setCursor(0, 0);
    display.display();
}

/**
 * @brief // affiche l'icone radio wifi ou BLE
 *  en fonction de  bwifiState
 *  et start le time out
 */
void displayRadio(bool radioState)
{
    // Serial.println(F("Display Radio"));
    //  TODO DISPLAY à revoir avec les icones ?
    startDisplayDefaultTimer(); // dans misc pour remettre l'affichage par default
    display.clearDisplay();
    display.display();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);
    display.println("- RADIO -");
    if (radioState)
    {
        display.print("Wifi ON");
    }
    else
    {
        display.print("BLE ON");
    }
    display.setCursor(0, 0);
    display.display();
}

void displayOff()
{
    // Serial.println(F("Display off"));
    display.clearDisplay();
    display.display();
}

void displayError(String msg, bool fatal)
{
    // Serial.print(F("Display Error : "));
    startDisplayDefaultTimer(); // pour remettre l'affichage par default
    display.clearDisplay();
    display.display();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);
    // TODO  DISPLAY à reprendre
    display.println("ERREUR");
    display.print(msg);
    display.display();
    if (fatal == true)
    {
        delay(5000);
        powerOff();
    }
}

void displayBatt()
{
    float vbat = (getVbat() * 0.001);
    display.print("Batt :");
    display.print(abs(vbat - 3) / 1.3 * 100, 0);
    display.println("%");
}

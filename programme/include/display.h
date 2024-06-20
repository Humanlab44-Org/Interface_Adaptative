/*
Nom du fichier : display.h
Auteur : Yann Cesbron  (yann@cesbron.org)
Date de création : 4 avril 2023
Licence :  Creative Commons BY-NC-SA
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// 0.91 Inch OLED I2C Display 128 x 32   SSDD1306

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C // Address; 0x3D for 128x64, 0x3C for 128x32
#define OLED_RESET -1       // Reset pin
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
void displayMsg(String msg, int col, int row);
void displayOff();
void displayDefault();
void displayBatt();
void displayRadio(bool);
void extern startDisplayOffTimer();     // dans misc
void extern startDisplayDefaultTimer(); // dans misc
void extern powerOff();                 // dans power
int extern getRadioMode();              // dans sdcard
int extern getVbat();                   // dans misc
bool isConnectedBLE();                  // dans BLEkeyboard
String extern getBtName();              // dans sdcard
std::string extern getAPpassword();     // dans net
std::string extern getAPssid();         // dans net

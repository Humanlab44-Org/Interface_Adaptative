/*
Nom du fichier : gpio.h
Auteur : Yann Cesbron  (yann@cesbron.org)
Date de création : 4 avril 2023
Licence :  Creative Commons BY-NC-SA
*/

// ---------------------   GPIO ESP32
// TX = 1;
// RX = 3;


// -- i2c  pour modules
// SDA = 21;
// SCL = 22;
//       affichage  Address; 0x3D for 128x64, 0x3C for 128x32
//       boutons/pad
//          ADDR not connected : 0x5A
//          ADDR tied to 3V : 0x5B
//          ADDR tied to SDA : 0x5C
//          ADDR tied to SCL : 0x5D

// -- spi   pour module carte SD
// SS = 5;
// MOSI = 23;
// MISO = 19;
// SCK = 18;

// -- SS pin SDcard
#define SDCARD_SS_PIN 5

// -- power
#define POWER_BUTTON_PIN 14  // bouton M/A appui bref ON,  appui long (>2s) pour OFF, double clic swicth WIFI/BLE
#define POWER_CMD_PIN 12     // commande du enable régulateur

// -- Serial2 pour debug et pour liberer Serial
#define RXD2 26
#define TXD2 27

// -- buzzer
#define BUZZER_PIN 32 // PWM output

// -- analog tension batterie
#define BATTERY_PIN 36  // analog input

/*
Nom du fichier : net.h
Auteur : Yann Cesbron  (yann@cesbron.org)
Date de création : 4 avril 2023
Licence :  Creative Commons BY-NC-SA
*/

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#define DYNAMIC_JSON_DOCUMENT_SIZE 4096 // Wroom !!!!! redefini avant la taille du buffer JSON pour ESPAsyncWebServer (dans AsyncJson.h de la lib)
// #define DYNAMIC_JSON_DOCUMENT_SIZE 8192 // Wrover !!!!! redefini avant la taille du buffer JSON pour ESPAsyncWebServer (dans AsyncJson.h de la lib)
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include <ESPAsyncWebServer.h>

#define ON true
#define OFF false

#define CONFIGFILE "/sysConfig.json"  
bool extern saveJsonFile(String file, DynamicJsonDocument *jsonDoc, bool reboot);

File uploadFile;

void initWebserver();
// ---------------------------------- Web serveur
AsyncWebServer server(80);
// const char *MDNSname = "HandiPad";
bool BrowserConnected = false;
void notFound(AsyncWebServerRequest *request);

// ---------------------------------- Wifi
const char *APssid = "Wssid";
const char *APpassword = "Wpasswd";
std::string ssid;
std::string passwd;
int rssiMin = 95;
IPAddress myIP;

std::string extern getAPssid();
std::string extern getAPpassword();
String extern getKeysConfFilesNames(); // dans sdcard

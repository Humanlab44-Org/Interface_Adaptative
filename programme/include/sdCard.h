/*
Nom du fichier : sdCard.h
Auteur : Yann Cesbron  (yann@cesbron.org)
Date de création : 4 avril 2023
Licence :  Creative Commons BY-NC-SA
*/

#include <SD.h>
#include <ArduinoJson.h>
#include <gpio.h>


#define CONFIGFILE "/sysConfig.json"
#define KEYBFR "/keyb_fr.json"
#define REPKEYCONF "/keysConfs"

//   -----------  https://arduinojson.org/v6/assistant/
const size_t confSysCapacity = 1024;
DynamicJsonDocument *pJSONconfSys; // dans PSRAM
// const size_t confKeysCapacity = 4096000; // --> c'est la taille max de la PSRAM
// const size_t confKeysCapacity = 2048000; //  2Mo dans PSRAM
const size_t confKeysCapacity = 20480; //  20ko en ram
DynamicJsonDocument *pJSONconfKeys;

uint32_t volumesize;
static const int MAX_FILES = 50;

char confKeysFilenames[MAX_FILES][25];
int nbconfKeys;
int nbBanks;

void initSDcard();
void printDirectory(File, int);
void listKeysConfFiles();
bool loadSysConf();
bool loadKeysConf();
String getBankName(int);
std::string getBLEdeviceName();
std::string getBLEmanufacturer();
std::string getAPssid();
std::string getAPpassword();
uint8_t getDeviceId();
String extern getBtName();

void startApp();
void stopApp();
void sendKey(int banque, int touche, int keyState);
bool saveJsonFile(String file, DynamicJsonDocument *jsonDoc, bool);
void extern displayError(String msg, bool fatal);             // dans display
void extern displayMsg(String msg, int row, int col);         // dans display
void extern sendKeyToBLE(String key, int type, int keyState); // dans BLEKeyboard
void extern reset_all();                                      // dans misc
bool extern isConnectedBLE();                            // dans bleKeyboard


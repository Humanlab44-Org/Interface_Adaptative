/*
Nom du fichier : main.ccp
Auteur : Yann Cesbron  (yann@cesbron.org)
Date de création : 4 avril 2023
Licence :  Creative Commons BY-NC-SA
*/

#include "main.h"

/*!
 *  @brief      Setup de main.cpp (framework arduino)
 */
void setup()
{
  //psramInit(); // --------------------------- initialise la PSRAM .. seulement pour un wroover
  powerOn();
  Serial.begin(115200);
  Serial.flush();
  // Serial.print(F("---- debug sur Serial2  TX pin:"));
  // Serial.println(TXD2);
  //  ---------------  debug sur Serial2
  // Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.println(F("==============================================================="));
  Serial.println(F("---- Starting HumanLab/HandiPad ----"));
  Serial.println(F("==============================================================="));
  // --------------------
  MemoryInfos();
  // infos
  // chip_infos();
  // ----------------init display
  initDisplay();
  // --------------- init  SDcard
  initSDcard();
  // ------------  charge la config systeme
  loadSysConf();
  //  --------- restaure la derniere config clavier
  loadKeysConf();
  MemoryInfos();
  //   ------------  init keySensor
  initKeysSensor();
  // --------------- init bouton on/off
  initPowerButton();
  // start mode BLE = 1 et WIWI = 0 incompatible -> partage meme antenne
  if (getRadioMode() == 0)
  { 
    //  --------------- init wifi & webserver
    initWifi();
    ArduinoOTA.begin();
  }
  else
  { 
    //   -------------  Nunchuck (manette wii)
    initNunchuck();
    //   -------------  init Clavier & souris en Bluetooth
    initBLEKeyboard();
  }

  // --------------- init Timers()
  initTimers();
  MemoryInfos();
}
/*!
 *  @brief      loop de main.cpp (framework arduino)
 *  @param      none
 *  @returns    void
 */
void loop()
{
  handleKeysSensor();
  handlePowerButton();
  if (getRadioMode() != 0)
  { // bluetooth
    handleNunchuck();
  }
  else
  { // wifi
    ArduinoOTA.handle();
  }
  handleTimers();
  delay(10);
  yield();
}

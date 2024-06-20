#include "BleConnectionStatus.h"
#include <BLEDevice.h>

BleConnectionStatus::BleConnectionStatus(void)
{
}

void BleConnectionStatus::onConnect(BLEServer *pServer)
{
  this->connected = true;
  BLE2902 *desc = (BLE2902 *)this->inputKeyboard->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(true);
  desc = (BLE2902 *)this->inputMouse->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(true);
  desc = (BLE2902 *)this->inputMediaKeys->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(true);
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->stop();

  Serial.println(F("Connexion Device"));
  std::string descriptorInfo = desc->toString();
  String descriptorInfoArduino(descriptorInfo.c_str());
  Serial.print("Informations sur le périphérique connecté : ");
  Serial.println(descriptorInfoArduino);

  //----------------------------------------------------------------
  // Récupérer l'adresse MAC du périphérique connecté
  // Récupérer l'objet BLEClient
  uint16_t conn_id = pServer->getConnId();
  BLEClient *pClient = BLEDevice::getClientByGattIf(conn_id);

  //----------------------
  // TODO  Récupérer le nom du périphérique associé à l'adresse MAC
  //----------------------
  Serial.print("ID de connexion:");
  Serial.println(conn_id);
  // ----------------------------------------------------------------
  displayDefault();
}

void BleConnectionStatus::onDisconnect(BLEServer *pServer)
{
  this->connected = false;
  // clavier
  BLE2902 *desc = (BLE2902 *)this->inputKeyboard->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(false);
  // souris
  desc = (BLE2902 *)this->inputMouse->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(false);
  // mediaKey
  desc = (BLE2902 *)this->inputMediaKeys->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(false);
  // // ajouter une pause ici ?
  // /* pause de 1000ms. */
  // //   const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;
  // //   vTaskDelay(xDelay);
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  Serial.println(F("Deconnexion Device"));
  displayDefault();
}

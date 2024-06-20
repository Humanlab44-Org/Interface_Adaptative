/*
Nom du fichier : misc.ccp
Auteur : Yann Cesbron  (yann@cesbron.org)
Date de création : 4 avril 2023
Licence :  Creative Commons BY-NC-SA
*/

#include "misc.h"

// ------------------  timers (ticker) -----------------------
void initTimers()
{
    blink_timer.start();
    display_off_timer.start();
    display_default_timer.start();
    power_off_timer.start();
}

void handleTimers()
{
    blink_timer.update();
    power_off_timer.update();
    display_off_timer.update();
    display_default_timer.update();
}

void extern startDisplayDefaultTimer()
{
    display_default_timer.resume();
}

void startDisplayOffTimer()
{
    display_off_timer.resume();
}

void PowerOffTimer(bool state)
{
    if (state)
    {
        power_off_timer.resume();
    }
    else
    {
        power_off_timer.stop();
    }
}

void blink()
{
    // int vbat = getVbat(); // TODO    ??   a completer pour affichage ?
}

void initBeep()
{
    // buzzer
    ledcSetup(0, 2000, 8);        // channel, frequency, resolution
    ledcAttachPin(BUZZER_PIN, 0); // buzzerPin, channel
}

void beep()
{
    ledcWrite(0, 128); // channel, dutyCycle = 0 pour arreter  128 =50%
    delay(200);
    ledcWrite(0, 0);
}

// -------------------  Memory ---------------------------------
/**
 * @brief Affiche l'etat de la mémoire
 *
 */
void MemoryInfos()
{
    Serial.print(F("---- Memory Infos -- "));
    Serial.printf("Heap: Size:%i, Free:%i, Min:%i,  Alloc:%i -- ", ESP.getHeapSize(), ESP.getFreeHeap(), ESP.getMinFreeHeap(), ESP.getMaxAllocHeap());
    Serial.printf("PSRAM: Size:%i, Free:%i\n", ESP.getPsramSize(), ESP.getFreePsram());
    //   log_d("---- Memory Infos -- ");
    //   log_d("Heap: Size:%i, Free:%i, Min:%i,  Alloc:%i -- ", ESP.getHeapSize(), ESP.getFreeHeap(), ESP.getMinFreeHeap(), ESP.getMaxAllocHeap());
    //   log_d("PSRAM: Size:%i, Free:%i\n", ESP.getPsramSize(), ESP.getFreePsram());
    Serial.println("");
}

// --------------- commandes
/**
 * @brief reset de l'ESP
 *
 */
void reset_all()
{
    Serial.print(F("---- restart ----"));
    for (int x = 0; x < 5; x++)
        ESP.restart();                  //  reset
    Serial.print(F("****restart****")); // n'affiche jamais ca...
}

/**
 * @brief Get the Vbat value
 *
 * @return int
 */
int getVbat()
{
    int vBat = map(analogRead(BATTERY_PIN), 0, 4095, 0, 4300); //  mV read  input analog pin  // TODO à etalonner
    return (vBat);
}

void affMacAdd()
{

    // Récupère l'adresse MAC BLE de l'appareil
    uint8_t macAddress[6];
    esp_base_mac_addr_get(macAddress); // c'est pas la bonne adresse ?  
    // Affiche l'adresse MAC de base
    Serial.print(F("Adresse MAC de base : "));
    for (int i = 0; i < 6; i++)
    {
        // Serial.print(macAddress[i], HEX);
        Serial.printf("%02X", macAddress[i]);
        if (i < 5)
        {
            Serial.print(F(":"));
        }
    }
    Serial.println("");
    // Affiche l'adresse MAC Bluetooth
    esp_read_mac(macAddress, ESP_MAC_BT);
    Serial.print(F("Adresse MAC BLE : "));
    for (int i = 0; i < 6; i++)
    {
       // Serial.print(macAddress[i], HEX);
        Serial.printf("%02X", macAddress[i]);
        if (i < 5)
        {
            Serial.print(F(":"));
        }
    }
    Serial.println();
}

esp_err_t changeMacAddress()
{
    esp_err_t ret;
    uint8_t devId = getDeviceId();      // recupere cette info dans le JSON
    Serial.printf("deviceID: ", devId); 
    uint8_t macAddress[6];
    affMacAdd();
    return ESP_OK;
    if (devId != 0)
    {
        //new_mac_address[5] = devId;
        // recupere l'adresse mac de base courante
        esp_base_mac_addr_get(macAddress);
        // Désactiver le contrôleur Bluetooth et liberer les ressources
        esp_bt_controller_disable();
        esp_bt_controller_deinit();
        esp_ble_scan_dupilcate_list_flush();

        // Modifier l'adresse MAC
        macAddress[5] = devId;
        ret = esp_base_mac_addr_set(macAddress);
        if (ret != ESP_OK)
        {
            Serial.println(F("Erreur lors du changement de l'adresse MAC"));
            return ret;
        }
        Serial.println(F("Adresse MAC changée avec succès !"));
        // Réactiver le contrôleur Bluetooth
        // esp_bt_controller_enable(ESP_BT_MODE_BLE);  // BLE seulement
        esp_bt_controller_enable(ESP_BT_MODE_BTDM);    // dual mode
        affMacAdd();
    }
    return ESP_OK;
}
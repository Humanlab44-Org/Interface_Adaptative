/*
Nom du fichier : sdCard.ccp
Auteur : Yann Cesbron  (yann@cesbron.org)
Date de création : 4 avril 2023
Licence :  Creative Commons BY-NC-SA
*/

#include "sdCard.h"

void initSDcard()
{
    File root;
    Serial.print(F("-- Initialisation de la carte SD :"));
    pinMode(SDCARD_SS_PIN, OUTPUT);
    if (!SD.begin(SDCARD_SS_PIN))
    {
        Serial.println(F(" Erreur!"));
        displayError("Carte SD ??", true); // bloquant
    }
    Serial.println(F(" ok"));
    uint8_t cardType = SD.cardType();
    Serial.print(F("SD Card Type: "));
    if (cardType == CARD_MMC)
    {
        Serial.println(F("MMC"));
    }
    else if (cardType == CARD_SD)
    {
        Serial.println(F("SDSC"));
    }
    else if (cardType == CARD_SDHC)
    {
        Serial.println(F("(SDHC"));
    }
    else
    {
        Serial.println(F("(UNKNOWN"));
    }
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    Serial.println();
    // ------------   affichage contenu SDcard
    // root = SD.open("/");
    // printDirectory(root, 0);
    // root.close();
    // Serial.println();
    listKeysConfFiles();
    pJSONconfSys = new DynamicJsonDocument(confSysCapacity);
    pJSONconfKeys = new DynamicJsonDocument(confKeysCapacity);
}

bool loadSysConf()
{
    File SDfile = SD.open(CONFIGFILE);
    if (!SDfile)
    {
        Serial.print(F("Failed open"));
        Serial.println(CONFIGFILE);
        displayError("open confsys", true);
    }

    // // -----------------------  affichage du contenu
    // while (SDfile.available())
    // {
    //   Serial.write(SDfile.read());
    // }

    // --------------- on recupere les infos du json
    DeserializationError error = deserializeJson(*pJSONconfSys, SDfile);
    if (error)
    {
        Serial.println(F("Error parsing JSON"));
        displayError("parsing JSON", false); // non bloquant
        displayError(error.c_str(), true);   // bloquant
        while (1)
            ; // Boucle infinie, arret du programme
    }
    SDfile.close();

    Serial.print(F("Nom bluetooth: "));
    Serial.println((*pJSONconfSys)["BLE"]["deviceName"].as<String>());
    Serial.print(F("Nom PA wifi: "));
    Serial.println((*pJSONconfSys)["WIFI"]["ssid"].as<String>());
    Serial.print(F("Password wifi: "));
    Serial.println((*pJSONconfSys)["WIFI"]["password"].as<String>());
    Serial.print(F("Nom : "));
    Serial.println((*pJSONconfSys)["USER"]["name"].as<String>());
    Serial.print(F("Prenom: "));
    Serial.println((*pJSONconfSys)["USER"]["surname"].as<String>());
    Serial.print(F("defaultCfg: "));
    Serial.println((*pJSONconfSys)["USER"]["defaultCfg"].as<String>());
    Serial.print(F("BLE on: "));
    Serial.println((*pJSONconfSys)["POWER"]["radioMode"].as<String>());
    Serial.print(F("Power timeOut: "));
    Serial.println((*pJSONconfSys)["POWER"]["timeOut"].as<String>());
    Serial.print(F("Hardware nbKeys: "));
    Serial.println((*pJSONconfSys)["HARDWARE"]["nbkeys"].as<String>());
    Serial.print(F("Device Id: "));
    Serial.println((*pJSONconfSys)["HARDWARE"]["id"].as<String>());

    return (true);
}
// ----------------------------------  keysConfigFile -----------------------------------
bool loadKeysConf()
{
    String keyFilePath = String(REPKEYCONF) + "/" + (*pJSONconfSys)["USER"]["defaultCfg"].as<String>();
    Serial.print(F(" ------> Chargement du fichier keysConf: "));
    Serial.println(keyFilePath);
    File SDfile = SD.open((String)keyFilePath, FILE_READ); // Ouverture en lecture seule
    if (!SDfile)
    {
        Serial.println(F("Failed to read file"));
        displayError(" conf clavier", false); // bloquant true
        return (false);
    }
    // Serial.println("deserialisation du json");
    //  const size_t confKeysCapacity = 2048000; //  pour les tests dans la psram ok
    // DynamicJsonDocument JSONconfKeys(confKeysCapacity);
    DeserializationError error = deserializeJson(*pJSONconfKeys, SDfile);
    if (error)
    {
        Serial.println(F("Error parsing JSON"));
        Serial.println(error.c_str());
        // sur l'afficheur
        displayError("parsing JSON", false); // non bloquant
        displayError(error.c_str(), true);   // bloquant
        while (1)
            ; // Boucle infinie, arret du programme
    }
    SDfile.close();

    // ****************************   affichage contenu par acces direct au JSON
    String programName = (*pJSONconfKeys)["PROGRAM"]["name"];
    Serial.print(F("nom app ou jeu: "));
    Serial.println(programName);

    // ------------- maj nb banques en fonction du nombre d'items
    nbBanks = (*pJSONconfKeys)["BANKS"].size();
    Serial.print(F("Nb de banques de touches trouvees: "));
    Serial.println(nbBanks);
    Serial.println();
    return (true);
}
// -----------------------------------------------------
void printDirectory(File dir, int numTabs)
{
    while (true)
    {
        File entry = dir.openNextFile();
        if (!entry)
        {
            // no more files
            break;
        }
        for (uint8_t i = 0; i < numTabs; i++)
        {
            Serial.print('\t');
        }
        Serial.print(entry.name());
        if (entry.isDirectory())
        {
            Serial.println("/");
            printDirectory(entry, numTabs + 1);
        }
        else
        {
            // files have sizes, directories do not
            Serial.print("\t\t");
            Serial.println(entry.size(), DEC);
        }
        entry.close();
    }
}

String getKeysConfFilesNames()
{
    listKeysConfFiles();
    // Créer un objet JSON
    StaticJsonDocument<1024> doc;
    JsonArray fichiers = doc.to<JsonArray>();
    int numFiles = sizeof(confKeysFilenames) / sizeof(confKeysFilenames[0]);
    for (int z = 0; z < numFiles; z++)
    {
        // Serial.println(confKeysFilenames[z]);
        if (confKeysFilenames[z][0] != '\0')
        {
            fichiers.add(confKeysFilenames[z]);
        }
    }
    // Convertir l'objet JSON en chaîne de caractères
    String json;
    serializeJson(doc, json);
    return json;
}

void listKeysConfFiles()
{
    // Ouvre le repertoire racine
    File keyConfDir = SD.open(REPKEYCONF);
    if (!keyConfDir)
    {
        Serial.print(F("Impossible d'ouvrir le repertoire "));
        Serial.println(REPKEYCONF);
        while (1)
            ; // Boucle infinie, arret du programme   // TODO  message sur afficheur Ã  ajouter
    }
    // Parcourt tous les fichiers du repertoire

    nbconfKeys = 0;

    while (File keyFile = keyConfDir.openNextFile())
    {
        // Verifie si le fichier est un fichier
        if (keyFile.isDirectory())
        {
            // Ignore les repertoires
            keyFile.close();
            continue;
        }

        // Stocke le nom du fichier sans l'extension dans un tableau de chaines de caracteres

        if (nbconfKeys < MAX_FILES)
        {
            strncpy(confKeysFilenames[nbconfKeys], keyFile.name(), 25);
            nbconfKeys++;
        }
        // Ferme le fichier
        keyFile.close();
    }

    // --------- affiche la liste des fichiers
    Serial.println(F("Liste des fichiers de keyConfig"));
    int numFiles = sizeof(confKeysFilenames) / sizeof(confKeysFilenames[0]);
    for (int z = 0; z < numFiles; z++)
    {
        if (confKeysFilenames[z][0] != '\0')
        {
            Serial.println(confKeysFilenames[z]);
        }
    }
    Serial.println("");
}

String getBankName(int banque)
{

    return ((*pJSONconfKeys)["BANKS"][banque]["name"].as<String>());
}

int getRadioMode()
{
    return ((*pJSONconfSys)["POWER"]["radioMode"].as<int>());
}

String getBtName()
{
    // return ((*pJSONconfSys)["BLE"]["deviceName"].as<String>());
    return ((*pJSONconfSys)["BLE"]["deviceName"].as<String>() + "-" +
            (*pJSONconfSys)["HARDWARE"]["id"].as<String>());
}

String getProgramName()
{
    return ((*pJSONconfKeys)["PROGRAM"]["name"].as<String>());
}

bool setDefaultCfg(String fileName)
{
    // File SDfile = SD.open(CONFIGFILE, FILE_WRITE);
    // // Vérifier si le fichier a été ouvert avec succès
    // if (!SDfile)
    // {
    //     Serial.println(F("Erreur d'ouverture du fichier"));
    //     return (false);
    // }
    Serial.print(F("  defaultCfg selectionne:"));
    Serial.println(fileName);
    (*pJSONconfSys)["USER"]["defaultCfg"] = fileName.c_str();
    // charge le nouveau fichier de touche (keyConfig)
    if (saveJsonFile(CONFIGFILE, pJSONconfSys, false))
    {
        return (loadKeysConf());
    }
    else
    {
        return (false);
    }
}

bool saveJsonFile(String file, DynamicJsonDocument *pJsonDoc, bool reboot)
{
    File SDfile = SD.open(file, FILE_WRITE);
    // Vérifier si le fichier a été ouvert avec succès
    if (!SDfile)
    {
        Serial.println(F("Erreur d'ouverture du fichier"));
        return (false);
    }
    // Serial.println(F("Ecriture du fichier de config"));
    // serializeJson((*pJsonDoc), Serial);
    serializeJson(*pJsonDoc, SDfile); // ecrit le json
    SDfile.close();
    if (reboot)
    {
        reset_all(); // reboot
    }
    return (true);
}

bool setNbKeys(int nbk)
{
    File SDfile = SD.open(CONFIGFILE, FILE_WRITE);

    // Vérifier si le fichier a été ouvert avec succès
    if (!SDfile)
    {
        Serial.println(F("Erreur d'ouverture du fichier"));
        return (false);
    }
    // Serial.print(F("  defaultCfg selectionne:"));
    // Serial.println(fileName);
    (*pJSONconfSys)["HARDWARE"]["nbkeys"] = String(nbk);
    // Serial.println(F("contenu à ecrire:"));
    // serializeJson((*pJSONconfSys), Serial);
    serializeJson(*pJSONconfSys, SDfile); // ecrit le contenu modifie
    // Fermer le fichier
    SDfile.close();
    // charge le nouveau fichier de touche (keyConfig)
    // loadKeysConf();
    return (true);
}

void switchRadioMode()
{
    loadSysConf();
    int tmpInt;
    File SDfile = SD.open(CONFIGFILE, FILE_WRITE);
    // Vérifier si le fichier a été ouvert avec succès
    if (!SDfile)
    {
        Serial.println(F("Erreur d'ouverture du fichier de config"));
    }
    else
    {
        tmpInt = (*pJSONconfSys)["POWER"]["radioMode"].as<int>();
        if (tmpInt != 0)
        {
            Serial.println(F("mode wifi"));
            (*pJSONconfSys)["POWER"]["radioMode"] = "0"; // mode wifi
        }
        else
        {
            Serial.println(F("mode BLE"));
            (*pJSONconfSys)["POWER"]["radioMode"] = "1"; // mode BLE
        }
        serializeJson(*pJSONconfSys, SDfile); // ecrit le contenu modifie
        // Fermer le fichier
        SDfile.close();
        delay(100);
        reset_all(); // reboot l'ESP
    }
}

void setNewDeviceID()
{
    int devId = 0;
    File SDfile = SD.open(CONFIGFILE, FILE_WRITE);
    // Vérifier si le fichier a été ouvert avec succès
    if (!SDfile)
    {
        Serial.println(F("Erreur d'ouverture du fichier de config"));
    }
    else
    {
        devId = (*pJSONconfSys)["HARDWARE"]["id"].as<int>();
        // Serial.print(F("old Id "));
        // Serial.println(devId);
        devId += 1;
        if (devId > 5)
        {
            devId = 0;
        }
        // Serial.print(F("new Id "));
        // Serial.println(devId);
        (*pJSONconfSys)["HARDWARE"]["id"] = String(devId); // nouvel Id

        serializeJson(*pJSONconfSys, SDfile); // ecrit le contenu modifie
        delay(10);
        // Fermer le fichier
        SDfile.close();
        delay(10);
        reset_all(); // reboot l'ESP
    }
}

std::string getBLEdeviceName()
{
    std::string Stmp = ((*pJSONconfSys)["BLE"]["deviceName"].as<std::string>() + "-" +
                        (*pJSONconfSys)["HARDWARE"]["id"].as<std::string>());
    return (Stmp);
}

std::string getBLEmanufacturer()
{
    std::string Stmp = (*pJSONconfSys)["BLE"]["deviceManufacturer"].as<std::string>();
    return (Stmp);
}

uint8_t getDeviceId()
{
    uint8_t mcAdd = (*pJSONconfSys)["HARDWARE"]["id"].as<uint8_t>();
    return (mcAdd);
}

std::string getAPssid()
{
    std::string Stmp = (*pJSONconfSys)["WIFI"]["ssid"].as<std::string>();
    return (Stmp);
}
std::string getAPpassword()
{
    std::string Stmp = (*pJSONconfSys)["WIFI"]["password"].as<std::string>();
    return (Stmp);
}

void startApp()
{
    Serial.print(F("----------> Envoi de la commande de START  "));
    int numCmd = (*pJSONconfKeys)["PROGRAM"]["cmd_run"].size();
    for (int i = 0; i < numCmd; i++)
    {
        Serial.print((*pJSONconfKeys)["PROGRAM"]["cmd_run"][i]["value"].as<String>());
        Serial.print(" ");
        String key = (*pJSONconfKeys)["PROGRAM"]["cmd_run"][i]["value"].as<String>();
        int type = (*pJSONconfKeys)["PROGRAM"]["cmd_run"][i]["type"].as<int>();
        sendKeyToBLE(key, type, 1); // envoi de la commande vers bluetooth dans BLEKeyboard
        delay(10);
    }
    sendKeyToBLE("0", 0, 3); // release all
    Serial.println("");
}

void stopApp()
{
    Serial.print(F("----------> Envoi de la commande de STOP  "));
    int numCmd = (*pJSONconfKeys)["PROGRAM"]["cmd_stop"].size();
    for (int i = 0; i < numCmd; i++)
    {
        Serial.print((*pJSONconfKeys)["PROGRAM"]["cmd_stop"][i]["value"].as<String>());
        Serial.print(" ");
        String key = (*pJSONconfKeys)["PROGRAM"]["cmd_stop"][i]["value"].as<String>();
        int type = (*pJSONconfKeys)["PROGRAM"]["cmd_stop"][i]["type"].as<int>();
        sendKeyToBLE(key, type, 1); // envoi de la commande vers bluetooth dans BLEKeyboard
        delay(10);
    }
    sendKeyToBLE("0", 0, 3); // release all
    Serial.println("");
}

void sendKey(int banque, int touche, int keyState)
{
    Serial.printf("banque: %d touche: %d keystate: %d\n", banque, touche, keyState);
    String banqueName = (*pJSONconfKeys)["BANKS"][banque]["name"].as<String>();
    Serial.print(F("Nom de la banque: "));
    Serial.println(banqueName);
    String keyName = (*pJSONconfKeys)["BANKS"][banque]["KEYS"][touche]["name"].as<String>();
    Serial.print(F("Nom de la touche: "));
    Serial.println(keyName);
    int numCmd = (*pJSONconfKeys)["BANKS"][banque]["KEYS"][touche]["commands"].size();
    Serial.print(F("nb de cmd: "));
    Serial.println(numCmd);
    Serial.println(F("------- commandes"));
    for (int i = 0; i < numCmd; i++) // boucle sur les commandes
    {
        String keyValue = "";
        keyValue = (*pJSONconfKeys)["BANKS"][banque]["KEYS"][touche]["commands"][i]["value"].as<String>();
        int keyType = (*pJSONconfKeys)["BANKS"][banque]["KEYS"][touche]["commands"][i]["type"].as<int>();
        Serial.print(F("    type: "));
        Serial.print(keyType);
        Serial.print(F("    valeur: "));
        Serial.println(keyValue);

        if (isConnectedBLE())
        {
            sendKeyToBLE(keyValue, keyType, keyState); // envoi de la commande vers bluetooth dans BLEKeyboard
            delay(10);
        }
        else
        {
            touche += 1;
            if (keyState)
            {
                //displayMsg(banqueName + "\n" + touche + "\n" + keyName + "\nT: " + keyType + " Nb: " + numCmd + " V: " + keyValue, 0, 0); // TODO    à reprendre avec l'affichage
                displayMsg(banqueName + "\n" + keyName + "\nT: " + keyType + " Nb: " + numCmd + " V: " + keyValue, 0, 0); // TODO    à reprendre avec l'affichage
                delay(400);
            }
        }
    }
    Serial.println("");
}
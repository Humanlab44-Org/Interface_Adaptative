/*
Nom du fichier : net.ccp
Auteur : Yann Cesbron  (yann@cesbron.org)
Date de création : 4 avril 2023
Licence :  Creative Commons BY-NC-SA
*/

#include "net.h"

// ------------------------- wifi ---------------------------------------------
void initWifi()
{
  Serial.println(F("-- Initialisation Wifi mode PA"));
  // ---------------  mode point d'acces
  ssid = getAPssid();
  passwd = getAPpassword();
  IPAddress apIP(192, 168, 4, 1);
  Serial.println(F("..."));
  Serial.print(F("Adresse IP du PA: "));
  Serial.println(apIP);
  // Serial.print(F("SSID: "));
  // Serial.println(ssid.c_str());
  // Serial.print(F("Password: "));
  // Serial.println(passwd.c_str());
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0)); // subnet FF FF FF 00
  WiFi.softAP(ssid.c_str(), passwd.c_str());

  initWebserver();
}

// ------------------------- webserver ---------------------------------------------
void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}
void uploadFileHandler(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
  if (!index)
  {
    String filePath = "/keysConfs/" + filename;
    uploadFile = SD.open(filePath, FILE_WRITE);
  }
  if (uploadFile)
  {
    for (size_t i = 0; i < len; i++)
    {
      uploadFile.write(data[i]);
    }
    if (final)
    {
      uploadFile.close();
      Serial.printf("Upload finished: %s, size: %u\n", filename.c_str(), index + len);
      String response = "{\"success\": true, \"message\": \"Le fichier a été ecrit avec succès\"}";
      request->send(200, "application/json", response);
    }
  }
}

void initWebserver()
{

  Serial.println(F("-- Initialisation Webserver"));
  // -------------------------------------- sauvegarde config ------------------------------
  AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/saveConfig", [](AsyncWebServerRequest *request, JsonVariant &json)
                                                                         {
  DynamicJsonDocument data(1024);
  if (json.is<JsonArray>())
  {
    data = json.as<JsonArray>();
  }
  else if (json.is<JsonObject>())
  {
    data = json.as<JsonObject>();
  }
  String response;
  serializeJson(data, response);    
    //request->send(200, "application/json", response);
    // Traitement des données JSON
    //   String deviceName = data["BLE"]["deviceName"].as<String>();
    //   Serial.print("reponse; ");
    //   Serial.println(response);
    //   Serial.print("deviceName: ");
    //   Serial.println(deviceName);
  if(saveJsonFile(CONFIGFILE, &data, true)){   // ecrit sur la carte
    request->send(200, "text/plain", "sauvegarde config OK ");
  }else{
    request->send(500, "text/plain", "ERREUR sauvegarde config");
  } });
  server.addHandler(handler);

  // -------------------------------------- sauvegarde keyconfig dans 'filename'.json ------------------------------
  AsyncCallbackJsonWebHandler *handler2 = new AsyncCallbackJsonWebHandler("/saveKeysConfig", [](AsyncWebServerRequest *request, JsonVariant &json)
                                                                          {
    String fileName = json["fileName"].as<String>();
    Serial.println("fichier: " + fileName);

    String jsonData;
    serializeJson((json["jsonData"]), jsonData);
    Serial.println(jsonData);

    DynamicJsonDocument doc(DYNAMIC_JSON_DOCUMENT_SIZE);
    deserializeJson(doc, jsonData);

    // // Write the file and JSON data
    // fileName = "/test.json";  // pour les tests
    if (saveJsonFile(fileName.c_str(), &doc, false))
    { // ecrit sur la carte
      request->send(200, "text/plain", "sauvegarde configKeys OK ");
    }
    else
    {
      request->send(500, "text/plain", "ERREUR sauvegarde configKeys");
    } });
  server.addHandler(handler2);

  // -------------------------------------- delete keyconfig  ------------------------------
  AsyncCallbackJsonWebHandler *handler3 = new AsyncCallbackJsonWebHandler("/deleteKeysConfigFile", [](AsyncWebServerRequest *request, JsonVariant &json)
                                                                          {
    String fileName = json["fileName"].as<String>();
    Serial.println("supression fichier: " + fileName);
    String filePath = "/keysConfs/" + fileName;
    if (SD.remove(filePath))
    {
      request->send(200, "text/plain", "supression configKeys OK ");
    }
    else
    {
      request->send(500, "text/plain", "ERREUR supression configKeys");
    } });
  server.addHandler(handler3);

  server.on(
      "/upload", HTTP_POST, [](AsyncWebServerRequest *request) {}, uploadFileHandler);

  // ------------------------------- test ---------------------------------------
  // GET request to <IP>/get?message=<message>
  // server.on("/gettest", HTTP_GET, [](AsyncWebServerRequest *request)
  //           {
  //     String message;
  //     if (request->hasParam(PARAM_MESSAGE)) {
  //         message = request->getParam(PARAM_MESSAGE)->value();
  //     } else {saveKeysConfig
  //         message = "No message sent";
  //     }
  //     request->send(200, "text/plain", "Hello, GET: " + message); });

  // --------------------------------- wifi-info -------------------------
  server.on("/wifi-info", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument json(1024);
    json["status"] = "ok";
    json["ssid"] = WiFi.SSID();
    json["ip"] = WiFi.localIP().toString();
    serializeJson(json, *response);
    request->send(response); });
  // ------------------------------------ list key files --------------------------------
  server.on("/listKeysFiles", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String response = getKeysConfFilesNames();
    // request->send(response); });
    //  request->send(["toto" : "titi"]); });
    request->send(200, "text/plain", response); });

  // -------------------------------------- divers --------------------------------------
  server.serveStatic("/src", SD, "/src");
  server.serveStatic("/img", SD, "/img");
  server.serveStatic("/", SD, "/");
  server.on("/test.htm", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", "<H1>HandiPad.. se porte bien</H1>"); });
  server.onNotFound(notFound);
  // ---------------------------------------  start web server ---------------------------
  server.begin();
  Serial.println("HTTP server started");
}

#include <ArduinoJson.h>
#include <WiFi.h>
#include <Update.h>
#include <WiFiMulti.h>
#include <nvs_flash.h>

extern AsyncEventSource events;
extern Preferences pref;
extern AppConfig appConfig;

uint8_t otaDone = 0;
size_t totalSize = 0;

String processorInfo(const String &var) {
  if (var == "TEMPLATE_MAC") {
    return WiFi.macAddress();

  } else if (var == "TEMPLATE_IP") {
    return WiFi.localIP().toString();

  } else if (var == "TEMPLATE_HOSTNAME") {
    return String(WiFi.getHostname());

  } else if (var == "TEMPLATE_RSSI") {
    return String(WiFi.RSSI());

  } else if (var == "TEMPLATE_VERSION") {
    return F(appConfig.version);

  } else if (var == "TEMPLATE_UPTIME") {
    unsigned long uptimeMillis = millis();

    unsigned long seconds = uptimeMillis / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    unsigned long days = hours / 24;

    seconds = seconds % 60;
    minutes = minutes % 60;
    hours = hours % 24;

    String uptime = String(days) + " days " + String(hours) + "h " + (minutes < 10 ? "0" : "") + String(minutes) + "min " + (seconds < 10 ? "0" : "") + String(seconds) + "s";
    return uptime;

  } else if (var == "TEMPLATE_LOCAL_TIME") {
    getLocalTime(&timeinfo);

    char timeStr[64];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);

    return String(timeStr) + " UTC";
  }

  return String();
}

String processorLogs(const String &var) {
  if (var == "LOG_TEMPLATE") {
    // check if logging is even active
    pref.begin("deviceSettings");
    bool logging = pref.getBool("logging", true);
    pref.end();

    if (!logging) {
      return "Logging is disabled!";
    }

    File logFile = LittleFS.open("/log.txt", "r");
    String logContent = "";
    if (logFile) {
      while (logFile.available()) {
        logContent += logFile.readStringUntil('\n') + "<br>";
      }
      logFile.close();
    } else {
      logContent = "Log file not found!";
    }
    return logContent;
  }

  // Return an empty string if the placeholder is unknown
  return String();
}


void handleUploadRestore(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (!index) {
    request->_tempFile = LittleFS.open("/" + filename, "w", true);
  }

  if (len) {
    request->_tempFile.write(data, len);
  }

  if (final) {
    request->_tempFile.close();

    File backupFile = LittleFS.open("/" + filename, "r");
    String backupContent = "";
    if (backupFile) {
      while (backupFile.available()) {
        backupContent += backupFile.readString();
      }
      backupFile.close();
    }

    JsonDocument doc;
    deserializeJson(doc, backupContent);


    // device settings
    JsonObject device = doc["device"];
    const char* name = device["name"];
    const bool wled = device["wled"];
    const uint8_t count = device["count"];
    const char* order = device["order"];
    const bool analog = device["analog"];
    const uint8_t mode = device["mode"];
    const bool sw = device["switch"];
    const uint8_t action = device["action"];
    const bool logging = device["logging"];

    pref.begin("deviceSettings");
    pref.putString("name", name);
    pref.putBool("wled", wled);
    pref.putInt("count", count);
    pref.putString("order", order);
    pref.putBool("analog", analog);
    pref.putInt("mode", mode);
    pref.putBool("sw", sw);
    pref.putInt("action", action);
    pref.putBool("logging", logging);
    pref.end();

    
    // printer settings
    JsonObject printer = doc["printer"];
    const char* ip = printer["ip"];
    const char* ac = printer["ac"];
    const char* sn = printer["sn"];
    const bool rtid = printer["rtid"];
    const uint8_t rtit = printer["rtit"];

    pref.begin("printerSettings");
    pref.putString("ip", ip);
    pref.putString("ac", ac);
    pref.putString("sn", sn);
    pref.putBool("rtid", rtid);
    pref.putInt("rtit", rtit);
    pref.end();

    // wifi settings
    JsonObject wifi = doc["wifi"];
    bool setup = wifi["setup"];
    const char *ssid = wifi["ssid"];
    const char *pw = wifi["pw"];

    pref.begin("wifi");
    pref.putBool("setup", true);
    pref.putString("ssid", ssid);
    pref.putString("pw", pw);
    pref.end();

    LittleFS.remove("/" + filename);
    delay(500);
    ESP.restart();
  }
}

void handleUploadOTA(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  logger("In handler", false);
  if (!index) {
    logger("Update Start: " + filename);
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
      Update.printError(Serial);
      request->send(500, "text/plain", "Update Failed: Could not begin OTA");
      return;
    }
    totalSize = 0;
  }

  if (!Update.hasError()) {
    if (Update.write(data, len) != len) {
      Update.printError(Serial);
    } else {
      totalSize += len;
      int progress = (totalSize * 100) / request->contentLength();
      events.send(String(progress).c_str(), "ota-progress", millis());
    }
  }

  if (final) {
    if (Update.end(true)) {
      String msg = "Update Success: " + String(index + len) + "B";
      logger(msg);
      events.send("100", "ota-progress", millis());

      delay(1000);
      ESP.restart();
    } else {
      Update.printError(Serial);
      request->send(500, "text/plain", "Update Failed: Could not finalize OTA");
    }
  }
}


void setupStaticRoutes(AsyncWebServer &server) {
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html").setFilter(ON_STA_FILTER);
  server.serveStatic("/", LittleFS, "/captive.html").setFilter(ON_AP_FILTER);

  // Main routes
  server.on("/info", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/info.html", String(), false, processorInfo);
  });

  server.on("/log", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/log.html", String(), false, processorLogs);
  });

  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/settings.html", String(), false);
  });

  server.on("/settings-device", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/settings-device.html", String(), false);
  });

  server.on("/settings-mapping", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/settings-mapping.html", String(), false);
  });

  server.on("/settings-printer", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/settings-printer.html", String(), false);
  });

  server.on("/settings-update", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/settings-update.html", String(), false);
  });

  server.on("/settings-wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/settings-wifi.html", String(), false);
  });
}


void setupSettingsRoutes(AsyncWebServer &server) {
  server.on("/api/settings-device", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    JsonDocument doc;
    doc["wled"] = appConfig.wled;
    doc["count"] = appConfig.count;
    doc["order"] = appConfig.order;
    doc["analog"] = appConfig.analog;
    doc["mode"] = appConfig.mode;
    doc["switch"] = appConfig.sw;
    doc["action"] = appConfig.action;
    doc["logging"] = appConfig.logging;
    serializeJson(doc, *response);
    request->send(response);
  });

  server.on("/api/settings-device", HTTP_POST, [](AsyncWebServerRequest *request) {
    pref.begin("deviceSettings");
    
    if (request->hasParam("wled", true)) {
      bool wled = request->getParam("wled", true)->value();
      pref.putBool("wled", wled);
    }

    if (request->hasParam("count", true)) {
      int count = request->getParam("count", true)->value().toInt();
      pref.putInt("count", count);
    }

    if (request->hasParam("order", true)) {
      String order = request->getParam("order", true)->value();
      pref.putString("order", order);
    }

    if (request->hasParam("analog", true)) {
      bool analog = request->getParam("analog", true)->value();
      pref.putBool("analog", analog);
    }

    if (request->hasParam("mode", true)) {
      int mode = request->getParam("mode", true)->value().toInt();
      pref.putInt("mode", mode);
    }

    if (request->hasParam("switch", true)) {
      bool sw = request->getParam("switch", true)->value();
      pref.putBool("sw", sw);
    }

    if (request->hasParam("action", true)) {
      int action = request->getParam("action", true)->value().toInt();
      pref.putInt("action", action);
    }

    if (request->hasParam("logging", true)) {
      bool logging = request->getParam("logging", true)->value();
      pref.putBool("logging", logging);
    }
    pref.end();

    request->send(200, "application/json", "{\"status\":\"saved\"}");
    ESP.restart();
  });

  server.on("/api/settings-printer", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    JsonDocument doc;
    doc["printerIp"] = appConfig.ip;
    doc["accessCode"] = appConfig.ac;
    doc["sn"] = appConfig.sn;
    doc["rtid"] = appConfig.rtid;
    doc["rtit"] = appConfig.rtit;

    serializeJson(doc, *response);
    request->send(response);
  });

  server.on("/api/settings-printer", HTTP_POST, [](AsyncWebServerRequest *request) {
    pref.begin("printerSettings");
    if (request->hasParam("ip", true)) {
      String printerIp = request->getParam("ip", true)->value();
      pref.putString("ip", printerIp);
    }

    if (request->hasParam("ac", true)) {
      String accessCode = request->getParam("ac", true)->value();
      pref.putString("ac", accessCode);
    }

    if (request->hasParam("sn", true)) {
      String sn = request->getParam("sn", true)->value();
      pref.putString("sn", sn);
    }

    if (request->hasParam("rtid", true)) {
      bool returnToIdleDoor = request->getParam("rtid", true)->value();
      pref.putBool("rtid", returnToIdleDoor);
    }

    if (request->hasParam("rtit", true)) {
      int returnToIdleTime = request->getParam("rtit", true)->value().toInt();
      pref.putInt("rtit", returnToIdleTime);
    }
    pref.end();

    request->send(200, "application/json", "{\"status\":\"saved\"}");
    ESP.restart();
  });

  server.on("/api/test-printer", HTTP_GET, [](AsyncWebServerRequest *request) {
    // connect to mqtt and wait for incoming report message

    request->send(200, "application/json", "{\"status\":\"success\"}");
  });

  server.on("/api/wifi", HTTP_POST, [](AsyncWebServerRequest *request) {
    String ssidNew;
    String pwNew;

    if (request->hasParam("ssid", true)) {
      ssidNew = request->getParam("ssid", true)->value();
    }

    if (request->hasParam("pw", true)) {
      pwNew = request->getParam("pw", true)->value();
    }

    WiFiMulti wifiMulti;
    wifiMulti.addAP(ssidNew.c_str(), pwNew.c_str());
    const unsigned long startTime = millis();
    unsigned long timeout = 10000;

    // test new connection
    logger("Testing new WiFi connection");
    while (wifiMulti.run() != WL_CONNECTED && millis() - startTime < timeout) {
      delay(500);
      Serial.print(".");
    }

    if (wifiMulti.run() == WL_CONNECTED) {
      logger("\nSuccessfully connected to the new WiFi network!");
      logger("New IP Address: " + WiFi.localIP().toString());

      pref.begin("wifi");
      pref.putString("ssid", ssidNew);
      pref.putString("pw", pwNew);
      pref.end();

      request->send(200, "application/json", "{\"status\":\"success\"}");
      delay(2000);
      ESP.restart();

    } else {
      logger("\nFailed to connect to the new WiFi network. Keeping current connection.");
      request->send(200, "application/json", "{\"status\":\"failed\"}");
    }
  });

  server.on("/api/wifi-scan", HTTP_GET, [](AsyncWebServerRequest *request) {
    int networkCount = WiFi.scanComplete();

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    JsonDocument doc;

    if (networkCount == -2) {
      WiFi.scanNetworks(true);
      doc["status"] = "scan_in_progress";
    } else if (networkCount >= 0) {
      JsonArray networks = doc["networks"].to<JsonArray>();

      for (int i = 0; i < networkCount; ++i) {
        JsonObject network = networks.add<JsonObject>();
        network["ssid"] = WiFi.SSID(i);
        network["rssi"] = WiFi.RSSI(i);
      }
      WiFi.scanDelete();
    } else {
      doc["status"] = "no_networks";
    }

    serializeJson(doc, *response);
    request->send(response);
  });
}

 
void setupMappingRoutes(AsyncWebServer &server) {
  server.on("/api/mappings", HTTP_GET, [](AsyncWebServerRequest *request) {

  });

  server.on("/api/mappings", HTTP_POST, [](AsyncWebServerRequest *request) {

  });

  server.on("/api/mappings", HTTP_DELETE, [](AsyncWebServerRequest *request) {
    if (request->hasParam("id")) {
      int id = request->getParam("id")->value().toInt();
    }
  });

  server.on("/api/mapping-upload", HTTP_POST, [](AsyncWebServerRequest *request) {

  });

  server.on("/api/mapping-download", HTTP_GET, [](AsyncWebServerRequest *request) {

  });

  server.on("/api/test-mapping", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("id")) {
      int id = request->getParam("id")->value().toInt();
    }
  });
}


void setupFileRoutes(AsyncWebServer &server) {
  server.on("/api/backup-download", HTTP_GET, [](AsyncWebServerRequest *request) {
    // wifi settings
    pref.begin("wifi", true);
    bool setup = pref.getBool("setup", false);
    String ssid = pref.getString("ssid", "");
    String pw = pref.getString("pw", "");
    pref.end();

    // TODO: generate json file for download
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    JsonDocument doc;
    JsonObject device = doc["device"].to<JsonObject>();
    device["wled"] = appConfig.wled;
    device["count"] = appConfig.count;
    device["order"] = appConfig.order;
    device["analog"] = appConfig.analog;
    device["mode"] = appConfig.mode;
    device["switch"] = appConfig.sw;
    device["action"] = appConfig.action;
    device["logging"] = appConfig.logging;

    JsonObject printer = doc["printer"].to<JsonObject>();
    printer["ip"] = appConfig.ip;
    printer["ac"] = appConfig.ac;
    printer["sn"] = appConfig.sn;
    printer["rtid"] = appConfig.rtid;
    printer["rtit"] = appConfig.rtit;

    JsonObject wifi = doc["wifi"].to<JsonObject>();
    wifi["setup"] = setup;
    wifi["ssid"] = ssid;
    wifi["pw"] = pw;

    serializeJson(doc, *response);
    response->addHeader("Content-Disposition", "attachment; filename=\"backup.blwled\"");
    request->send(response);
  });

  server.on("/api/backup-upload", HTTP_POST, [](AsyncWebServerRequest *request) { 
    request->send(200); 
  }, handleUploadRestore);

  server.on("/api/ota-upload", HTTP_POST, [](AsyncWebServerRequest *request) { 
    request->send(200);
  }, handleUploadOTA);
}


void setupApiRoutes(AsyncWebServer &server) {
   server.on("/api/log-download", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (LittleFS.exists("/log.txt")) {
      request->send(LittleFS, "/log.txt", "text/plain", true);
    } else {
      request->send(404, "text/plain", "Log file not found!");
    }
  });

  server.on("/api/log-delete", HTTP_GET, [](AsyncWebServerRequest *request) {
    LittleFS.remove("/log.txt");
    request->redirect("/log");
  });

  server.on("/api/reset", HTTP_POST, [](AsyncWebServerRequest *request) {
    logger("Reset by user");

    nvs_flash_erase();
    nvs_flash_init();

    request->redirect("/");
    delay(300);
    ESP.restart();
  });

  server.on("/api/reboot", HTTP_GET, [](AsyncWebServerRequest *request) {
    logger("Reboot by user");
    request->redirect("/");
    delay(100);
    ESP.restart();
  });
}


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "application/json", "{\"message\":\"Not found\"}");
}


// Routing here
void routing(AsyncWebServer &server) {

  server.addHandler(&events);

  setupStaticRoutes(server);
  setupMappingRoutes(server);
  setupSettingsRoutes(server);
  setupApiRoutes(server);
  setupFileRoutes(server);

  server.onNotFound(notFound);
}

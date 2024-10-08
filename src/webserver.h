#include <ESPAsyncWebserver.h>
#include <Preferences.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include "ArduinoJson.h"
#include "nvs_flash.h"

#include <log.h>

Preferences pref;

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
    return F("0.0.1-b");

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
    return F("to be added");
  }

  return String();
}

String processorLogs(const String &var) {
  if (var == "LOG_TEMPLATE") {
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

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "application/json", "{\"message\":\"Not found\"}");
}

void routing(AsyncWebServer &server) {
  // Serve static files from LittleFS
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

  // API routes
  server.on("/api/log-download", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (LittleFS.exists("/log.txt")) {
      request->send(LittleFS, "/log.txt", "text/plain", true);
    } else {
      request->send(404, "text/plain", "Log file not found!");
    }
  });

  server.on("/api/reset", HTTP_GET, [](AsyncWebServerRequest *request) {
    logger("Reset by user");

    nvs_flash_erase();
    nvs_flash_init();

    request->redirect("/");
    delay(100);
    ESP.restart();
  });

  server.on("/api/reboot", HTTP_GET, [](AsyncWebServerRequest *request) {
    logger("Reboot by user");
    request->redirect("/");
    delay(100);
    ESP.restart();
  });

  // WiFi SETTINGS
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
    unsigned long startTime = millis();
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

  // MAPPINGS
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

  // SETTINGS DEVICE
  server.on("/api/settings-device", HTTP_GET, [](AsyncWebServerRequest *request) {
    pref.begin("deviceSettings");
    bool wled = pref.getBool("wled", false);
    int count = pref.getInt("count", 0);
    String order = pref.getString("order", "gbr");

    bool analog = pref.getBool("analog", false);
    String mode = pref.getString("mode", "strip");

    bool sw = pref.getBool("sw", false);
    String fnct = pref.getString("function", "event");
    pref.end();

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    JsonDocument doc;
    doc["wled"] = wled;
    doc["count"] = count;
    doc["order"] = order;
    doc["analog"] = analog;
    doc["mode"] = mode;
    doc["switch"] = sw;
    doc["function"] = fnct;
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
      bool returnToIdleDoor = request->getParam("order", true)->value();
      pref.putBool("order", returnToIdleDoor);
    }

    if (request->hasParam("analog", true)) {
      bool analog = request->getParam("analog", true)->value();
      pref.putBool("analog", analog);
    }

    if (request->hasParam("mode", true)) {
      String mode = request->getParam("mode", true)->value();
      pref.putString("mode", mode);
    }

    if (request->hasParam("switch", true)) {
      bool sw = request->getParam("switch", true)->value();
      pref.putBool("sw", sw);
    }

    if (request->hasParam("function", true)) {
      String fnct = request->getParam("function", true)->value();
      pref.putString("function", fnct);
    }

    pref.end();
    request->send(200, "application/json", "{\"status\":\"saved\"}");
  });

  // SETTINGS PRINTER
  server.on("/api/settings-printer", HTTP_GET, [](AsyncWebServerRequest *request) {
    pref.begin("printerSettings");
    String printerIp = pref.getString("ip", "");
    String accessCode = pref.getString("ac", "");
    String sn = pref.getString("sn", "");
    bool returnToIdleDoor = pref.getBool("rtid", true);
    int returnToIdleTime = pref.getInt("rtit", 10);
    pref.end();

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    JsonDocument doc;
    doc["printerIp"] = printerIp;
    doc["accessCode"] = accessCode;
    doc["sn"] = sn;
    doc["rtid"] = returnToIdleDoor;
    doc["rtit"] = returnToIdleTime;
    doc["test"] = ESP.getFreeHeap();

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
  });

  server.on("/api/test-printer", HTTP_GET, [](AsyncWebServerRequest *request) {
    // check printer mqtt for info msg

    pref.begin("printerSettings");
    String printerIp = pref.getString("ip", "");
    String accessCode = pref.getString("ac", "");
    String sn = pref.getString("sn", "");
    pref.end();

    request->send(200, "application/json", "{\"status\":\"success\"}");
  });

  // BACKUP & OTA
  server.on("/api/backup-download", HTTP_GET, [](AsyncWebServerRequest *request) {
    // device settings
    pref.begin("deviceSettings");
    bool wled = pref.getBool("wled", false);
    int count = pref.getInt("count", 0);
    String order = pref.getString("order", "gbr");

    bool analog = pref.getBool("analog", false);
    String mode = pref.getString("mode", "strip");

    bool sw = pref.getBool("sw", false);
    String fnct = pref.getString("function", "event");
    pref.end();

    // printer settings
    pref.begin("printerSettings");
    String printerIp = pref.getString("ip", "");
    String accessCode = pref.getString("ac", "");
    String sn = pref.getString("sn", "");
    bool returnToIdleDoor = pref.getBool("rtid", true);
    int returnToIdleTime = pref.getInt("rtit", 10);
    pref.end();

    // wifi settings
    pref.begin("wifi");
    bool setup = pref.getBool("setup", false);
    String ssid = pref.getString("ssid", "");
    String pw = pref.getString("pw", "");
    pref.end();

    // TODO: generate json file for download
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    JsonDocument doc;
    JsonObject device = doc["device"].to<JsonObject>();
    device["wled"] = wled;
    device["count"] = count;
    device["order"] = order;
    device["analog"] = analog;
    device["mode"] = mode;
    device["switch"] = sw;
    device["function"] = fnct;

    JsonObject printer = doc["printer"].to<JsonObject>();
    printer["ip"] = printerIp;
    printer["ac"] = accessCode;
    printer["sn"] = sn;
    printer["rtid"] = returnToIdleDoor;
    printer["rtit"] = returnToIdleTime;
  
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
    // receive ota file and process
  });
  
  server.onNotFound(notFound);
}

void handleUploadRestore(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {

  if (!index) {
    request->_tempFile = LittleFS.open("/" + filename, "w");
  }

  if (len) {
    request->_tempFile.write(data, len);
  }

  if (final) {
    request->_tempFile.close();
    request->redirect("/");

    delay(200);
  	ESP.restart();
  }  
}
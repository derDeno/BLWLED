#include <ESPAsyncWebserver.h>
#include <LITTLEFS.h>
#include <WiFi.h>

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

        String uptime = String(days) + " days, " + String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes) + ":" + (seconds < 10 ? "0" : "") + String(seconds);
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
                logContent += logFile.readStringUntil('\n');
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
    request->send(404, "text/plain", "Not found");
}

void routing(AsyncWebServer &server) {
    // Serve static files from LittleFS
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html").setFilter(ON_STA_FILTER);
    server.serveStatic("/", LittleFS, "/captive.html").setFilter(ON_AP_FILTER);

    //server.serveStatic("/info", LittleFS, "/info.html").setTemplateProcessor(processorInfo);

    server.on("/info", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/info.html", String(), false, processorInfo);
    });

    // Serve a route for fetching the text file content
    server.on("/logs.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/logs.html", String(), false, processorLogs);
    });

    server.onNotFound(notFound);
}
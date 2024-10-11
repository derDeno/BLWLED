#include <LITTLEFS.h>

void logger(String logData) {
  Serial.println(logData);
  delay(10);

  File logFile = LittleFS.open("/log.txt", "a");
  if (!logFile) {
    Serial.println("Failed to open log file");
    return;
  }

  logFile.println(logData);
  logFile.close();
}
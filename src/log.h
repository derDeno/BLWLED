#include <LITTLEFS.h>

void logger(String logData, bool file = true) {
  Serial.println(logData);
  delay(10);

  if (!file) {
    return;
  }
  
  File logFile = LittleFS.open("/log.txt", "a");
  if (!logFile) {
    Serial.println("Failed to open log file");
    return;
  }

  logFile.println(logData);
  logFile.close();
}
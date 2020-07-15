#include "maintenance/Logger.h"

void Logger::log(const char *type, const char *message) {
  // TODO: [in future] add persistence to SD cart storage, and add
  // synchronization time with NTP server
  unsigned long timestamp = millis();
  String logMessage = String("[");
  logMessage += type;
  logMessage += ":";
  logMessage += timestamp;
  logMessage += "]: ";
  logMessage += message;
  Serial.println(logMessage);
}

void Logger::info(const char *message) {
  Logger::log("LOG", message);
}

void Logger::warning(const char *message) {
  Logger::log("WARNING", message);
}

void Logger::error(const char *message) {
  Logger::log("ERROR", message);
}

void Logger::debug(const char *message) {
  Logger::log("DEBUG", message);
}

void Logger::log(const char *type, const String *message) {
  // TODO: [in future] add persistence to SD cart storage, and add
  // synchronization time with NTP server
  unsigned long timestamp = millis();
  String logMessage = String("[");
  logMessage += type;
  logMessage += ":";
  logMessage += timestamp;
  logMessage += "]: ";
  logMessage.concat(*message);
  Serial.println(logMessage);
}

<<<<<<< HEAD
void Logger::info(const String *message) { Logger::log("LOG", message); }

void Logger::warning(const String *message) { Logger::log("WARNING", message); }

void Logger::error(const String *message) { Logger::log("ERROR", message); }

void Logger::debug(const String *message) { Logger::log("DEBUG", message); }
=======
void Logger::info(const String *message) {
  Logger::log("LOG", message);
}

void Logger::warning(const String *message) {
  Logger::log("WARNING", message);
}

void Logger::error(const String *message) {
  Logger::log("ERROR", message);
}

void Logger::debug(const String *message) {
  Logger::log("DEBUG", message);
}
>>>>>>> 18a98c1c5272cefd284419dcb963eca526f4ce11

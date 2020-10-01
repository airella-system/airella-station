#include "maintenance/Logger.h"

HardwareSerial Logger::serial = HardwareSerial(0);

void Logger::setUp() {
  serial.begin(115200,SERIAL_8N1,3,1,false,1000); 
}

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
  serial.println(logMessage);
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

void Logger::log(const char *type, const String& message) {
  // TODO: [in future] add persistence to SD cart storage, and add
  // synchronization time with NTP server
  unsigned long timestamp = millis();
  String logMessage = String("[");
  logMessage += type;
  logMessage += ":";
  logMessage += timestamp;
  logMessage += "]: ";
  logMessage += message;
  serial.println(logMessage);
}

void Logger::info(const String *message) {
  Logger::log("LOG", *message);
}

void Logger::warning(const String *message) {
  Logger::log("WARNING", *message);
}

void Logger::error(const String *message) {
  Logger::log("ERROR", *message);
}

void Logger::debug(const String *message) {
  Logger::log("DEBUG", *message);
}

void Logger::info(const String& message) {
  Logger::log("LOG", message);
}

void Logger::warning(const String& message) {
  Logger::log("WARNING", message);
}

void Logger::error(const String& message) {
  Logger::log("ERROR", message);
}

void Logger::debug(const String& message) {
  Logger::log("DEBUG", message);
}

void Logger::info(String& message) {
  Logger::log("LOG", message);
}

void Logger::warning(String& message) {
  Logger::log("WARNING", message);
}

void Logger::error(String& message) {
  Logger::log("ERROR", message);
}

void Logger::debug(String& message) {
  Logger::log("DEBUG", message);
}
#include "maintenance/Logger.h"
#include "device/DeviceContainer.h"

HardwareSerial Logger::serial = HardwareSerial(0);
DataPersister *Logger::logPersister = nullptr;

void Logger::setUp() {
  serial.begin(115200, SERIAL_8N1, 3, 1, false, 1000);
  Logger::logPersister = new DataPersister();
}

void Logger::log(const char *type, const char *message) {
  String logMessage = String("[");
  logMessage += type;
  logMessage += ":";
  logMessage += timeProvider.getDataTime().toString();
  logMessage += "]: ";
  logMessage += message;
  serial.println(logMessage);
  #ifdef PERSIST_LOGS
  logPersister->saveLog(logMessage);
  #endif
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
  String logMessage = String("[");
  logMessage += type;
  logMessage += ":";
  logMessage += timeProvider.getDataTime().toString();
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

void Logger::info(const String &message) {
  Logger::log("LOG", message);
}

void Logger::warning(const String &message) {
  Logger::log("WARNING", message);
}

void Logger::error(const String &message) {
  Logger::log("ERROR", message);
}

void Logger::debug(const String &message) {
  Logger::log("DEBUG", message);
}

void Logger::info(String &message) {
  Logger::log("LOG", message);
}

void Logger::warning(String &message) {
  Logger::log("WARNING", message);
}

void Logger::error(String &message) {
  Logger::log("ERROR", message);
}

void Logger::debug(String &message) {
  Logger::log("DEBUG", message);
}

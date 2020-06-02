#include "maintenance/Logger.h"

void Logger::log(const char *type, const char *message)
{
    //TODO: [in future] add persistence to SD cart storage, and add synchronization time with NTP server
    unsigned long timestamp = millis();
    String logMessage = String("[");
    logMessage += type;
    logMessage += ":";
    logMessage += timestamp;
    logMessage += "]: ";
    logMessage += message; 
    Serial.println(logMessage);
}

void Logger::info(const char *message)
{
    Logger::log("LOG", message);
}

void Logger::warning(const char *message)
{
    Logger::log("WARNING", message);
}

void Logger::error(const char *message)
{
    Logger::log("ERROR", message);
}

void Logger::debug(const char *message)
{
    Logger::log("DEBUG", message);
}

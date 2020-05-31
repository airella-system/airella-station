#include "maintaince/Logger.h"

void Logger::log(const char *type, const char *message)
{
    String logMessage = String("[");
    logMessage += type;
    logMessage += ":<date_time>]: "; // TODO
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

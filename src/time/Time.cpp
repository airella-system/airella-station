#include "time/Time.h"
#include "config/Config.h"

Time::Time() : timeClient(ntpUDP) {
  semaphore = xSemaphoreCreateMutex();
}

void Time::lock() {
  xSemaphoreTake(semaphore, portMAX_DELAY);
}

void Time::unlock() {
  xSemaphoreGive(semaphore);
}

bool Time::isInitialized() const {
  return initialized;
}

void Time::connect() {
  Logger::info("[Time::connect()] Try to connect to NTP server");
  if (initialized) {
    Logger::warning("[Time::connect()] Already connect to NTP server");
    return;
  }

  lock();
  if (WiFi.status() != WL_CONNECTED) {
    initialized = false;
    unlock();
    Logger::debug("[Time::connect()] Unable to connect to NTP server, reasone: no internet connection");
    return;
  }

  timeClient.begin();
  initialized = true;
  unlock();
  Logger::debug("[Time::connect()] Initialized OK");
}

void Time::update() {
  lock();
  timeClient.setTimeOffset(0);
  timeClient.update();
  lastRefreshTimestamp = millis();
  unlock();
  Logger::debug("[Time::connect()] Synchronized time with NTP server");
}

Date_t Time::getDate() {
  tm* timeInfo = getTimeInfo();

  return Date_t(timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday);
}

Time_t Time::getTime() {
  tm* timeInfo = getTimeInfo();

  return Time_t(timeInfo->tm_hour + TIMEZONE_HOUR_SHIFT, timeInfo->tm_min, timeInfo->tm_sec);
}

DateTime_t Time::getDataTime() {
  tm* timeInfo = getTimeInfo();

  Date_t date(timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday);
  Time_t time(timeInfo->tm_hour + TIMEZONE_HOUR_SHIFT, timeInfo->tm_min, timeInfo->tm_sec);

  return DateTime_t(date, time);
}

tm* Time::getTimeInfo() {
  time_t totalSecunds = timeClient.getEpochTime();
  // Logger::serial.println("pies______________________");
  // Logger::serial.println(String((long)totalSecunds));
  return localtime(&totalSecunds);
}

void Time::persistTime() {
  Config::setPersistedTime(timeClient.getEpochTime());
  lastTimeOfPersist = millis();
}

void Time::loadPersistedTime() {
  timeClient.setTimeOffset(Config::getPersistedTime());
}

bool Time::shouldBePersist() {
  return abs(millis() - lastTimeOfPersist) > 1000 * 60 * 30;
}

Time timeProvider;

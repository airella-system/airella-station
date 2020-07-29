#include "time/Time.h"

Time::Time() : timeClient(ntpUDP) {
  connect();
  update();
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
  if(initialized) return;

  lock();
  if(WiFi.status() != WL_CONNECTED) {
    initialized = false;
  }

  timeClient.begin();
  initialized = true;
  unlock();
}

void Time::update() {
  lock();
  timeClient.update();
  timestamp = millis();
  unlock();
}

unsigned int Time::getYear() {
  
}

unsigned int Time::getMonth() {
  
}

unsigned int Time::getDay() {
  
}

unsigned int Time::getHour() {
  
}

unsigned int Time::getMinute() {

}

unsigned int Time::getSecond() {
  
}

Date_t Time::getDate() {
  
}

Time_t Time::getTime() {
  
}

DateTime_t Time::getDataTime() {
  
}

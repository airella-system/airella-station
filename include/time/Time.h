#pragma once

#include <Arduino.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <time.h>
#include "maintenance/Logger.h"

#define TIMEZONE_HOUR_SHIFT 0

struct Date_t {
  unsigned int year;
  unsigned int month;
  unsigned int day;

  Date_t(){};
  Date_t(unsigned int _year, unsigned int _month, unsigned int _day) : year(_year), month(_month), day(_day){};

  const String toString() {
    String monthStr = month < 10 ? String("0") + month : String(month);
    String dayStr = day < 10 ? String("0") + day : String(day);

    return String(year) + "-" + monthStr + "-" + dayStr;
  }
};

struct Time_t {
  unsigned int hour;
  unsigned int minute;
  unsigned int second;

  Time_t(){};
  Time_t(unsigned int _hour, unsigned int _minute, unsigned int _second)
      : hour(_hour), minute(_minute), second(_second){};

  const String toString() {
    String hourStr = hour < 10 ? "0" + hour : String(hour, DEC);
    String minuteStr = minute < 10 ? String("0") + minute : String(minute, DEC);
    String secondStr = second < 10 ? String("0") + second : String(second, DEC);

    return hourStr + ":" + minuteStr + ":" + secondStr;
  }
};

struct DateTime_t {
  Date_t date;
  Time_t time;

  DateTime_t(){};
  DateTime_t(Date_t &_date, Time_t &_time) : date(_date), time(_time){};

  const String toString() {
    return date.toString() + " " + time.toString();
  }

  const String toISOString() {
    return date.toString() + "T" + time.toString() + "Z";
  }
};

class Time {
 public:
  Time();
  ~Time(){};

  bool isInitialized() const;
  void connect();
  void update();
  Date_t getDate();
  Time_t getTime();
  DateTime_t getDataTime();
  unsigned long lastTimeOfPersist = 0;
  void persistTime();
  void loadPersistedTime();
  bool shouldBePersist() const;

 private:
  WiFiUDP ntpUDP;
  NTPClient timeClient;
  bool initialized = false;
  unsigned long lastRefreshTimestamp;
  long persistedTime;
  unsigned long lastTimestamp = 0;
  tm *getTimeInfo();

  SemaphoreHandle_t semaphore;
  void lock();
  void unlock();
};

extern Time timeProvider;

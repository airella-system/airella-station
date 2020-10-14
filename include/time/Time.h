#pragma once

#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFi.h>
#include <time.h>
#include "maintenance/Logger.h"

#define TIMEZONE_HOUR_SHIFT 2

struct Date_t {
  unsigned int year;
  unsigned int month;
  unsigned int day;

  Date_t() {};
  Date_t(unsigned int _year, unsigned int _month, unsigned int _day) 
    : year(_year), month(_month), day(_day)
  {};
  
  String toString() {
    String monthStr = month < 10 ? String("0") + month : String(month);
    String dayStr = day < 10 ? String("0") + day : String(day);

    return String(year) + "-" + monthStr + "-" + dayStr;
  }
};

struct Time_t {
  unsigned int hour;
  unsigned int minute;
  unsigned int second;

  Time_t() {};
  Time_t(unsigned int _hour, unsigned int _minute, unsigned int _second) 
    : hour(_hour), minute(_minute), second(_second)
  {};

  String toString() {
    String hourStr = hour < 10 ? "0" + hour : String(hour);
    String minuteStr = minute < 10 ? String("0") + minute : String(minute);
    String secondStr = second < 10 ? String("0") + second : String(second);

    return hourStr + ":" + minuteStr + ":" + secondStr;
  }
};

struct DateTime_t {
  Date_t date;
  Time_t time;

  DateTime_t() {};
  DateTime_t(Date_t &_date, Time_t &_time)
    : date(_date), time(_time) 
  {};

  String toString() {
    return date.toString() + " " + time.toString();
  }

  String toISOString() {
    return date.toString() + time.toString() + "Z";
  }
};

class Time {

public:
  Time();
  ~Time() {};
  
  bool isInitialized() const;
  void connect();
  void update();
  Date_t getDate();
  Time_t getTime();
  DateTime_t getDataTime();

private:
  WiFiUDP ntpUDP;
  NTPClient timeClient;
  bool initialized;
  unsigned long lastRefreshTimestamp;
  tm* getTimeInfo();

  SemaphoreHandle_t semaphore;
  void lock();
  void unlock();

};

extern Time timeProvider;

#pragma once

#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFi.h>

struct Date_t {
  unsigned int year;
  unsigned int month;
  unsigned int day;

  Date_t() {};
  Date_t(unsigned int _year, unsigned int _month, unsigned int _day) 
    : year(_year), month(_month), day(_day)
  {};
  
  String toString() {
    return String(year) + "-" + String(month) + "-" + String(day);
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
    return String(hour) + ":" + String(minute) + ":" + String(second);
  }
};

struct DateTime_t {
  Date_t date;
  Time_t time;

  DateTime_t() {};
  DateTime_t(Date_t _date, Time_t _time)
    : date(_date), time(_time) 
  {};

  String toString() {
    return date.toString() + " " + time.toString();
  }
};

class Time {

public:
  bool isInitialized() const;
  void connect();
  void update();
  unsigned int getYear();
  unsigned int getMonth();
  unsigned int getDay();
  unsigned int getHour();
  unsigned int getMinute();
  unsigned int getSecond();
  Date_t getDate();
  Time_t getTime();
  DateTime_t getDataTime();

private:
  Time();
  ~Time() {};

  WiFiUDP ntpUDP;
  NTPClient timeClient;
  bool initialized;
  unsigned long timestamp;

  SemaphoreHandle_t semaphore;
  void lock();
  void unlock();

};

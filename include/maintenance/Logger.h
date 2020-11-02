#pragma once

#include <Arduino.h>
#include "time/Time.h"
#include "config/Defines.h"

class DataPersister;

class Logger {
 private:
  static void log(const char *type, const char *message);
  static void log(const char *type, const String &message);
  static DataPersister logPersister;

 public:
  static HardwareSerial serial;
  static void setUp();
  static void info(const char *message);
  static void warning(const char *message);
  static void error(const char *message);
  static void debug(const char *message);

  static void info(const String *message);
  static void warning(const String *message);
  static void error(const String *message);
  static void debug(const String *message);

  static void info(const String &message);
  static void warning(const String &message);
  static void error(const String &message);
  static void debug(const String &message);

  static void info(String &message);
  static void warning(String &message);
  static void error(String &message);
  static void debug(String &message);
};

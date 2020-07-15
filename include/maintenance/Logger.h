#pragma once
#include <Arduino.h>

class Logger {
 private:
  static void log(const char *type, const char *message);
  static void log(const char *type, const String *message);

 public:
  static void info(const char *message);
  static void warning(const char *message);
  static void error(const char *message);
  static void debug(const char *message);

  static void info(const String *message);
  static void warning(const String *message);
  static void error(const String *message);
  static void debug(const String *message);
};

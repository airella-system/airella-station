#pragma once
#include <Arduino.h>

class Logger
{
private:
	static void log(const char *type, const char *message);
public:
	static void info(const char *message);
	static void warning(const char *message);
	static void error(const char *message);
	static void debug(const char *message);
};

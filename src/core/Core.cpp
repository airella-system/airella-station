#include "core/Core.h"

Core::Core() {
  Logger::info("[Core]: Iniatlizing...");

  airSensor = new AirSensor();
  weatherSensor = new WeatherSensor();
  powerSensor = new PowerSensor();
  heater = new Heater();
  storage = new Storage();

  Logger::info("[Core]: Iniatlized OK");
}

void Core::setUp() {
  // TODO: main program setup here
}

void Core::loop() {
  // TODO: main program logic here
}
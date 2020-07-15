#include "core/Core.h"

Core::Core() {
  Logger::info("[Core]: Iniatlizing...");

  // airSensor = new AirSensor();
  // weatherSensor = new WeatherSensor();
  // powerSensor = new PowerSensor();
  // heater = new Heater();
  // storage = new Storage();

  Logger::info("[Core]: Iniatlized OK");
}

void Core::setUp() {
  Logger::info("[Core]: Setting up started");

  Config::load();
  BluetoothRefreshHandler* handler = new BluetoothRefreshHandler();
  Bluetooth::start(handler);
  Internet::setType(Internet::WIFI);
  Internet::start();

  Logger::info("[Core]: Setting up ended, run main thread");
}

void Core::loop() {
  
}

Core core;

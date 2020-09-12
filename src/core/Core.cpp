#include "core/Core.h"

// #define STOP_MAIN_LOOP

Core::Core() {
  Logger::setUp();
  Logger::info("[Core]: Iniatlizing...");
  // powerSensor = new PowerSensor();
  // storage = new Storage();

  Logger::info("[Core]: Iniatlized OK");
}

void Core::setUp() {
  Logger::info("[Core]: Setting up started");

  Config::load();
  Logger::info("[Core]: Loaded preferences.");

  Bluetooth::start(new BluetoothRefreshHandler());
  Internet::setType(Internet::WIFI);
  Internet::start();

  if (WiFi.status() == WL_CONNECTED) {
    // crash while internet is not connected
    timeProvider.connect();
    timeProvider.update();
  }

  airAndGpsSensorStrategy = new AirAndGpsSensorStrategy();
  weatherSensor = new WeatherSensor();
  heater = new Heater(*weatherSensor);
  heater->run();

  Logger::info("[Core]: Setting up ended");
}

void Core::main() {
  if (!Api.isRegistered()) {
    Logger::info("[Core]: Wait for registrations.");
    while (!Api.isRegistered()) {
      delay(1000);
    }
  }

#ifdef STOP_MAIN_LOOP
  while (true) {
    delay(1000);
  }
#endif

  while (true) {
    if (abs(millis() - lastPublishMillis) > 10000) {
      Logger::info("[Core]: Start measurement");

      Api.publishMeasurement(measurementType.temperature, weatherSensor->getTemperature());
      Api.publishMeasurement(measurementType.humidity, weatherSensor->getHumidity());
      Api.publishMeasurement(measurementType.pressure, weatherSensor->getPressure());
      airAndGpsSensorStrategy->getAirSensor()->measurement();
      Api.publishMeasurement(measurementType.pm1, airAndGpsSensorStrategy->getAirSensor()->getPM1());
      Api.publishMeasurement(measurementType.pm2_5, airAndGpsSensorStrategy->getAirSensor()->getPM2_5());
      Api.publishMeasurement(measurementType.pm10, airAndGpsSensorStrategy->getAirSensor()->getPM10());

      lastPublishMillis = millis();
    }

    if (abs(millis() - lastGpsUpdateMillis) > 60000) {
      Logger::info("[Core]: Start switch to GPS");
      airAndGpsSensorStrategy->switchToGpsSensor();

      if (airAndGpsSensorStrategy->getGpsSensor()->fetchLocation()) {
        Api.publishLocation(airAndGpsSensorStrategy->getGpsSensor()->getLatitude(),
                            airAndGpsSensorStrategy->getGpsSensor()->getLongitude());
      }
      airAndGpsSensorStrategy->switchToAirSensor();

      lastGpsUpdateMillis = millis();
      Logger::info("[Core]: End switch to GPS");
    }
    delay(1000);
  }
}

Core core;

#include "core/Core.h"
#include "communication/gsm/GsmConn.h"
// #define STOP_MAIN_LOOP

Core::Core() { }

void Core::setUp() {
  Logger::setUp();
  Logger::info("[Core]: Setting up started");
  powerSensor = new PowerSensor();
  powerSensor->begin();
  // storage = new Storage();
  DeviceContainer.powerSensor = powerSensor;

  Logger::info("[Core]: Iniatlized OK");

  Config::load();
  Logger::info("[Core]: Loaded preferences.");

  Bluetooth::start(new BluetoothRefreshHandler());
  Internet::setType(Internet::WIFI);

  if (WiFi.status() == WL_CONNECTED) {
    timeProvider.connect();
    timeProvider.update();
  }

  airAndGpsSensorStrategy = new AirAndGpsSensorStrategy();
  weatherSensor = new WeatherSensor();
  heater = new Heater(*weatherSensor);
  heater->run();

  DeviceContainer.airSensor = airAndGpsSensorStrategy->getAirSensor();
  DeviceContainer.weatherSensor = weatherSensor;
  DeviceContainer.heater = heater;

  Logger::info("[Core]: Setting up ended");
}

void Core::main() {
  isWorking = true;
  if (!Api.isRegistered()) {
    Logger::info("[Core]: Wait for registrations.");
    while (!Api.isRegistered()) {
      delay(10000);
    }
  } else {
    Statistics.reportBootUp();
  }

#ifdef STOP_MAIN_LOOP
  while (true) {
    GsmConn::gsm.httpGetRequest();
    Http::Response res = Internet::httpGet("https://airella.cyfrogen.com/api/stations/JzZJMbouBINp?strategy=latest");
    Logger::debug("JASNA PALA: " + res.payload);
    delay(30000);
  }
#endif
  
  while(isWorking) {
    Guardian::statistics();
    
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

void Core::reset() {
  isWorking = false;
}

Core core;

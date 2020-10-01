#include "core/Core.h"

// #define STOP_MAIN_LOOP

Core::Core() {

}

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
  Internet::start();

  if(WiFi.status() == WL_CONNECTED) {
    // crash while internet is not connected
    timeProvider.connect();
    timeProvider.update();
  }

  airSensor = new AirSensor();
  airSensor->powerOn();
  airSensor->calibrate();
  weatherSensor = new WeatherSensor();
  heater = new Heater(*weatherSensor);
  heater->run();

  DeviceContainer.airSensor = airSensor;
  DeviceContainer.weatherSensor = weatherSensor;
  DeviceContainer.heater = heater;

  Logger::info("[Core]: Setting up ended");
}

void Core::main() {

  if(!Api.isRegistered()) {
    Logger::info("[Core]: Wait for registrations.");
    while(!Api.isRegistered()) {
      delay(1000);
    }
  } else {
    Statistics.reportBootUp();
  }
  
#ifdef STOP_MAIN_LOOP
  while(true) {
    delay(1000);
  }
#endif
  
  while(true) {
    Guardian::statistics();
    
    if (abs(millis() - lastPublishMillis) > 10000) {
      Logger::info("[Core]: Start measurement");

      Api.publishMeasurement(measurementType.temperature, weatherSensor->getTemperature());
      Api.publishMeasurement(measurementType.humidity, weatherSensor->getHumidity());
      Api.publishMeasurement(measurementType.pressure, weatherSensor->getPressure());
      airSensor->measurement();
      Api.publishMeasurement(measurementType.pm1, airSensor->getPM1());
      Api.publishMeasurement(measurementType.pm2_5, airSensor->getPM2_5());
      Api.publishMeasurement(measurementType.pm10, airSensor->getPM10());

      lastPublishMillis = millis();
    }
    delay(10000);
  }
}

Core core;

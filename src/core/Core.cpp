#include "core/Core.h"

// #define STATIC_CONFIG
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

  #ifdef STATIC_CONFIG
  Config::setWifiSsid("default");
  Config::setApiUrl("http://airella.cyfrogen.com/api");
  Config::setRegistratonToken("03cdfeca-ddb2-4856-b69e-0b0ceaf10113");
  Config::setStationName("Mleko");
  Config::setAddressCity("Slopnice");
  Config::setAddressCountry("Poland");
  Config::setAddressStreet("Slopnice");
  Config::setAddressNumber("123");
  Config::save();
  #endif

  Bluetooth::start(new BluetoothRefreshHandler());
  Internet::setType(Internet::WIFI);
  Internet::start();

  airSensor = new AirSensor();
  airSensor->powerOn();
  airSensor->calibrate();
  weatherSensor = new WeatherSensor();
  heater = new Heater(*weatherSensor);
  heater->run();

  #ifdef STATIC_CONFIG
  Api.configUpdated();
  Api.publishName("Mleko");
  Api.publishAddress("Poland", "Slopnice", "Slopnice", "123");
  Api.publishLocation(49.713481, 20.339463);
  #endif

  Logger::info("[Core]: Setting up ended");
}

void Core::main() {
  
#ifdef STOP_MAIN_LOOP
  while(true) delay(1000);
#endif

  if(!Api.isRegistered()) {
    Logger::info("[Core]: Wait for registrations.");
    while(!Api.isRegistered()) {
      delay(1000);
    }
  }
  
  while(true) {
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

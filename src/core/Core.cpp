#include "core/Core.h"

Core::Core() {
  Logger::setUp();
  Logger::info("[Core]: Iniatlizing...");
  // powerSensor = new PowerSensor();
  // heater = new Heater();
  // storage = new Storage();

  Logger::info("[Core]: Iniatlized OK");
}

void Core::setUp() {
  Logger::info("[Core]: Setting up started");

  Config::load();
  // Config::setWifiSsid("default"); //static config
  Config::save();

  Bluetooth::start(new BluetoothRefreshHandler());
  // Internet::setType(Internet::WIFI);
  // Internet::start();

  // airSensor = new AirSensor();
  // airSensor->powerOn();
  // airSensor->calibrate();
  // weatherSensor = new WeatherSensor();

  if(false) { //static config
    Config::setRegistratonToken("dbb2c782-9947-44fc-9f0c-44bf2c8223e6");
    Config::setStationName("Mleko");
    Config::setAddressCity("Slopnice");
    Config::setAddressCountry("Poland");
    Config::setAddressStreet("Slopnice");
    Config::setAddressNumber("123");
    Config::save();
    Api.publishName("Mleko");
    Api.publishAddress("Poland", "Slopnice", "Slopnice", "123");
    Api.publishLocation(49.713481, 20.339463);
    Api.configUpdated();
  }
  // Logger::info("[Core]: Setting up ended, run main thread");
}

void Core::loop() {
  // todo: obsłożyć overflow
  // if ((millis() - lastPublishMillis) > 10000) {
  //   Logger::info("[Core]: Start measurement");

  //   Api.publishMeasurement(measurementType.temperature, weatherSensor->getTemperature());
  //   Api.publishMeasurement(measurementType.humidity, weatherSensor->getHumidity());
  //   airSensor->measurement();
  //   Api.publishMeasurement(measurementType.pm1, airSensor->getPM1());
  //   Api.publishMeasurement(measurementType.pm2_5, airSensor->getPM2_5());
  //   Api.publishMeasurement(measurementType.pm10, airSensor->getPM10());

  //   lastPublishMillis = millis();
  // }
  delay(10000);
}

Core core;

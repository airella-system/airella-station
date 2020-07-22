#include "core/Core.h"

// #define STATIC_CONFIG

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

  #ifdef STATIC_CONFIG
  Api.configUpdated();
  Api.publishName("Mleko");
  Api.publishAddress("Poland", "Slopnice", "Slopnice", "123");
  Api.publishLocation(49.713481, 20.339463);
  #endif

  Logger::info("[Core]: Setting up ended");
}

// TODO: oganąć uniwersalny handler do BT i zapisywanie stanu
// TODO: ogarnać kiedy pytać o autoryzację, bo teraz pytało o autoryzację przy rejestracji czyli bez sensu
// TODO: co się stanie jeśli wszsytkie albo część preferencji się skasuje
// TODO: logowanie błędów w measurement
// TODO: logowanie błędów w klasie od neta
// TODO: dorobić obsługę błędów, klasę Troubleshooting
// TODO: klasa api tam trzeba dodać logi i autoryzacje ogarnać
// TODO: dorobić temporary config i dopiero potem flushować
// TODO: dorobić wysyłanie lokalizacji przy rejestracji
// TODO: co w sytuacji gdy preferences nie istnieje a ja chce je załątować
void Core::main() {
  // todo: obsłożyć overflow
  while(true) {
    if ((millis() - lastPublishMillis) > 10000) {
      Logger::info("[Core]: Start measurement");

      Api.publishMeasurement(measurementType.temperature, weatherSensor->getTemperature());
      Api.publishMeasurement(measurementType.humidity, weatherSensor->getHumidity());
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

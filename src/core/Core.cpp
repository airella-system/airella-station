#include "core/Core.h"
#include "communication/gsm/GsmConn.h"
// #define STOP_MAIN_LOOP

Core::Core() {}

void Core::setUp() {
  Logger::setUp();
  Logger::info("[Core]: Setting up started");
  taskHandler = new TaskHandler<void*, double, String>(1);
  powerSensor = new PowerSensor();
  powerSensor->begin();
  // storage = new Storage();
  DeviceContainer.powerSensor = powerSensor;

  Logger::info("[Core]: Iniatlized OK");

  Config::load();
  Logger::info("[Core]: Loaded preferences.");

  Bluetooth::start(new BluetoothRefreshHandler(), this->taskHandler);
  Internet::resetType(Config::getInternetConnectionType() == Config::WIFI ? Internet::WIFI : Internet::GSM);

  if (WiFi.status() == WL_CONNECTED) {
    timeProvider.connect();
    timeProvider.update();
  }

  airAndGpsSensorStrategy = new AirAndGpsSensorStrategy();
  weatherSensor = new WeatherSensor();
  heater = new Heater(*weatherSensor);
  heater->run();

  DeviceContainer.airSensor = airAndGpsSensorStrategy->getAirSensor();
  DeviceContainer.gpsSensor = airAndGpsSensorStrategy->getGpsSensor();
  DeviceContainer.weatherSensor = weatherSensor;
  DeviceContainer.heater = heater;

  Logger::info("[Core]: Setting up ended");
}

void Core::doCoreTasks() {
  taskHandler->runPendingTasks(nullptr);
}

void Core::main() {
  isWorking = true;
  if (!Api.isRegistered()) {
    Logger::info("[Core]: Wait for registrations.");
    while (!Api.isRegistered()) {
      doCoreTasks();
      delay(100);
    }
  } else {
    Statistics.reportBootUp();
  }

  error = false;

#ifdef STOP_MAIN_LOOP
  while (true) {
    GsmConn::gsm.httpGetRequest();
    Http::Response res = Internet::httpGet("https://airella.cyfrogen.com/api/stations/JzZJMbouBINp?strategy=latest");
    delay(30000);
  }
#endif

  while (isWorking) {
    doCoreTasks();
    bool notErrorInIteration = true;
    Guardian::statistics();

    if (error) {
      if (abs(millis() - lastErrorMillis) > 60000) {
        error = false;
      }
    }

    if (abs(millis() - lastPublishMillis) > 10000) {
      Logger::info("[Core]: Start measurement");

      notErrorInIteration &= Api.publishMeasurement(measurementType.temperature, weatherSensor->getTemperature());
      notErrorInIteration &= Api.publishMeasurement(measurementType.humidity, weatherSensor->getHumidity());
      notErrorInIteration &= Api.publishMeasurement(measurementType.pressure, weatherSensor->getPressure());
      airAndGpsSensorStrategy->getAirSensor()->measurement();
      notErrorInIteration &=
          Api.publishMeasurement(measurementType.pm1, airAndGpsSensorStrategy->getAirSensor()->getPM1());
      notErrorInIteration &=
          Api.publishMeasurement(measurementType.pm2_5, airAndGpsSensorStrategy->getAirSensor()->getPM2_5());
      notErrorInIteration &=
          Api.publishMeasurement(measurementType.pm10, airAndGpsSensorStrategy->getAirSensor()->getPM10());

      lastPublishMillis = millis();
    }

    if (abs(millis() - lastGpsUpdateMillis) > 60000) {
      Logger::info("[Core]: Start switch to GPS");
      airAndGpsSensorStrategy->switchToGpsSensor();

      if (airAndGpsSensorStrategy->getGpsSensor()->fetchLocation()) {
        notErrorInIteration &= Api.publishLocation(airAndGpsSensorStrategy->getGpsSensor()->getLatitude(),
                                                   airAndGpsSensorStrategy->getGpsSensor()->getLongitude());
      }
      airAndGpsSensorStrategy->switchToAirSensor();

      lastGpsUpdateMillis = millis();
      Logger::info("[Core]: End switch to GPS");
    }

    if (!notErrorInIteration) {
      lastErrorMillis = millis();
      error = true;
    }
    delay(100);
  }
}

bool Core::isError() {
  return this->error;
}

void Core::reset() {
  isWorking = false;
}

Core core;

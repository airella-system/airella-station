#include "core/Core.h"
#include "communication/gsm/GsmConn.h"
// #define STOP_MAIN_LOOP

void Core::setUp() {
  Logger::setUp();
  Logger::info("[Core]: Setting up started");
  powerSensor = new PowerSensor();
  powerSensor->begin();
  DeviceContainer.powerSensor = powerSensor;
  storage = new Storage();
  DeviceContainer.storage = storage;

  Logger::info("[Core]: Iniatlized OK");

  Config::load();
  Logger::info("[Core]: Loaded preferences.");

  Bluetooth::start(new BluetoothRefreshHandler());
  Internet::setType(Internet::WIFI);

  if (WiFi.status() == WL_CONNECTED) {
    timeProvider.connect();
    timeProvider.update();
    timeProvider.persistTime();
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
    delay(30000);
  }
#endif
  
  while(isWorking) {
    Guardian::statistics();

    sendMeasurements();

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

    if(timeProvider.shouldBePersist()) {
      timeProvider.persistTime();
    }
    delay(1000);
  }
}

void Core::reset() {
  isWorking = false;
}

void Core::sendMeasurements() {
  if (abs(millis() - lastPublishMillis) > 10000) {
    Logger::info("[Core]: Start measurement");

    float value = weatherSensor->getTemperature();
    if(!Api.publishMeasurement(measurementType.temperature, value)) {
      storableBuffer.push(measurementType.temperature, String(value));
    }
    value = weatherSensor->getHumidity();
    if(!Api.publishMeasurement(measurementType.humidity, value)) {
      storableBuffer.push(measurementType.humidity, String(value));
    }
    value = weatherSensor->getPressure();
    if(!Api.publishMeasurement(measurementType.pressure, value)) {
      storableBuffer.push(measurementType.pressure, String(value));
    }
    airAndGpsSensorStrategy->getAirSensor()->measurement();

    uint16_t pmValue = airAndGpsSensorStrategy->getAirSensor()->getPM1();
    if(!Api.publishMeasurement(measurementType.pm1, pmValue)) {
      storableBuffer.push(measurementType.pm1, String(value));
    }
    pmValue = airAndGpsSensorStrategy->getAirSensor()->getPM2_5();
    if(!Api.publishMeasurement(measurementType.pm2_5, pmValue)) {
      storableBuffer.push(measurementType.pm2_5, String(value));
    }
    pmValue = airAndGpsSensorStrategy->getAirSensor()->getPM10();
    if(!Api.publishMeasurement(measurementType.pm10, pmValue)) {
      storableBuffer.push(measurementType.pm10, String(value));
    }

    lastPublishMillis = millis();
  }
  storableBuffer.sync();
}

Core core;

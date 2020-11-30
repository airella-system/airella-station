#include "core/Core.h"
#include "communication/gsm/GsmConn.h"
// #define STOP_MAIN_LOOP

void Core::setUp() {
  timeProvider.loadPersistedTime();
  Logger::setUp();
  Logger::info("[Core]: Setting up started");
  taskHandler = new TaskHandler<void*, double, String>(1);
  DeviceContainer.storage->tryToInit();

  powerSensor = new PowerSensor();
  powerSensor->begin();
  DeviceContainer.powerSensor = powerSensor;

  Logger::info("[Core]: Iniatlized OK");
  Config::load();
  Autocofiguration autocofiguration;
  autocofiguration.tryConfig();
  Logger::info("[Core]: Loaded preferences.");

  Bluetooth::start(new BluetoothRefreshHandler(), this->taskHandler);
  Internet::resetType(Config::getInternetConnectionType() == Config::WIFI ? Internet::WIFI : Internet::GSM);

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

  gpsError = false;
  sensorError = true;

#ifdef STOP_MAIN_LOOP
  while (true) {
    delay(30000);
  }
#endif

  while (isWorking) {
    doCoreTasks();
    Guardian::statistics();
    Guardian::check();

    if (timeProvider.shouldBePersist()) {
      timeProvider.persistTime();
    }

    sendMeasurements();
    sendGpsLocation();

    delay(100);
  }
}

bool Core::isError() {
  return this->gpsError || this->sensorError;
}

void Core::reset() {
  isWorking = false;
}

void Core::sendGpsLocation() {
  if (abs(millis() - lastGpsUpdateMillis) > 60000) {
    Logger::info("[Core]: Start switch to GPS");
    airAndGpsSensorStrategy->switchToGpsSensor();

    if (airAndGpsSensorStrategy->getGpsSensor()->fetchLocation()) {
      bool success = Api.publishLocation(airAndGpsSensorStrategy->getGpsSensor()->getLatitude(),
                                                  airAndGpsSensorStrategy->getGpsSensor()->getLongitude());
      this->gpsError = !success;
    }
    airAndGpsSensorStrategy->switchToAirSensor();
    lastGpsUpdateMillis = millis();
    Logger::info("[Core]: End switch to GPS");
  }
}

void Core::sendMeasurements() {
  int state = 0;
  if (abs(millis() - lastPublishMillis) > 10000) {
    Logger::info("[Core]: Start measurement");
    state = 1;

    float value = Statistics.calcTemperature();
    if(!Api.publishMeasurement(measurementType.temperature, value)) {
      storableBuffer.push(measurementType.temperature, String(value));
      state = -1;
    }
    value = Statistics.calcHumidity();
    if(!Api.publishMeasurement(measurementType.humidity, value)) {
      storableBuffer.push(measurementType.humidity, String(value));
      state = -1;
    }
    value = Statistics.calcPressure();
    if(!Api.publishMeasurement(measurementType.pressure, value)) {
      storableBuffer.push(measurementType.pressure, String(value));
      state = -1;
    }
    airAndGpsSensorStrategy->getAirSensor()->measurement();

    uint16_t pmValue = airAndGpsSensorStrategy->getAirSensor()->getPM1();
    if(!Api.publishMeasurement(measurementType.pm1, pmValue)) {
      storableBuffer.push(measurementType.pm1, String(value));
      state = -1;
    }
    pmValue = airAndGpsSensorStrategy->getAirSensor()->getPM2_5();
    if(!Api.publishMeasurement(measurementType.pm2_5, pmValue)) {
      storableBuffer.push(measurementType.pm2_5, String(value));
      state = -1;
    }
    pmValue = airAndGpsSensorStrategy->getAirSensor()->getPM10();
    if(!Api.publishMeasurement(measurementType.pm10, pmValue)) {
      storableBuffer.push(measurementType.pm10, String(value));
      state = -1;
    }

    lastPublishMillis = millis();
    Guardian::tryFlushBuffers();
  }
  storableBuffer.sync();
  if (state == -1) {
    this->sensorError = true;
  } else if (state == 1) {
    this->sensorError = false;
  }
}

Core core;

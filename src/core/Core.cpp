#include "core/Core.h"
#include "communication/gsm/GsmConn.h"
// #define STOP_MAIN_LOOP

void Core::setUp() {
  timeProvider.loadPersistedTime();
  Logger::setUp();
  Logger::info("[Core]: Setting up started");
  taskHandler = new TaskHandler<void*, double, String>(1);
  DeviceContainer.storage->tryToInit();
  Guardian::measurePersister = &measurementPersister;
  
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
    Logger::debug("bobobobo");
  } else {
    Statistics.reportBootUp();
  }
  
#ifdef STOP_MAIN_LOOP
  while (true) {
    delay(30000);
  }
#endif

  while (isWorking) {
    doCoreTasks();
    if (abs(millis() - lastCheckMillis) > 10000) {
      DataModel dataModel;
      Guardian::statistics(dataModel);
      doMeasurements(dataModel);
      checkGpsLocation(dataModel);

      if(dataModel.containsData) {
        String body;
        serializeJson(dataModel.doc, body);
        if(!Api.publishDataModel(body)) {
          storableBuffer.push(dataModel);
        }
      }

      Guardian::tryFlushBuffers();
      storableBuffer.sync();

      if (timeProvider.shouldBePersist()) timeProvider.persistTime();
      lastCheckMillis = millis();
    }
    
    Guardian::check();
    delay(100);
  }
}

void Core::reset() {
  isWorking = false;
}

void Core::checkGpsLocation(DataModel& dataModel) {
  if (abs(millis() - lastGpsUpdateMillis) > 1000 * 60) {
    Logger::info("[Core]: Start switch to GPS");
    airAndGpsSensorStrategy->switchToGpsSensor();
    doCoreTasks();
    if (airAndGpsSensorStrategy->getGpsSensor()->fetchLocation()) {
      dataModel.setLocation(
        airAndGpsSensorStrategy->getGpsSensor()->getLatitude(),
        airAndGpsSensorStrategy->getGpsSensor()->getLongitude()
      );
    }
    doCoreTasks();
    airAndGpsSensorStrategy->switchToAirSensor();
    lastGpsUpdateMillis = millis();
    Logger::info("[Core]: End switch to GPS");
    doCoreTasks();
  }
}

void Core::doMeasurements(DataModel& dataModel) {
  if (abs(millis() - lastPublishMillis) > 10000) {
    Logger::info("[Core]: Start measurement");

    float value = Statistics.calcTemperature();
    dataModel.addMeasurement(measurementType.temperature, value);
    measurementPersister.saveMeasurement(measurementType.temperature, value);

    value = Statistics.calcHumidity();
    dataModel.addMeasurement(measurementType.humidity, value);
    measurementPersister.saveMeasurement(measurementType.humidity, value);

    value = Statistics.calcPressure();
    dataModel.addMeasurement(measurementType.pressure, value);
    measurementPersister.saveMeasurement(measurementType.pressure, value);

    airAndGpsSensorStrategy->getAirSensor()->measurement();
    uint16_t pmValue = airAndGpsSensorStrategy->getAirSensor()->getPM1();
    dataModel.addMeasurement(measurementType.pm1, pmValue);
    measurementPersister.saveMeasurement(measurementType.pm1, pmValue);

    pmValue = airAndGpsSensorStrategy->getAirSensor()->getPM2_5();
    dataModel.addMeasurement(measurementType.pm2_5, pmValue);
    measurementPersister.saveMeasurement(measurementType.pm2_5, pmValue);

    pmValue = airAndGpsSensorStrategy->getAirSensor()->getPM10();
    dataModel.addMeasurement(measurementType.pm10, pmValue);
    measurementPersister.saveMeasurement(measurementType.pm10, pmValue);

    lastPublishMillis = millis();
  }
}

Core core;

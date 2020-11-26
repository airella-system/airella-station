#include "device/Heater.h"
#include "maintenance/Statistics.h"

Heater::Heater(WeatherSensor &_weatherSensor) : communicationBus(config.oneWirePin), weatherSensor(_weatherSensor) {
  Logger::info("[Heater] Initalizing ...");
  setTextState("HEATER|INIT");
  this->mux = xSemaphoreCreateMutex();

  if (!weatherSensor.isInit()) {
    Logger::error("[Heater] Weather sensor is uninitialized or null.");
    setTextState("HEATER|INIT_ERROR");
    return;
  }

  String message;
  analogWriteFrequency(config.analogPin, 1000);
  analogWrite(config.analogPin, 0);
  heaterStatus.heaterIsOn = false;
  heaterStatus.threadIsRunning = false;
  currentPower = 0;
  analogWrite(config.analogPin, 0);

  thermometer = DallasTemperature(&communicationBus);
  /*
   * This section is a little strange, but i haven't idea why this does not work with single begin
   */
  thermometer.begin();
  delay(2000);
  thermometer.begin();
  delay(1000);

  if (thermometer.isParasitePowerMode()) {
    Logger::info("[Heater] Parasite power is: ON");
  } else {
    Logger::info("[Heater] Parasite power is: OFF");
  }

  thermometer.requestTemperatures();
  numberOfDevices = thermometer.getDeviceCount();
  Logger::info("[Heater] Number of device: " + numberOfDevices);
  if (numberOfDevices == 0) {
    Logger::info("[Heater] Can't find thermometer");
    setTextState("HEATER|INIT_ERROR");
    return;
  }

  thermometer.requestTemperatures();

  bool foundAnyDevice = false;
  for (int i = 0; i < numberOfDevices; i++) {
    if (thermometer.getAddress(deviceAddress[i], i)) {
      message = "[Heater] Found device " + i;
      message += " with address: ";
      message += deviceAddressToString(deviceAddress[i]);
      Logger::info(&message);
      foundAnyDevice = true;
    } else {
      message = "[Heater] Found ghost device at " + i;
      message += " but could not detect address. Check power and cabling";
      Logger::warning(&message);
    }

    Logger::info("[Heater] Resolution: " + thermometer.getResolution(deviceAddress[i]));
  }

  initialized = foundAnyDevice;
  Logger::info("[Heater] initialized: OK");
  setTextState("HEATER|OK");
}

Heater::~Heater() {
  if (heaterStatus.threadIsRunning) {
    stop();
  }
}

void Heater::on() {
  if (!initialized) {
    Logger::warning("[Heater::on] Unable to turn on Heater because exist hardware error");
    return;
  }
  heaterStatus.heaterIsOn = true;
  currentPower = 50;
  analogWrite(config.analogPin, 50);
  Logger::info("[Heater::on()] Heater is ON");
}

void Heater::setPower(int power) {
  if (!initialized) {
    Logger::warning("[Heater::setPower] Unable to set power of Heater because exist hardware error");
    return;
  }
  heaterStatus.heaterIsOn = true;
  currentPower = power;
  analogWrite(config.analogPin, power);
}

void Heater::off() {
  if (!initialized) {
    Logger::warning("[Heater::off] Unable to turn off Heater because exist hardware error");
    return;
  }
  heaterStatus.heaterIsOn = false;
  analogWrite(config.analogPin, 0);
  Logger::info("[Heater::off()] Heater is OFF");
}

void Heater::run() {
  if (!initialized) {
    Logger::warning("[Heater::run] Unable to run Heater because exist hardware error");
    return;
  }
  heaterStatus.threadIsRunning = true;
  // xTaskCreatePinnedToCore(TaskFunction_t, Name, StackSize, void *pvParameters, Priority, TaskHandle_t, xCoreID)
  xTaskCreatePinnedToCore(this->threadFunction, "termoThread", 10000, this, 1, &termoThreadHandler, 1);
  Logger::info("[Heater] run heater thread");
}

#ifdef CALCULATE_DEWPOINT
void Heater::threadFunction(void *pvParameters) {
  Heater *heater = (Heater *)pvParameters;
  unsigned int intervalMax = 1000 * 10;  // 10s
  unsigned int interval = intervalMax;
  unsigned long lastTimestamp = millis() - 2 * interval;  // make first interation instant
  bool heaterIsOn = heater->getHeaterState().heaterIsOn;
  bool heaterLastState = !heaterIsOn;
  int counter = 0;
  int wrongMeasurements = 0;

  while (true) {
    if (abs(millis() - lastTimestamp) > interval) {
      lastTimestamp = millis();

      float temperature = heater->getTemperature();
      // Air sensor works well in temperature less than 60 degress
      // Let's stop heater when temperature is higher than 50 degress
      if (heaterIsOn && temperature > 50) {
        Logger::info("[Heater] Too hot! Stopping heater");
        heater->setPower(0);
        heater->off();
        heaterIsOn = false;
        interval = intervalMax;
        continue;
      }

      if (temperature < -100) { 
        // Dallas library returns -127 on error but we are not sure about comparing floating variables
        // (and practically you can't use station bellow 100 degress)
        wrongMeasurements++;
        if (heaterIsOn && wrongMeasurements >= 3) {
          Logger::info("[Heater] Got 3 wrong temperature measurements... Stopping heater");
          heater->setPower(0);
          heater->off();
          heaterIsOn = false;
          interval = intervalMax;
        }
        continue;
      }

      wrongMeasurements = 0;

      float humidity = heater->getHumidity();
      float outsideTemperature = heater->getOutsideTemperature();
      float dewPoint = heater->calculateDewPoint(humidity, outsideTemperature);
      float temperatureLevel = dewPoint + 5;

      if (!heaterIsOn && temperature < temperatureLevel - 0.5) {
        heater->setPower(MAX_POWER);
        heaterIsOn = true;
        Logger::info("[Heater] Power is ON");
        interval = intervalMax;
      } else if (heaterIsOn && temperature < temperatureLevel - 0.5) {
        heater->setPower(MAX_POWER);
        interval = intervalMax;
      } else if (heaterIsOn && abs(temperatureLevel - temperature) <= 0.5) {
        int power = MAX_POWER * (temperatureLevel + 0.5 - temperature);
        heater->setPower(power);
        interval = intervalMax / 5;
      } else if (heaterIsOn && temperature > temperatureLevel + 0.5) {
        heater->setPower(0);
        heater->off();
        heaterIsOn = false;
        interval = intervalMax;
      }

      heater->setReport(temperature, humidity, dewPoint, heater->getCurrentPower(), heaterIsOn);

      counter++;
      if (counter % 10 == 0) {
        String message = "[Heater]: ";
        message += "T: ";
        message += temperature;
        message += ", H: ";
        message += humidity;
        message += ", Dp: ";
        message += dewPoint;
        message += ", P: ";
        message += heater->getCurrentPower();
        message += ", state: ";
        message += heaterIsOn;
        Logger::debug(&message);
        counter = 1;
        if (heaterLastState != heaterIsOn) {
          heaterLastState = heaterIsOn;
        }
      }
    } else {
      delay(interval);
    }
  }
}
#else
void Heater::threadFunction(void *pvParameters) {
  Heater *heater = (Heater *)pvParameters;
  unsigned long lastTimestamp = millis();
  unsigned int interval = 1000 * 10;  // 10s
  bool heaterIsOn = heater->getHeaterState().heaterIsOn;
  float currentTemperature = 0;
  int counter = 0;

  while (true) {
    if (abs(millis() - lastTimestamp) > interval) {
      lastTimestamp = millis();
      currentTemperature = heater->getTemperature();

      if (!heaterIsOn && currentTemperature < heater->temperatureLevel &&
          abs(currentTemperature - heater->temperatureLevel) > heater->temperatureTrashold) {
        heater->on();
        heaterIsOn = true;
      }
      if (heaterIsOn && currentTemperature > heater->temperatureLevel &&
          abs(currentTemperature - heater->temperatureLevel) > heater->temperatureTrashold) {
        heater->off();
        heaterIsOn = false;
      }
    } else {
      delay(interval / 2);
    }

    counter++;
    if (counter % 10 == 0) {
      String message = "[Heater]: ";
      message += "T: ";
      message += heater->getTemperature();
      message += ", Tc: ";
      message += currentTemperature;
      message += ", state: ";
      message += heaterIsOn;
      Logger::debug(&message);
      counter = 1;
    }
  }
}
#endif

void Heater::stop() {
  heaterStatus.threadIsRunning = false;
  vTaskDelete(&termoThreadHandler);
  Logger::info("[Heater] stopped heater thread");
}

float Heater::getTemperature() {
  thermometer.requestTemperatures();
  for (int i = 0; i < numberOfDevices; i++) {
    temperatureDevice[i] = thermometer.getTempC(deviceAddress[i]);
  }

  return temperatureDevice[0];
}

HeaterStatus Heater::getHeaterState() const {
  return heaterStatus;
}

String Heater::deviceAddressToString(DeviceAddress deviceAddress) const {
  String str = "";
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) str += String(0, HEX);
    str += String(deviceAddress[i], HEX);
  }
  return str;
}

float Heater::getHumidity() {
  return Statistics.calcHumidity();
}

float Heater::getOutsideTemperature() {
  return Statistics.calcTemperature();
}

float Heater::calculateDewPoint(float humidity, float temperature) const {
  /*
   * Equations courtesy of Brian McNoldy from http://andrew.rsmas.miami.edu
   * Based on https://github.com/finitespace/BME280/blob/master/src/EnvironmentCalculations.cpp
   */
  return 243.04 * (log(humidity / 100.0) + ((17.625 * temperature) / (243.04 + temperature))) /
         (17.625 - log(humidity / 100.0) - ((17.625 * temperature) / (243.04 + temperature)));
}

unsigned int Heater::getCurrentPower() const {
  return currentPower;
}

void Heater::lock() const {
  xSemaphoreTake(Heater::mux, portMAX_DELAY);
}

void Heater::unlock() const {
  xSemaphoreGive(Heater::mux);
}

void Heater::setReport(float temperature, float humidity, float dewPoint, float currentPower, bool isOn) {
  lock();
  lastReport.temperature = temperature;
  lastReport.humidity = humidity;
  lastReport.dewPoint = dewPoint;
  lastReport.currentPower = currentPower;
  lastReport.isOn = isOn;
  unlock();
}

HeaterReport Heater::getReport() const {
  lock();
  HeaterReport report;
  report.temperature = lastReport.temperature;
  report.humidity = lastReport.humidity;
  report.dewPoint = lastReport.dewPoint;
  report.currentPower = lastReport.currentPower;
  report.isOn = lastReport.isOn;
  unlock();
  return report;
}

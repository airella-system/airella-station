#include "device/Heater.h"

Heater::Heater(WeatherSensor &_weatherSensor) 
  : communicationBus(config.oneWirePin), weatherSensor(_weatherSensor) {
  Logger::info("[Heater] Initalizing ...");
  
  if(!weatherSensor.isInit()) {
    Logger::error("[Heater] Weather sensor is uninitialized or null.");
    return;
  }

  String message;
  analogWriteFrequency(config.analogPin, 1000);
  analogWrite(config.analogPin, 0);
  heaterStatus.heaterIsOn = false;
  heaterStatus.threadIsRunning = false;

  thermometer = DallasTemperature(&communicationBus);
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
  thermometer.requestTemperatures();

  for (int i = 0; i < numberOfDevices; i++) {
    if (thermometer.getAddress(deviceAddress[i], i)) {
      message = "[Heater] Found device " + i;
      message += " with address: ";
      message += deviceAddressToString(deviceAddress[i]);
      Logger::info(&message);
    } else {
      message = "[Heater] Found ghost device at " + i;
      message += " but could not detect address. Check power and cabling";
      Logger::warning(&message);
    }

    Logger::info("[Heater] Resolution: " + thermometer.getResolution(deviceAddress[i]));
  }

  initialized = true;
  Logger::info("[Heater] initialized: OK");
}

Heater::~Heater() {
  if (heaterStatus.threadIsRunning) {
    stop();
  }
}

void Heater::on() {
  heaterStatus.heaterIsOn = true;
  analogWrite(config.analogPin, 50);
  Logger::info("Heater is ON");
}

void Heater::setPower(int power) {
  heaterStatus.heaterIsOn = true;
  analogWrite(config.analogPin, power);
  Logger::info("Heater is ON");
}

void Heater::off() {
  heaterStatus.heaterIsOn = false;
  analogWrite(config.analogPin, 0);
  Logger::info("Heater is OFF");
}

void Heater::run() {
  heaterStatus.threadIsRunning = true;
  // xTaskCreatePinnedToCore(TaskFunction_t, Name, StackSize, void *pvParameters, Priority, TaskHandle_t, xCoreID)
  xTaskCreatePinnedToCore(this->threadFunction, "termoThread", 10000, this, 1, &termoThreadHandler, 1);
  Logger::info("[Heater] run heater thread");
}

#ifdef CALCULATE_DEWPOIN
void Heater::threadFunction(void *pvParameters) {
  Heater *heater = (Heater *)pvParameters;
  unsigned long lastTimestamp = millis();
  unsigned int interval = 1000 * 10; // 10s
  bool heaterIsOn = heater->getHeaterState().heaterIsOn;

  while (true) {
    if (abs(millis() - lastTimestamp) > interval) {
      lastTimestamp = millis();

      float temperature = heater->getTemperature();
      float humidity = heater->getHumidity();
      float dewPoint = heater->dewPoint(humidity, temperature);
      float temperatureLevel = dewPoint + 5;

      if (!heaterIsOn && temperature < temperatureLevel) {
        heater->on();
        heaterIsOn = true;
        heater->heaterStatus.heaterIsOn = heaterIsOn;
      }
      if (heaterIsOn && temperature > temperatureLevel &&
          abs(temperature - temperatureLevel) > heater->temperatureTrashold) {
        heater->off();
        heaterIsOn = false;
        heater->heaterStatus.heaterIsOn = heaterIsOn;
      }
    } else {
      delay(interval/2);
    }
  }
}
#else
void Heater::threadFunction(void *pvParameters) {
  Heater *heater = (Heater *)pvParameters;
  unsigned long lastTimestamp = millis();
  unsigned int interval = 1000 * 10; // 10s
  bool heaterIsOn = heater->getHeaterState().heaterIsOn;
  float currentTemperature = 0;

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
      delay(interval/2);
    }

    String tmp = "CT: ";
    tmp += currentTemperature;
    tmp += " state: ";
    tmp += heaterIsOn;
    Logger::debug(&tmp);

  }
}
#endif

void Heater::stop() {
  heaterStatus.threadIsRunning = false;
  vTaskDelete(&termoThreadHandler);
  Logger::info("[Heater] stopped heater thread");
}

float Heater::getTemperature() {
  for (int i = 0; i < numberOfDevices; i++) {
    temperatureDevice[i] = thermometer.getTempC(deviceAddress[i]);
  }

  thermometer.setWaitForConversion(false);
  thermometer.requestTemperatures();
  return temperatureDevice[0];
}

HeaterStatus Heater::getHeaterState() const {
  return heaterStatus;
}

String Heater::deviceAddressToString(DeviceAddress deviceAddress) const {
  String str = "";
  for (uint8_t i = 0; i < 8; i++){
    if( deviceAddress[i] < 16 ) str += String(0, HEX);
    str += String(deviceAddress[i], HEX);
  }
  return str;
}

float Heater::getHumidity() {
  return weatherSensor.getHumidity();
}

float Heater::dewPoint(float humidity, float temperature) const {
  return 243.04 * (log(humidity / 100.0) + ((17.625 * temperature) / (243.04 + temperature))) 
         / (17.625 - log(humidity / 100.0) - ((17.625 * temperature) / (243.04 + temperature)));
}

int Heater::clalculatePower(float temperature) const {
  // temperatureLevel;
  
  // return 255 * (1 - temperature)
  return 0;
}

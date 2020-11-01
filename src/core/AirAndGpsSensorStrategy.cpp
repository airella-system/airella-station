#include "core/AirAndGpsSensorStrategy.h"

AirAndGpsSensorStrategy::AirAndGpsSensorStrategy() {
  this->serial = new HardwareSerial(config.uartNum);
  this->serial->begin(9600, SERIAL_8N1, config.uartRx, config.uartTx, false, 1000);
  pinMode(config.airSensorOrGpsMultiplexerPin, OUTPUT);

  digitalWrite(config.airSensorOrGpsMultiplexerPin, config.airSensorMultiplexerLogicValue);

  airSensor = new AirSensor(serial);
  airSensor->setupSerial();
  airSensor->powerOn();
  airSensor->calibrate();
  gpsSensor = new GpsSensor(serial);
}

void AirAndGpsSensorStrategy::switchToGpsSensor() {
  digitalWrite(config.airSensorOrGpsMultiplexerPin, config.gpsMultiplexerLogicValue);
  gpsSensor->setupSerial();
  while (this->serial->available()) {
    this->serial->read();
  }
}

void AirAndGpsSensorStrategy::switchToAirSensor() {
  digitalWrite(config.airSensorOrGpsMultiplexerPin, config.airSensorMultiplexerLogicValue);
  airSensor->setupSerial();
  while (this->serial->available()) {
    this->serial->read();
  }
  airSensor->calibrate();
}

AirSensor* AirAndGpsSensorStrategy::getAirSensor() {
  return this->airSensor;
}

GpsSensor* AirAndGpsSensorStrategy::getGpsSensor() {
  return this->gpsSensor;
}
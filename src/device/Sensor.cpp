#include "device/Sensor.h"

bool Sensor::isInit() {
  return initialized;
}

void Sensor::setTextState(String string) {
  this->stringState = string;
}

String Sensor::getTextState() {
  return stringState;
}
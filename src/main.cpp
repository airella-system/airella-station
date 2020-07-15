#include <Arduino.h>
#include <ArduinoJson.h>
#include "api/Api.h"
#include "communication/bluetooth/Bluetooth.h"
#include "communication/bluetooth/BluetoothHandler.h"
#include "communication/common/Internet.h"
#include "config/Config.h"
#include "maintenance/Logger.h"
#include "core/Core.h"

void setup() {
  core.setUp();
}

void loop() {
  Logger::info("[Core]: Started main loop");
  core.loop();
  Logger::error("[Core]: Escape from main core loop");
}
#pragma once

#include "api/Api.h"
#include "communication/bluetooth/BluetoothHandler.h"
#include "communication/common/Internet.h"
#include "config/Config.h"
#include "maintenance/Logger.h"

class BluetoothRefreshHandler : public BluetoothHandler {
 public:
  BluetoothRefreshHandler(){};
  ~BluetoothRefreshHandler(){};
  void deviceRefreshRequest(String &actionName);
};
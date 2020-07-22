#pragma once

#include "config/Config.h"
#include "api/Api.h"
#include "maintenance/Logger.h"
#include "communication/bluetooth/BluetoothHandler.h"
#include "communication/common/Internet.h"

class BluetoothRefreshHandler : public BluetoothHandler {
public:
    BluetoothRefreshHandler() {};
    ~BluetoothRefreshHandler() {};
    void deviceRefreshRequest(const String &actionName);
};
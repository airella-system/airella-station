#pragma once

#include "config/Config.h"
#include "api/Api.h"
#include "maintenance/Logger.h"
#include "communication/bluetooth/BluetoothHandler.h"

class BluetoothRefreshHandler : public BluetoothHandler {
public:
    BluetoothRefreshHandler() {};
    ~BluetoothRefreshHandler() {};
    void deviceRefreshRequest();
};
#include "communication/bluetooth/BluetoothRefreshHandler.h"

void BluetoothRefreshHandler::deviceRefreshRequest() {
    Logger::info("[BluetoothRefreshHandler]: Receive config update request");
    Config::save();
    Api.configUpdated();
    Logger::info("[BluetoothRefreshHandler]: Config updated");
}

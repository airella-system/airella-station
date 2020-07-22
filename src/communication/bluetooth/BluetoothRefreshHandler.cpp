#include "communication/bluetooth/BluetoothRefreshHandler.h"

void BluetoothRefreshHandler::deviceRefreshRequest(const String &actionName) {
    Logger::info("[BluetoothRefreshHandler]: Receive config update request");
    if(actionName.equals("wifi")) {
        Config::save();
        Internet::stop();
        Internet::start();
        Api.configUpdated();
    }
    else if(actionName.equals("address")) {
        Config::save();
        Api.configUpdated();
    }
    else if(actionName.equals("location")) {
        Config::save();
        Api.configUpdated();
    }
    else if(actionName.equals("register")) {
        Config::save();
        Api.configUpdated();
    }

    Logger::info("[BluetoothRefreshHandler]: Config updated");
}

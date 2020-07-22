#include "communication/bluetooth/BluetoothRefreshHandler.h"
#include "communication/bluetooth/Bluetooth.h"

void BluetoothRefreshHandler::deviceRefreshRequest(String &actionName) {
    Logger::info("[BluetoothRefreshHandler]: Receive config update request");
    if(actionName.equals("wifi")) {
        Bluetooth::setLastOperationStatus("wifi|setting_up");
        Internet::stop();
        Internet::start();
        Api.configUpdated();
        Config::save();
        Bluetooth::setLastOperationStatus("wifi|ok");
    }
    else if(actionName.equals("address")) {
        Bluetooth::setLastOperationStatus("address|setting_up");
        Api.publishAddressFromConfig();
        Api.publishNameFromConfig();
        Config::save();
        Bluetooth::setLastOperationStatus("address|ok");
    }
    else if(actionName.equals("location")) {
        Bluetooth::setLastOperationStatus("location|setting_up");
        Api.publishLocationFromConfig();
        Config::save();
        Bluetooth::setLastOperationStatus("location|ok");
    }
    else if(actionName.equals("register")) {
        Bluetooth::setLastOperationStatus("register|setting_up");
        Api.configUpdated();
        Config::save();
        Bluetooth::setLastOperationStatus("register|ok");
    }

    Logger::info("[BluetoothRefreshHandler]: Config updated");
}

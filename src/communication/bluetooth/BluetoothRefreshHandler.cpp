#include "communication/bluetooth/BluetoothRefreshHandler.h"
#include "communication/bluetooth/Bluetooth.h"

void BluetoothRefreshHandler::deviceRefreshRequest(String& actionName) {
  Logger::info(
      (String("[BluetoothRefreshHandler]: Receive config update request, action (") + actionName + ")").c_str());
  if (actionName.equals("wifi")) {
    Bluetooth::setLastOperationStatus("wifi|setting_up");
    int response = Internet::resetType(Internet::WIFI);
    if (response == 0) {
      Bluetooth::setLastOperationStatus("wifi|ok");
    } else {
      Bluetooth::setLastOperationStatus("wifi|error");
    }
  } else if (actionName.equals("gsm")) {
    Bluetooth::setLastOperationStatus("gsm|setting_up");
    int response = Internet::resetType(Internet::GSM);
    if (response == 0) {
      Bluetooth::setLastOperationStatus("gsm|ok");
    } else {
      Bluetooth::setLastOperationStatus("gsm|error");
    }
  } else if (actionName.equals("address")) {
    if (!Api.isRegistered()) {
      Logger::debug("[BluetoothRefreshHandler::deviceRefreshRequest()] Unauthorized to publishAddress()");
      Bluetooth::setLastOperationStatus("address|Unauthorized to publishAddress()");
      return;
    }

    bool actionResult = Api.publishAddress(Config::getAddressCountry().c_str(), Config::getAddressCity().c_str(),
                                           Config::getAddressStreet().c_str(), Config::getAddressNumber().c_str());

    if (!actionResult) {
      Logger::info("[BluetoothRefreshHandler::deviceRefreshRequest()] Unable to set address");
      Bluetooth::setLastOperationStatus("address|Unable to set address");
    } else {
      Logger::info("[BluetoothRefreshHandler::deviceRefreshRequest()] Set station address: ok");
      Bluetooth::setLastOperationStatus("address|ok");
    }
  } else if (actionName.equals("location")) {
    if (!Api.isRegistered()) {
      Logger::debug("[BluetoothRefreshHandler::deviceRefreshRequest()] Unauthorized to publishLocation()");
      Bluetooth::setLastOperationStatus("address|Unauthorized to publishLocation()");
      return;
    }

    bool actionResult =
        Api.publishLocation(Config::getLocationLatitude().toDouble(), Config::getLocationLongitude().toDouble());

    if (!actionResult) {
      Logger::info("[BluetoothRefreshHandler::deviceRefreshRequest()] Unable to set location");
      Bluetooth::setLastOperationStatus("location|Unable to set location");
    } else {
      Logger::info("[BluetoothRefreshHandler::deviceRefreshRequest()] Set station location: ok");
      Bluetooth::setLastOperationStatus("location|ok");
    }
  } else if (actionName.equals("register")) {
    Bluetooth::setLastOperationStatus("register|setting_up");
    RegistrationResult* result = Api.registerStation();

    if (result->ok)
      Bluetooth::setLastOperationStatus("register|ok");
    else
      Bluetooth::setLastOperationStatus(String("register|") + result->message);

    delete result;
  }

  Logger::info("[BluetoothRefreshHandler]: Config updated");
}

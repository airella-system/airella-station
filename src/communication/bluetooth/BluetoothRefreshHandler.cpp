#include "communication/bluetooth/BluetoothRefreshHandler.h"
#include "communication/bluetooth/Bluetooth.h"
#include "core/Task.h"

void BluetoothRefreshHandler::deviceRefreshRequest(String& actionName) {
  Logger::info(
      (String("[BluetoothRefreshHandler]: Receive config update request, action (") + actionName + ")").c_str());
  if (actionName.equals("wifi")) {
    Bluetooth::setLastOperationStatus("wifi|setting_up");
    Bluetooth::getTaskHandler()->startBlockingTask(
        [](TaskRequestorHandle<void*, double, String> task, void* data) {
          return String(Internet::resetType(Internet::WIFI));
        },
        [](double progress) {},
        [](String result) {
          if (result.equals("0")) {
            Bluetooth::setLastOperationStatus("wifi|ok");
          } else {
            Bluetooth::setLastOperationStatus("wifi|error");
          }
        });
  } else if (actionName.equals("gsm")) {
    Bluetooth::setLastOperationStatus("gsm|setting_up");
    Bluetooth::getTaskHandler()->startBlockingTask(
        [](TaskRequestorHandle<void*, double, String> task, void* data) {
          return String(Internet::resetType(Internet::GSM));
        },
        [](double progress) {},
        [](String result) {
          if (result.equals("0")) {
            Bluetooth::setLastOperationStatus("gsm|ok");
          } else {
            Bluetooth::setLastOperationStatus("gsm|error");
          }
        });
  } else if (actionName.equals("address")) {
    Bluetooth::setLastOperationStatus("address|setting_up");
    Bluetooth::getTaskHandler()->startBlockingTask(
        [](TaskRequestorHandle<void*, double, String> task, void* data) {
          if (!Api.isRegistered()) {
            Logger::debug("[BluetoothRefreshHandler::deviceRefreshRequest()] Unauthorized to publishAddress()");
            return String("OK");
          }

          bool actionResult =
              Api.publishAddress(Config::getAddressCountry().c_str(), Config::getAddressCity().c_str(),
                                 Config::getAddressStreet().c_str(), Config::getAddressNumber().c_str());

          if (!actionResult) {
            Logger::info("[BluetoothRefreshHandler::deviceRefreshRequest()] Unable to set address");
            return String("ERROR");
          } else {
            Logger::info("[BluetoothRefreshHandler::deviceRefreshRequest()] Set station address: ok");
            return String("OK");
          }
        },
        [](double progress) {},
        [](String result) {
          if (result.equals("ERROR")) {
            Bluetooth::setLastOperationStatus("address|error");
          } else if (result.equals("OK")) {
            Bluetooth::setLastOperationStatus("address|ok");
          }
        });
  } else if (actionName.equals("name")) {
    Bluetooth::setLastOperationStatus("name|setting_up");
    Bluetooth::getTaskHandler()->startBlockingTask(
        [](TaskRequestorHandle<void*, double, String> task, void* data) {
          if (!Api.isRegistered()) {
            Logger::debug("[BluetoothRefreshHandler::deviceRefreshRequest()] Unauthorized to publishName()");
            return String("OK");
          }

          bool actionResult = Api.publishName(Config::getStationName().c_str());

          if (!actionResult) {
            Logger::info("[BluetoothRefreshHandler::deviceRefreshRequest()] Unable to set name");
            return String("ERROR");
          } else {
            Logger::info("[BluetoothRefreshHandler::deviceRefreshRequest()] Set station name: ok");
            return String("OK");
          }
        },
        [](double progress) {},
        [](String result) {
          if (result.equals("ERROR")) {
            Bluetooth::setLastOperationStatus("name|error");
          } else if (result.equals("OK")) {
            Bluetooth::setLastOperationStatus("name|ok");
          }
        });

  } else if (actionName.equals("location")) {
    Bluetooth::setLastOperationStatus("location|setting_up");
    Bluetooth::getTaskHandler()->startBlockingTask(
        [](TaskRequestorHandle<void*, double, String> task, void* data) {
          if (!Api.isRegistered()) {
            Logger::debug("[BluetoothRefreshHandler::deviceRefreshRequest()] Unauthorized to publishLocation()");
            return String("OK");
          }

          bool actionResult =
              Api.publishLocation(Config::getLocationLatitude().toDouble(), Config::getLocationLongitude().toDouble());

          if (!actionResult) {
            Logger::info("[BluetoothRefreshHandler::deviceRefreshRequest()] Unable to set location");
            return String("ERROR");
          } else {
            Logger::info("[BluetoothRefreshHandler::deviceRefreshRequest()] Set station location: ok");
            return String("OK");
          }
        },
        [](double progress) {},
        [](String result) {
          if (result.equals("OK")) {
            Bluetooth::setLastOperationStatus("location|ok");
          } else {
            Bluetooth::setLastOperationStatus("location|error");
          }
        });

  } else if (actionName.equals("register")) {
    Bluetooth::setLastOperationStatus("register|setting_up");
    Bluetooth::getTaskHandler()->startBlockingTask(
        [](TaskRequestorHandle<void*, double, String> task, void* data) {
          RegistrationResult result = Api.registerStation();
          return result.ok ? String("OK") : String(result.message);
        },
        [](double progress) {},
        [](String result) {
          if (result.equals("OK")) {
            Bluetooth::setLastOperationStatus("register|ok");
          } else {
            Bluetooth::setLastOperationStatus(String("register|") + result);
          }
        });
  }

  Logger::info("[BluetoothRefreshHandler]: Config updated");
}

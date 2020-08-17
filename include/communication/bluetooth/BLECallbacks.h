#pragma once

#include "communication/bluetooth/Bluetooth.h"
#include "config/Config.h"
#include "maintenance/Logger.h"

class WifiSsidCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[WifiSsidCallback::onWrite()] called");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setWifiSsid(stringValue);
  }
};

class LastActionStateCallback : public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *pCharacteristic) {
    Logger::debug("[LastActionStateCallback::onRead()] called");
    pCharacteristic->setValue(Bluetooth::getLastOperationStatus().c_str());
  }
};

class WifiPasswordCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[WifiPasswordCallback::onWrite()] called");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setWifiPassword(stringValue);
  }
};

class ApiUrlCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[ApiUrlCallback::onWrite()] called");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setApiUrl(stringValue);
    Config::save();
  }
};

class StationNameCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[StationNameCallback::onWrite()] called");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setStationName(stringValue);
  }
};

class StationCountryCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[StationCountryCallback::onWrite()] called");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setAddressCountry(stringValue);
  }
};

class StationCityCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[StationCityCallback::onWrite()] called");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setAddressCity(stringValue);
  }
};

class StationStreetCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[StationStreetCallback::onWrite()] called");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setAddressStreet(stringValue);
  }
};

class StationNumberCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[StationNumberCallback::onWrite()] called");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setAddressNumber(stringValue);
  }
};

class LatitudeCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[LatitudeCallback::onWrite()] called");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setLocationLatitude(stringValue);
  }
};

class LongitudeCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[LongitudeCallback::onWrite()] called");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setLocationLongitude(stringValue);
  }
};

class LocationManualCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[LocationManualCallback::onWrite()] called");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    if (stringValue.charAt(0) == '1') {
      Config::setLocationManual(true);
    } else {
      Config::setLocationManual(false);
    }
  }
};

class RegistrationTokenCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[RegistrationTokenCallback::onWrite()] called");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setRegistratonToken(stringValue);
  }
};

class RefreshDeviceCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) { 
    Logger::debug("[RefreshDeviceCallback::onWrite()] called");
    std::string value = pCharacteristic->getValue();
    String actionName(value.c_str());
    Bluetooth::bluetoothHandler->deviceRefreshRequest(actionName); 
  }
};

class ClearDataCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[ClearDataCallback::onWrite()] called");
    Config::reset();
    Bluetooth::reloadValues();
  }
};
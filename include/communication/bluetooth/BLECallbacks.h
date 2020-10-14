#pragma once

#include "communication/bluetooth/Bluetooth.h"
#include "config/Config.h"
#include "maintenance/Logger.h"
#include "maintenance/Guardian.h"
#include "core/Core.h"

class InetConnTypeCallback : public BluetoothChunkerCallback {
  void afterReceive() {
    Logger::debug("[InetConnTypeCallback::afterReceive()] called");
    Config::InternetConnectionType type = static_cast<Config::InternetConnectionType>(atoi(chunker->getValue().c_str()));
    Config::setInternetConnectionType(type);
  }

  void beforeSend() {
    Logger::debug("[InetConnTypeCallback::beforeSend()] called");
    std::string type = "" + static_cast<int>(Config::getInternetConnectionType());
    chunker->setValue(type);
  }
};

class WifiSsidCallback : public BluetoothChunkerCallback {
  void afterReceive() {
    Logger::debug("[WifiSsidCallback::afterReceive()] called");
    Config::setWifiSsid(chunker->getValue().c_str());
  }

  void beforeSend() {
    Logger::debug("[WifiSsidCallback::beforeSend()] called");
    chunker->setValue(Config::getWifiSsid().c_str());
  }
};

class LastActionStateCallback : public BluetoothChunkerCallback {
  void afterReceive() {}

  void beforeSend() {
    Logger::debug("[LastActionStateCallback::beforeSend()] called");
    chunker->setValue(Bluetooth::getLastOperationStatus().c_str());
  }
};

class WifiPasswordCallback : public BluetoothChunkerCallback {
  void afterReceive() {
    Logger::debug("[WifiPasswordCallback::afterReceive()] called");
    Config::setWifiPassword(chunker->getValue().c_str());
  }

  void beforeSend() {
    Logger::debug("[WifiPasswordCallback::beforeSend()] called");
    chunker->setValue(Config::getWifiPassword().c_str());
  }
};

class GsmUrlCallback : public BluetoothChunkerCallback {
  void afterReceive() {
    Logger::debug("[GsmUrlCallback::afterReceive()] called");
    Config::setGsmExtenderUrl(chunker->getValue().c_str());
  }

  void beforeSend() {
    Logger::debug("[GsmUrlCallback::beforeSend()] called");
    chunker->setValue(Config::getGsmExtenderUrl().c_str());
  }
};

class GsmConfigCallback : public BluetoothChunkerCallback {
  void afterReceive() {
    Logger::debug("[GsmConfigCallback::afterReceive()] called");
    Config::setGsmConfig(chunker->getValue().c_str());
  }

  void beforeSend() {
    Logger::debug("[GsmConfigCallback::beforeSend()] called");
      chunker->setValue(Config::getGsmConfig().c_str());
  }
};

class ApiUrlCallback : public BluetoothChunkerCallback {
  void afterReceive() {
    Logger::debug("[ApiUrlCallback::afterReceive()] called");
    Config::setApiUrl(chunker->getValue().c_str());
  }

  void beforeSend() {
    Logger::debug("[ApiUrlCallback::beforeSend()] called");
    chunker->setValue(Config::getApiUrl().c_str());
  }
};

class StationNameCallback : public BluetoothChunkerCallback {
  void afterReceive() {
    Logger::debug("[StationNameCallback::afterReceive()] called");
    Config::setStationName(chunker->getValue().c_str());
  }

  void beforeSend() {
    Logger::debug("[StationNameCallback::beforeSend()] called");
    chunker->setValue(Config::getStationName().c_str());
  }
};

class StationCountryCallback : public BluetoothChunkerCallback {
  void afterReceive() {
    Logger::debug("[StationCountryCallback::afterReceive()] called");
    Config::setAddressCountry(chunker->getValue().c_str());
  }

  void beforeSend() {
    Logger::debug("[StationCountryCallback::beforeSend()] called");
    chunker->setValue(Config::getAddressCountry().c_str());
  }
};

class StationCityCallback : public BluetoothChunkerCallback {
  void afterReceive() {
    Logger::debug("[StationCityCallback::afterReceive()] called");
    Config::setAddressCity(chunker->getValue().c_str());
  }

  void beforeSend() {
    Logger::debug("[StationCityCallback::beforeSend()] called");
    chunker->setValue(Config::getAddressCity().c_str());
  }
};

class StationStreetCallback : public BluetoothChunkerCallback {
  void afterReceive() {
    Logger::debug("[StationCityCallback::afterReceive()] called");
    Config::setAddressStreet(chunker->getValue().c_str());
  }

  void beforeSend() {
    Logger::debug("[StationCityCallback::beforeSend()] called");
    chunker->setValue(Config::getAddressStreet().c_str());
  }
};

class StationNumberCallback : public BluetoothChunkerCallback {
  void afterReceive() {
    Logger::debug("[StationNumberCallback::afterReceive()] called");
    Config::setAddressNumber(chunker->getValue().c_str());
  }

  void beforeSend() {
    Logger::debug("[StationNumberCallback::beforeSend()] called");
    chunker->setValue(Config::getAddressNumber().c_str());
  }
};

class LatitudeCallback : public BluetoothChunkerCallback {
  void afterReceive() {
    Logger::debug("[LatitudeCallback::afterReceive()] called");
    Config::setLocationLatitude(chunker->getValue().c_str());
  }

  void beforeSend() {
    Logger::debug("[LatitudeCallback::beforeSend()] called");
    chunker->setValue(Config::getLocationLatitude().c_str());
  }
};

class LongitudeCallback : public BluetoothChunkerCallback {
  void afterReceive() {
    Logger::debug("[LongitudeCallback::afterReceive()] called");
    Config::setLocationLongitude(chunker->getValue().c_str());
  }

  void beforeSend() {
    Logger::debug("[LongitudeCallback::beforeSend()] called");
    chunker->setValue(Config::getLocationLongitude().c_str());
  }
};

class LocationManualCallback : public BluetoothChunkerCallback {
  void afterReceive() {
    Logger::debug("[LocationManualCallback::afterReceive()] called");
    std::string message = chunker->getValue();
    if (message.length() > 0 && message[0] == '1') {
      Config::setLocationManual(true);
    } else {
      Config::setLocationManual(false);
    }
  }

  void beforeSend() {
    Logger::debug("[LocationManualCallback::beforeSend()] called");
    chunker->setValue("" + Config::getLocationManual());
  }
};

class RegistrationTokenCallback : public BluetoothChunkerCallback {
  void afterReceive() {
    Logger::debug("[RegistrationTokenCallback::afterReceive()] called");
    Config::setRegistratonToken(chunker->getValue().c_str());
  }

  void beforeSend() {
    Logger::debug("[RegistrationTokenCallback::beforeSend()] called");
    chunker->setValue(Config::getRegistratonToken().c_str());
  }
};

class RefreshDeviceCallback : public BluetoothChunkerCallback {
  void afterReceive() {
    Logger::debug("[RefreshDeviceCallback::afterReceive()] called");
    String message = chunker->getValue().c_str();
    Bluetooth::bluetoothHandler->deviceRefreshRequest(message); 
  }

  void beforeSend() {}
};

class ClearDataCallback : public BluetoothChunkerCallback {
  void afterReceive() {
    Logger::debug("[ClearDataCallback::afterReceive()] called");
    Config::reset();
    Bluetooth::reloadValues();
    core.reset();
  }

  void beforeSend() {}
};

class RegistrationStateCallback : public BluetoothChunkerCallback {
  void afterReceive() {}

  void beforeSend() {
    Logger::debug("[RegistrationStateCallback::beforeSend()] called");
    chunker->setValue("" + (int)Config::getRegistrationState());
  }
};

class InetConnectionStateCallback : public BluetoothChunkerCallback {
  void afterReceive() {}

  void beforeSend() {
    Logger::debug("[InetConnectionStateCallback::beforeSend()] called");
    chunker->setValue("" + WiFiConn::isConnected());
  }
};

class DeviceStateCallback : public BluetoothChunkerCallback {
  void afterReceive() {}

  void beforeSend() {
    Logger::debug("[DeviceStateCallback::beforeSend()] called");
    chunker->setValue(Guardian::getDeviceState().c_str());
  }
};

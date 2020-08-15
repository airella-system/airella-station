#include "communication/bluetooth/Bluetooth.h"
#include "config/Config.h"
#include "maintenance/Logger.h"

#define BT_AUTH_ENABLE 1

const uint32_t Bluetooth::W_PROPERTY = BLECharacteristic::PROPERTY_WRITE;
const uint32_t Bluetooth::R_PROPERTY = BLECharacteristic::PROPERTY_READ;
const uint32_t Bluetooth::RW_PROPERTY = BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE;

BluetoothHandler *Bluetooth::bluetoothHandler = nullptr;

BLECharacteristic *Bluetooth::stationNameCharacteristic = nullptr;
BLECharacteristic *Bluetooth::stationCountryCharacteristic = nullptr;
BLECharacteristic *Bluetooth::stationCityCharacteristic = nullptr;
BLECharacteristic *Bluetooth::stationStreetCharacteristic = nullptr;
BLECharacteristic *Bluetooth::stationNumberCharacteristic = nullptr;
BLECharacteristic *Bluetooth::latitudeCharacteristic = nullptr;
BLECharacteristic *Bluetooth::longitudeCharacteristic = nullptr;
BLECharacteristic *Bluetooth::locationManualCharacteristic = nullptr;

BLECharacteristic *Bluetooth::devPasswordCharacteristic = nullptr;
BLECharacteristic *Bluetooth::inetConnTypeCharacteristic = nullptr;
BLECharacteristic *Bluetooth::registrationStateCharacteristic = nullptr;
BLECharacteristic *Bluetooth::ssidCharacteristic = nullptr;
BLECharacteristic *Bluetooth::wifiPassCharacteristic = nullptr;
BLECharacteristic *Bluetooth::registerTokenCharacteristic = nullptr;
BLECharacteristic *Bluetooth::apiUrlCharacteristic = nullptr;
BLECharacteristic *Bluetooth::devStateCharacteristic = nullptr;
BLECharacteristic *Bluetooth::connStateCharacteristic = nullptr;
BLECharacteristic *Bluetooth::refreshDeviceCharacteristic = nullptr;
BLECharacteristic *Bluetooth::clearDataCharacteristic = nullptr;

String Bluetooth::lastOperatioinState = String();

#ifdef BT_AUTH_ENABLE
class MySecurity : public BLESecurityCallbacks {
  uint32_t onPassKeyRequest() { return 123456; }

  void onPassKeyNotify(uint32_t pass_key) {}

  bool onConfirmPIN(uint32_t pass_key) {
    vTaskDelay(5000);
    return true;
  }

  bool onSecurityRequest() { return true; }

  void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl) {}
};
#endif

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

void Bluetooth::reloadValues() {
  ssidCharacteristic->setValue(Config::getWifiSsid().c_str());
  wifiPassCharacteristic->setValue(Config::getWifiPassword().c_str());
  registerTokenCharacteristic->setValue(Config::getRegistratonToken().c_str());
  apiUrlCharacteristic->setValue(Config::getApiUrl().c_str());
  stationNameCharacteristic->setValue(Config::getStationName().c_str());
  stationCountryCharacteristic->setValue(Config::getAddressCountry().c_str());
  stationCityCharacteristic->setValue(Config::getAddressCity().c_str());
  stationStreetCharacteristic->setValue(Config::getAddressStreet().c_str());
  stationNumberCharacteristic->setValue(Config::getAddressNumber().c_str());
  latitudeCharacteristic->setValue(Config::getLocationLatitude().c_str());
  longitudeCharacteristic->setValue(Config::getLocationLongitude().c_str());
  locationManualCharacteristic->setValue(Config::getLocationManual() ? "1" : "0");
}

void Bluetooth::start(BluetoothHandler *bluetoothHandler) {
  Bluetooth::bluetoothHandler = bluetoothHandler;
  BLEDevice::init("Airella Station");

  #ifdef BT_AUTH_ENABLE
  BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
  BLEDevice::setSecurityCallbacks(new MySecurity());
  #endif
  
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(BLEUUID((const char *)SERVICE_UUID), 60);

  devPasswordCharacteristic = pService->createCharacteristic(DEVICE_PASSWORD_CUUID, W_PROPERTY);
  
  inetConnTypeCharacteristic = pService->createCharacteristic(INTERNET_CONNECTION_TYPE_CUUID, RW_PROPERTY);
  
  registrationStateCharacteristic = pService->createCharacteristic(REGISTRATION_STATE_CUUID, R_PROPERTY);
  registrationStateCharacteristic->setCallbacks(new LastActionStateCallback());

  ssidCharacteristic = pService->createCharacteristic(SSID_CUUID, RW_PROPERTY);
  ssidCharacteristic->setCallbacks(new WifiSsidCallback());

  wifiPassCharacteristic = pService->createCharacteristic(WIFI_PASWORD_CUUID, W_PROPERTY);
  wifiPassCharacteristic->setCallbacks(new WifiPasswordCallback());

  registerTokenCharacteristic = pService->createCharacteristic(REGISTRATION_TOKEN_CUUID, W_PROPERTY);
  registerTokenCharacteristic->setCallbacks(new RegistrationTokenCallback());

  apiUrlCharacteristic = pService->createCharacteristic(API_URL_CUUID, RW_PROPERTY);
  apiUrlCharacteristic->setCallbacks(new ApiUrlCallback());

  stationNameCharacteristic = pService->createCharacteristic(STATION_NAME_CUUID, RW_PROPERTY);
  stationNameCharacteristic->setCallbacks(new StationNameCallback());

  stationCountryCharacteristic = pService->createCharacteristic(STATION_COUNTRY_CUUID, RW_PROPERTY);
  stationCountryCharacteristic->setCallbacks(new StationCountryCallback());

  stationCityCharacteristic = pService->createCharacteristic(STATION_CITY_CUUID, RW_PROPERTY);
  stationCityCharacteristic->setCallbacks(new StationCityCallback());

  stationStreetCharacteristic = pService->createCharacteristic(STATION_STREET_CUUID, RW_PROPERTY);
  stationStreetCharacteristic->setCallbacks(new StationStreetCallback());

  stationNumberCharacteristic = pService->createCharacteristic(STATION_NUMBER_CUUID, RW_PROPERTY);
  stationNumberCharacteristic->setCallbacks(new StationNumberCallback());

  latitudeCharacteristic = pService->createCharacteristic(LOCATION_LATITUDE_CUUID, RW_PROPERTY);
  latitudeCharacteristic->setCallbacks(new LatitudeCallback());

  longitudeCharacteristic = pService->createCharacteristic(LOCATION_LONGITUDE_CUUID, RW_PROPERTY);
  longitudeCharacteristic->setCallbacks(new LongitudeCallback());

  locationManualCharacteristic = pService->createCharacteristic(LOCATION_MANUALLY_CUUID, RW_PROPERTY);
  locationManualCharacteristic->setCallbacks(new LocationManualCallback());

  devStateCharacteristic = pService->createCharacteristic(DEVICE_STATE_CUUID, R_PROPERTY);

  connStateCharacteristic = pService->createCharacteristic(CONNECTION_STATE_CUUID, R_PROPERTY);

  refreshDeviceCharacteristic = pService->createCharacteristic(REFRESH_DEVICE_CUUID, W_PROPERTY);
  refreshDeviceCharacteristic->setCallbacks(new RefreshDeviceCallback());

  clearDataCharacteristic = pService->createCharacteristic(CLEAR_DATA_CUUID, W_PROPERTY);
  clearDataCharacteristic->setCallbacks(new ClearDataCallback());

  #ifdef BT_AUTH_ENABLE
  devPasswordCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  inetConnTypeCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  registrationStateCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  ssidCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  wifiPassCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  registerTokenCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  apiUrlCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  stationNameCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  stationCountryCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  stationCityCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  stationStreetCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  stationNumberCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  latitudeCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  longitudeCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  locationManualCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  devStateCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  connStateCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  refreshDeviceCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  clearDataCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  #endif

  reloadValues();
  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  #ifdef BT_AUTH_ENABLE
  BLESecurity *pSecurity = new BLESecurity();
  uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
  uint32_t passkey = 234567;
  uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t));
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND);
  pSecurity->setCapability(ESP_IO_CAP_OUT);
  pSecurity->setKeySize(16);
  esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &auth_option, sizeof(uint8_t));
  pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));
  #endif
}

String Bluetooth::getLastOperationStatus() {
  return lastOperatioinState;
}

void Bluetooth::setLastOperationStatus(String operationStatus) {
  lastOperatioinState = operationStatus;
}
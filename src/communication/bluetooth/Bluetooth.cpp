#include "communication/bluetooth/Bluetooth.h"
#include "config/Config.h"
#include "maintenance/Logger.h"

#define BT_AUTH_ENABLE 1

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
    Logger::debug("[WifiSsidCallback::onWrite()] revoke");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setWifiSsid(stringValue);
    Config::save();
    Internet::start();
  }
};

//TODO: zamienić na last state
class RegistrationStateCallback : public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *pCharacteristic) {
    Logger::debug("[RegistrationStateCallback::onRead()] revoke");
    pCharacteristic->setValue(String(Config::getRegistrationState()).c_str());
  }
};

class WifiPasswordCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[WifiPasswordCallback::onWrite()] revoke");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setWifiPassword(stringValue);
    Config::save();
    Internet::start();
  }
};

class ApiUrlCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[ApiUrlCallback::onWrite()] revoke");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setApiUrl(stringValue);
    Config::save();
  }
};

class StationNameCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[StationNameCallback::onWrite()] revoke");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setStationName(stringValue);
    Config::save();
  }
};

class StationCountryCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[StationCountryCallback::onWrite()] revoke");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setAddressCountry(stringValue);
    Config::save();
  }
};

class StationCityCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[StationCityCallback::onWrite()] revoke");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setAddressCity(stringValue);
    Config::save();
  }
};

class StationStreetCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[StationStreetCallback::onWrite()] revoke");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setAddressStreet(stringValue);
    Config::save();
  }
};

class StationNumberCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[StationNumberCallback::onWrite()] revoke");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setAddressNumber(stringValue);
    Config::save();
  }
};

class LatitudeCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[LatitudeCallback::onWrite()] revoke");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setLocationLatitude(stringValue);
    Config::save();
  }
};

class LongitudeCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[LongitudeCallback::onWrite()] revoke");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setLocationLongitude(stringValue);
    Config::save();
  }
};

class LocationManualCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[LocationManualCallback::onWrite()] revoke");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    if (stringValue.charAt(0) == '1') {
      Config::setLocationManual(true);
    } else {
      Config::setLocationManual(false);
    }
    Config::save();
  }
};



class RegistrationTokenCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[RegistrationTokenCallback::onWrite()] revoke");
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::setRegistratonToken(stringValue);
    Config::save();
  }
};

class RefreshDeviceCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) { 
    Logger::debug("[RefreshDeviceCallback::onWrite()] revoke");
    Bluetooth::bluetoothHandler->deviceRefreshRequest(); 
  }
};

class ClearDataCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Logger::debug("[ClearDataCallback::onWrite()] revoke");
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

  devPasswordCharacteristic =
      pService->createCharacteristic(DEVICE_PASSWORD_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  
  inetConnTypeCharacteristic =
      pService->createCharacteristic(INTERNET_CONNECTION_TYPE_CHARACTERISTIC_UUID,
                                     BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  
  registrationStateCharacteristic =
      pService->createCharacteristic(REGISTRATION_STATE_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
  
  registrationStateCharacteristic->setCallbacks(new RegistrationStateCallback());

  ssidCharacteristic = pService->createCharacteristic(
      SSID_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  ssidCharacteristic->setCallbacks(new WifiSsidCallback());

  wifiPassCharacteristic =
      pService->createCharacteristic(WIFI_PASWORD_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  wifiPassCharacteristic->setCallbacks(new WifiPasswordCallback());

  registerTokenCharacteristic =
      pService->createCharacteristic(REGISTRATION_TOKEN_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  registerTokenCharacteristic->setCallbacks(new RegistrationTokenCallback());

  apiUrlCharacteristic = pService->createCharacteristic(
      API_URL_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  apiUrlCharacteristic->setCallbacks(new ApiUrlCallback());

  stationNameCharacteristic = pService->createCharacteristic(
      STATION_NAME_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  stationNameCharacteristic->setCallbacks(new StationNameCallback());

  stationCountryCharacteristic = pService->createCharacteristic(
      STATION_COUNTRY_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  stationCountryCharacteristic->setCallbacks(new StationCountryCallback());

  stationCityCharacteristic = pService->createCharacteristic(
      STATION_CITY_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  stationCityCharacteristic->setCallbacks(new StationCityCallback());

  stationNumberCharacteristic = pService->createCharacteristic(
      STATION_NUMBER_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  stationNumberCharacteristic->setCallbacks(new StationNumberCallback());

  latitudeCharacteristic = pService->createCharacteristic(
      STATION_STREET_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  latitudeCharacteristic->setCallbacks(new LatitudeCallback());

  longitudeCharacteristic = pService->createCharacteristic(
      STATION_NUMBER_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  longitudeCharacteristic->setCallbacks(new LongitudeCallback());

  locationManualCharacteristic = pService->createCharacteristic(
      STATION_NUMBER_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  locationManualCharacteristic->setCallbacks(new LocationManualCallback());

  devStateCharacteristic =
      pService->createCharacteristic(DEVICE_STATE_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);

  connStateCharacteristic =
      pService->createCharacteristic(CONNECTION_STATE_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);

  refreshDeviceCharacteristic =
      pService->createCharacteristic(REFRESH_DEVICE_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  refreshDeviceCharacteristic->setCallbacks(new RefreshDeviceCallback());

  clearDataCharacteristic =
      pService->createCharacteristic(CLEAR_DATA_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  clearDataCharacteristic->setCallbacks(new ClearDataCallback());

  #ifdef BT_AUTH_ENABLE
  devPasswordCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  inetConnTypeCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  registrationStateCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  ssidCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  wifiPassCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  registerTokenCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  apiUrlCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  stationNameCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  stationCountryCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  stationCityCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  stationStreetCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  stationNumberCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  latitudeCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  longitudeCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  locationManualCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  devStateCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  connStateCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  refreshDeviceCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  clearDataCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
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
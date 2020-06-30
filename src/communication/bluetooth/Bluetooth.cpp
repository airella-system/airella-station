#include "communication/bluetooth/Bluetooth.h"
#include "config/Config.h"

BluetoothHandler *Bluetooth::bluetoothHandler = nullptr;

BLECharacteristic *Bluetooth::stationNameCharacteristic = nullptr;
BLECharacteristic *Bluetooth::stationCountryCharacteristic = nullptr;
BLECharacteristic *Bluetooth::stationCityCharacteristic = nullptr;
BLECharacteristic *Bluetooth::stationStreetCharacteristic = nullptr;
BLECharacteristic *Bluetooth::stationNumberCharacteristic = nullptr;

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

class WifiSsidCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::instance().setWifiSsid(stringValue);
    Config::instance().save();
  }
};

class RegistrationStateCallback : public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *pCharacteristic) {
    pCharacteristic->setValue(String(Config::instance().getRegistrationState()).c_str());
  }
};

class WifiPasswordCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::instance().setWifiPassword(stringValue);
    Config::instance().save();
  }
};

class ApiUrlCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::instance().setApiUrl(stringValue);
    Config::instance().save();
  }
};

class StationNameCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::instance().setStationName(stringValue);
    Config::instance().save();
  }
};

class StationCountryCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::instance().setAddressCountry(stringValue);
    Config::instance().save();
  }
};

class StationCityCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::instance().setAddressCity(stringValue);
    Config::instance().save();
  }
};

class StationStreetCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::instance().setAddressStreet(stringValue);
    Config::instance().save();
  }
};

class StationNumberCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::instance().setAddressNumber(stringValue);
    Config::instance().save();
  }
};

class RegistrationTokenCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    String stringValue = String(value.c_str());
    Config::instance().setRegistratonToken(stringValue);
    Config::instance().save();
  }
};

class RefreshDeviceCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Bluetooth::bluetoothHandler->deviceRefreshRequest();
  }
};

class ClearDataCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Config::instance().reset();
    Bluetooth::reloadValues();
  }
};

void Bluetooth::reloadValues() {
  ssidCharacteristic->setValue(Config::instance().getWifiSsid().c_str());
  wifiPassCharacteristic->setValue(
      Config::instance().getWifiPassword().c_str());
  registerTokenCharacteristic->setValue(
      Config::instance().getRegistratonToken().c_str());
  apiUrlCharacteristic->setValue(Config::instance().getApiUrl().c_str());
  stationNameCharacteristic->setValue(Config::instance().getStationName().c_str());
  stationCountryCharacteristic->setValue(Config::instance().getAddressCountry().c_str());
  stationCityCharacteristic->setValue(Config::instance().getAddressCity().c_str());
  stationStreetCharacteristic->setValue(Config::instance().getAddressStreet().c_str());
  stationNumberCharacteristic->setValue(Config::instance().getAddressNumber().c_str());
}

void Bluetooth::start(BluetoothHandler *bluetoothHandler) {
  Bluetooth::bluetoothHandler = bluetoothHandler;
  BLEDevice::init("Airella Station");
  BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
  BLEDevice::setSecurityCallbacks(new MySecurity());
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService =
      pServer->createService(BLEUUID((const char *)SERVICE_UUID), 60);

  devPasswordCharacteristic = pService->createCharacteristic(
      DEVICE_PASSWORD_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  devPasswordCharacteristic->setAccessPermissions(
      ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);

  inetConnTypeCharacteristic = pService->createCharacteristic(
      INTERNET_CONNECTION_TYPE_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  inetConnTypeCharacteristic->setAccessPermissions(
      ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);

  registrationStateCharacteristic = pService->createCharacteristic(
      REGISTRATION_STATE_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ);
  registrationStateCharacteristic->setAccessPermissions(
      ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  registrationStateCharacteristic->setCallbacks(new RegistrationStateCallback());

  ssidCharacteristic = pService->createCharacteristic(
      SSID_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  ssidCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED |
                                           ESP_GATT_PERM_WRITE_ENCRYPTED);
  ssidCharacteristic->setCallbacks(new WifiSsidCallback());

  wifiPassCharacteristic = pService->createCharacteristic(
      WIFI_PASWORD_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  wifiPassCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED |
                                               ESP_GATT_PERM_WRITE_ENCRYPTED);
  wifiPassCharacteristic->setCallbacks(new WifiPasswordCallback());

  registerTokenCharacteristic =
      pService->createCharacteristic(REGISTRATION_TOKEN_CHARACTERISTIC_UUID,
                                     BLECharacteristic::PROPERTY_WRITE);
  registerTokenCharacteristic->setAccessPermissions(
      ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  registerTokenCharacteristic->setCallbacks(new RegistrationTokenCallback());

  apiUrlCharacteristic = pService->createCharacteristic(
      API_URL_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  apiUrlCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED |
                                             ESP_GATT_PERM_WRITE_ENCRYPTED);
  apiUrlCharacteristic->setCallbacks(new ApiUrlCallback());

  stationNameCharacteristic = pService->createCharacteristic(
      STATION_NAME_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  stationNameCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED |
                                             ESP_GATT_PERM_WRITE_ENCRYPTED);
  stationNameCharacteristic->setCallbacks(new StationNameCallback());

  stationCountryCharacteristic = pService->createCharacteristic(
      STATION_COUNTRY_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  stationCountryCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED |
                                             ESP_GATT_PERM_WRITE_ENCRYPTED);
  stationCountryCharacteristic->setCallbacks(new StationCountryCallback());

  stationCityCharacteristic = pService->createCharacteristic(
      STATION_CITY_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  stationCityCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED |
                                             ESP_GATT_PERM_WRITE_ENCRYPTED);
  stationCityCharacteristic->setCallbacks(new StationCityCallback());

  stationStreetCharacteristic = pService->createCharacteristic(
      STATION_STREET_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  stationStreetCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED |
                                             ESP_GATT_PERM_WRITE_ENCRYPTED);
  stationStreetCharacteristic->setCallbacks(new StationStreetCallback());

  stationNumberCharacteristic = pService->createCharacteristic(
      STATION_NUMBER_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  stationNumberCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED |
                                             ESP_GATT_PERM_WRITE_ENCRYPTED);
  stationNumberCharacteristic->setCallbacks(new StationNumberCallback());

  devStateCharacteristic = pService->createCharacteristic(
      DEVICE_STATE_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
  devStateCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED |
                                               ESP_GATT_PERM_WRITE_ENCRYPTED);

  connStateCharacteristic = pService->createCharacteristic(
      CONNECTION_STATE_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
  connStateCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED |
                                                ESP_GATT_PERM_WRITE_ENCRYPTED);

  refreshDeviceCharacteristic = pService->createCharacteristic(
      REFRESH_DEVICE_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  refreshDeviceCharacteristic->setAccessPermissions(
      ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  refreshDeviceCharacteristic->setCallbacks(new RefreshDeviceCallback());

  clearDataCharacteristic = pService->createCharacteristic(
      CLEAR_DATA_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  clearDataCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED |
                                                ESP_GATT_PERM_WRITE_ENCRYPTED);
  clearDataCharacteristic->setCallbacks(new ClearDataCallback());

  reloadValues();

  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  BLESecurity *pSecurity = new BLESecurity();
  uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
  uint32_t passkey = 234567;
  uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey,
                                 sizeof(uint32_t));
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND);
  pSecurity->setCapability(ESP_IO_CAP_OUT);
  pSecurity->setKeySize(16);
  esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH,
                                 &auth_option, sizeof(uint8_t));
  pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key,
                                 sizeof(uint8_t));
}
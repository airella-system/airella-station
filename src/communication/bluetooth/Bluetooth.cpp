#include "communication/bluetooth/Bluetooth.h"
#include "config/Config.h"
#include "maintenance/Logger.h"
#include "communication/bluetooth/BLECallbacks.h"

const uint32_t Bluetooth::W_PROPERTY = BLECharacteristic::PROPERTY_WRITE;
const uint32_t Bluetooth::R_PROPERTY = BLECharacteristic::PROPERTY_READ;
const uint32_t Bluetooth::RW_PROPERTY = BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE;

BluetoothHandler *Bluetooth::bluetoothHandler = nullptr;

BluetoothChunker *Bluetooth::stationCountryCharacteristic = nullptr;
BluetoothChunker *Bluetooth::stationNameCharacteristic = nullptr;
BluetoothChunker *Bluetooth::stationCityCharacteristic = nullptr;
BluetoothChunker *Bluetooth::stationStreetCharacteristic = nullptr;
BluetoothChunker *Bluetooth::stationNumberCharacteristic = nullptr;
BluetoothChunker *Bluetooth::latitudeCharacteristic = nullptr;
BluetoothChunker *Bluetooth::longitudeCharacteristic = nullptr;
BluetoothChunker *Bluetooth::locationManualCharacteristic = nullptr;
BluetoothChunker *Bluetooth::inetConnTypeCharacteristic = nullptr;

BluetoothChunker *Bluetooth::ssidCharacteristic = nullptr;
BluetoothChunker *Bluetooth::wifiPassCharacteristic = nullptr;
BluetoothChunker *Bluetooth::registerTokenCharacteristic = nullptr;
BluetoothChunker *Bluetooth::apiUrlCharacteristic = nullptr;
BluetoothChunker *Bluetooth::refreshDeviceCharacteristic = nullptr;
BluetoothChunker *Bluetooth::clearDataCharacteristic = nullptr;

BluetoothChunker *Bluetooth::registrationStateCharacteristic = nullptr;
BluetoothChunker *Bluetooth::inetConnStateCharacteristic = nullptr;
BluetoothChunker *Bluetooth::deviceStateCharacteristic = nullptr;

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
  BLEService *pService = pServer->createService(BLEUUID((const char *)SERVICE_UUID), 100);

  inetConnTypeCharacteristic = new BluetoothChunker(pService, INTERNET_CONNECTION_TYPE_CUUID, RW_PROPERTY);
  inetConnTypeCharacteristic->setCallback(new InetConnTypeCallback());

  ssidCharacteristic = new BluetoothChunker(pService, SSID_CUUID, RW_PROPERTY);
  ssidCharacteristic->setCallback(new WifiSsidCallback());

  wifiPassCharacteristic = new BluetoothChunker(pService, WIFI_PASWORD_CUUID, W_PROPERTY);
  wifiPassCharacteristic->setCallback(new WifiPasswordCallback());

  registerTokenCharacteristic = new BluetoothChunker(pService, REGISTRATION_TOKEN_CUUID, W_PROPERTY);
  registerTokenCharacteristic->setCallback(new RegistrationTokenCallback());

  apiUrlCharacteristic = new BluetoothChunker(pService, API_URL_CUUID, RW_PROPERTY);
  apiUrlCharacteristic->setCallback(new ApiUrlCallback());

  stationNameCharacteristic = new BluetoothChunker(pService, STATION_NAME_CUUID, RW_PROPERTY);
  stationNameCharacteristic->setCallback(new StationNameCallback());

  stationCountryCharacteristic = new BluetoothChunker(pService, STATION_COUNTRY_CUUID, RW_PROPERTY);
  stationCountryCharacteristic->setCallback(new StationCountryCallback());

  stationCityCharacteristic = new BluetoothChunker(pService, STATION_CITY_CUUID, RW_PROPERTY);
  stationCityCharacteristic->setCallback(new StationCityCallback());

  stationStreetCharacteristic = new BluetoothChunker(pService, STATION_STREET_CUUID, RW_PROPERTY);
  stationStreetCharacteristic->setCallback(new StationStreetCallback());

  stationNumberCharacteristic = new BluetoothChunker(pService, STATION_NUMBER_CUUID, RW_PROPERTY);
  stationNumberCharacteristic->setCallback(new StationNumberCallback());

  latitudeCharacteristic = new BluetoothChunker(pService, LOCATION_LATITUDE_CUUID, RW_PROPERTY);
  latitudeCharacteristic->setCallback(new LatitudeCallback());

  longitudeCharacteristic = new BluetoothChunker(pService, LOCATION_LONGITUDE_CUUID, RW_PROPERTY);
  longitudeCharacteristic->setCallback(new LongitudeCallback());

  locationManualCharacteristic = new BluetoothChunker(pService, LOCATION_MANUALLY_CUUID, RW_PROPERTY);
  locationManualCharacteristic->setCallback(new LocationManualCallback());

  refreshDeviceCharacteristic = new BluetoothChunker(pService, REFRESH_DEVICE_CUUID, W_PROPERTY);
  refreshDeviceCharacteristic->setCallback(new RefreshDeviceCallback());

  clearDataCharacteristic = new BluetoothChunker(pService, CLEAR_DATA_CUUID, W_PROPERTY);
  clearDataCharacteristic->setCallback(new ClearDataCallback());

  registrationStateCharacteristic = new BluetoothChunker(pService, REGISTRATION_STATE_CUUID, R_PROPERTY);
  registrationStateCharacteristic->setCallback(new RegistrationStateCallback());

  inetConnStateCharacteristic = new BluetoothChunker(pService, CONNECTION_STATE_CUUID, R_PROPERTY);
  inetConnStateCharacteristic->setCallback(new InetConnectionStateCallback());

  deviceStateCharacteristic = new BluetoothChunker(pService, DEVICE_STATE_CUUID, R_PROPERTY);
  deviceStateCharacteristic->setCallback(new DeviceStateCallback());

  #ifdef BT_AUTH_ENABLE
  stationNameCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  stationCountryCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  stationCityCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  stationStreetCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  stationNumberCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  latitudeCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  longitudeCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  locationManualCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  inetConnTypeCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);

  ssidCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  wifiPassCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  registerTokenCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  apiUrlCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  refreshDeviceCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  clearDataCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);

  registrationStateCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  inetConnStateCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
  deviceStateCharacteristic->setAccessPermissions(DEFAULT_BT_PERMISSION);
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
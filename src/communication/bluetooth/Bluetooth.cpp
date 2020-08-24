#include "communication/bluetooth/Bluetooth.h"
#include "communication/bluetooth/BLECallbacks.h"
#include "config/Config.h"
#include "maintenance/Logger.h"

const uint32_t Bluetooth::W_PROPERTY = BLECharacteristic::PROPERTY_WRITE;
const uint32_t Bluetooth::R_PROPERTY = BLECharacteristic::PROPERTY_READ;
const uint32_t Bluetooth::RW_PROPERTY = BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE;

BluetoothHandler *Bluetooth::bluetoothHandler = nullptr;

BLEServer *Bluetooth::bleServer = nullptr;

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

class CustomBLESecurity : public BLESecurityCallbacks {
  uint32_t onPassKeyRequest() { return 0; }

  void onPassKeyNotify(uint32_t pass_key) {}

  bool onConfirmPIN(uint32_t pass_key) { return true; }

  bool onSecurityRequest() { return true; }

  void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl) {
    int bondDevicesNum = esp_ble_get_bond_device_num();
    if (cmpl.success && bondDevicesNum > 1) {
      Logger::debug("[Bluetooth::onAuthenticationComplete()] Already added device - removing bond request");
      esp_ble_remove_bond_device(cmpl.bd_addr);
    } else if (cmpl.success) {
      Logger::debug("[Bluetooth::onAuthenticationComplete()] Added new device");
      Bluetooth::setDiscoverability(false);
    }
  }
};

BLEServer *Bluetooth::getBleServer() {
  return bleServer;
}

void Bluetooth::removeBondDevices() {
  int dev_num = esp_ble_get_bond_device_num();
  if (dev_num > 0) {
    esp_ble_bond_dev_t *dev_list = (esp_ble_bond_dev_t *)malloc(sizeof(esp_ble_bond_dev_t) * dev_num);
    esp_ble_get_bond_device_list(&dev_num, dev_list);
    for (int i = 0; i < dev_num; i++) {
      esp_ble_remove_bond_device(dev_list[i].bd_addr);
    }
    free(dev_list);
  }
}

void Bluetooth::setDiscoverability(bool discoverability) {
  Logger::debug((String("[Bluetooth::setDiscoverability()] Setting discoverability to ") + String(discoverability)).c_str());
  Bluetooth::getBleServer()->getAdvertising()->setScanFilter(!discoverability, false);
}

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

  BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
  BLEDevice::setSecurityCallbacks(new CustomBLESecurity());

  bleServer = BLEDevice::createServer();
  BLEService *pService = bleServer->createService(BLEUUID((const char *)SERVICE_UUID), 100);

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

  reloadValues();
  pService->start();
  BLEAdvertising *pAdvertising = bleServer->getAdvertising();
  pAdvertising->start();

  esp_ble_auth_req_t auth_req = ESP_LE_AUTH_BOND;
  esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));

  if (esp_ble_get_bond_device_num() > 0) {
    Logger::debug("[Bluetooth::start()] One device is already bonded");
    Bluetooth::setDiscoverability(false);
  } else {
    Logger::debug("[Bluetooth::start()] There are no bonded device");
    Bluetooth::setDiscoverability(true);
  }
}

String Bluetooth::getLastOperationStatus() {
  return lastOperatioinState;
}

void Bluetooth::setLastOperationStatus(String operationStatus) {
  lastOperatioinState = operationStatus;
}

String Bluetooth::getMAC() {
  String mac;
  const uint8_t *point = esp_bt_dev_get_address();
  for (int i = 0; i < 6; i++) {
    char chunk[3];
    sprintf(chunk, "%02X", (int)point[i]);
    mac += chunk;
    if (i < 5) mac += ":";
  }
  return mac;
}
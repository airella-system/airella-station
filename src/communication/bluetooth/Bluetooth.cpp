#include "communication/bluetooth/Bluetooth.h"
#include "communication/bluetooth/BLECallbacks.h"
#include "config/Config.h"
#include "maintenance/Logger.h"

const uint32_t Bluetooth::W_PROPERTY = NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_ENC;
const uint32_t Bluetooth::R_PROPERTY = NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC;
const uint32_t Bluetooth::RW_PROPERTY =
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_ENC;

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
BluetoothChunker *Bluetooth::gsmUrlCharacteristic = nullptr;
BluetoothChunker *Bluetooth::gsmConfigCharacteristic = nullptr;

BluetoothChunker *Bluetooth::registerTokenCharacteristic = nullptr;
BluetoothChunker *Bluetooth::apiUrlCharacteristic = nullptr;
BluetoothChunker *Bluetooth::refreshDeviceCharacteristic = nullptr;
BluetoothChunker *Bluetooth::clearDataCharacteristic = nullptr;

BluetoothChunker *Bluetooth::registrationStateCharacteristic = nullptr;
BluetoothChunker *Bluetooth::inetConnStateCharacteristic = nullptr;
BluetoothChunker *Bluetooth::deviceStateCharacteristic = nullptr;

String Bluetooth::lastOperatioinState = String();

class CustomBLESecurity : public NimBLESecurityCallbacks {
  uint32_t onPassKeyRequest() { return 0; }

  void onPassKeyNotify(uint32_t pass_key) {}

  bool onConfirmPIN(uint32_t pass_key) { return true; }

  bool onSecurityRequest() { return true; }

  void onAuthenticationComplete(ble_gap_conn_desc *cmpl) {
    ble_addr_t peer_id_addrs[MYNEWT_VAL(BLE_STORE_MAX_BONDS)];
    int num_peers;
    int rc = ble_store_util_bonded_peers(&peer_id_addrs[0], &num_peers, MYNEWT_VAL(BLE_STORE_MAX_BONDS));

    if (rc != 0) {
      Logger::debug("[Bluetooth::onAuthenticationComplete()] Unknown error");
      return;
    }
    if (ble_addr_cmp(&peer_id_addrs[0], &cmpl->peer_id_addr) == 0) {
      Logger::debug(
          "[Bluetooth::onAuthenticationComplete()] Authenticating user that already authenticated - do nothing");
      return;
    }
    if (cmpl->sec_state.bonded && num_peers >= 1) {
      Logger::debug("[Bluetooth::onAuthenticationComplete()] Already added device - removing bond request");
      ble_gap_unpair(&cmpl->peer_id_addr);
    } else if (cmpl->sec_state.bonded) {
      Logger::debug("[Bluetooth::onAuthenticationComplete()] Added new device");
      Bluetooth::setDiscoverability(false);
    }
  }
};

BLEServer *Bluetooth::getBleServer() {
  return bleServer;
}

void Bluetooth::setDiscoverability(bool discoverability) {
  Logger::debug(
      (String("[Bluetooth::setDiscoverability()] Setting discoverability to ") + String(discoverability)).c_str());
  Bluetooth::getBleServer()->getAdvertising()->setScanFilter(!discoverability, false);
}

void Bluetooth::reloadValues() {
  ssidCharacteristic->setValue(Config::getWifiSsid().c_str());
  wifiPassCharacteristic->setValue(Config::getWifiPassword().c_str());
  gsmUrlCharacteristic->setValue(Config::getGsmExtenderUrl().c_str());
  gsmConfigCharacteristic->setValue(Config::getGsmConfig().c_str());
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
  BLEDevice::setSecurityAuth(true, true, true);
  BLEDevice::setSecurityCallbacks(new CustomBLESecurity());

  bleServer = BLEDevice::createServer();
  bleServer->getAdvertising()->addServiceUUID(SERVICE_UUID);
  
  BLEService *pService = bleServer->createService(BLEUUID((const char *)SERVICE_UUID), 100);

  inetConnTypeCharacteristic = new BluetoothChunker(pService, INTERNET_CONNECTION_TYPE_CUUID, RW_PROPERTY);
  inetConnTypeCharacteristic->setCallback(new InetConnTypeCallback());

  ssidCharacteristic = new BluetoothChunker(pService, SSID_CUUID, RW_PROPERTY);
  ssidCharacteristic->setCallback(new WifiSsidCallback());

  wifiPassCharacteristic = new BluetoothChunker(pService, WIFI_PASWORD_CUUID, W_PROPERTY);
  wifiPassCharacteristic->setCallback(new WifiPasswordCallback());

  registerTokenCharacteristic = new BluetoothChunker(pService, REGISTRATION_TOKEN_CUUID, W_PROPERTY);
  registerTokenCharacteristic->setCallback(new RegistrationTokenCallback());

  gsmUrlCharacteristic = new BluetoothChunker(pService, GSM_EXTENDER_URL_CUUID, RW_PROPERTY);
  gsmUrlCharacteristic->setCallback(new GsmUrlCallback());

  gsmConfigCharacteristic = new BluetoothChunker(pService, GSM_CONFIG_CUUID, RW_PROPERTY);
  gsmConfigCharacteristic->setCallback(new GsmConfigCallback());

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

  reloadValues();
  pService->start();
  BLEAdvertising *pAdvertising = bleServer->getAdvertising();
  pAdvertising->start();

  ble_addr_t peer_id_addrs[MYNEWT_VAL(BLE_STORE_MAX_BONDS)];
  int num_peers;

  int rc = ble_store_util_bonded_peers(&peer_id_addrs[0], &num_peers, MYNEWT_VAL(BLE_STORE_MAX_BONDS));

  if (num_peers > 0) {
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
  std::string mac = BLEDevice::getAddress();
  return String(mac.c_str());
}
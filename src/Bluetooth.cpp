#include "Bluetooth.h"
#include "Config.h"

BluetoothHandler *Bluetooth::bluetoothHandler = nullptr;
BLECharacteristic *Bluetooth::devPasswordCharacteristic = nullptr;
BLECharacteristic *Bluetooth::inetConnTypeCharacteristic = nullptr;
BLECharacteristic *Bluetooth::ssidCharacteristic = nullptr;
BLECharacteristic *Bluetooth::wifiPassCharacteristic = nullptr;
BLECharacteristic *Bluetooth::registerTokenCharacteristic = nullptr;
BLECharacteristic *Bluetooth::apiUrlCharacteristic = nullptr;
BLECharacteristic *Bluetooth::devStateCharacteristic = nullptr;
BLECharacteristic *Bluetooth::connStateCharacteristic = nullptr;
BLECharacteristic *Bluetooth::refreshDeviceCharacteristic = nullptr;

class MySecurity : public BLESecurityCallbacks
{
    uint32_t onPassKeyRequest()
    {
        return 123456;
    }

    void onPassKeyNotify(uint32_t pass_key)
    {
    }

    bool onConfirmPIN(uint32_t pass_key)
    {
        vTaskDelay(5000);
        return true;
    }

    bool onSecurityRequest()
    {
        return true;
    }

    void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl)
    {
    }
};

class WifiSsidCallback : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string value = pCharacteristic->getValue();
        String stringValue = String(value.c_str());
        Config::instance().setWifiSsid(stringValue);
        Config::instance().save();
    }
};

class WifiPasswordCallback : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string value = pCharacteristic->getValue();
        String stringValue = String(value.c_str());
        Config::instance().setWifiPassword(stringValue);
        Config::instance().save();
    }
};

class RefreshDeviceCallback : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        Bluetooth::bluetoothHandler->deviceRefreshRequest();
    }
};

void Bluetooth::start(BluetoothHandler *bluetoothHandler)
{
    Bluetooth::bluetoothHandler = bluetoothHandler;
    BLEDevice::init("Airella Station");
    BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
    BLEDevice::setSecurityCallbacks(new MySecurity());
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(BLEUUID((const char *)SERVICE_UUID), 30);

    devPasswordCharacteristic = pService->createCharacteristic(
        DEVICE_PASSWORD_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE);
    //devPasswordCharacteristic->setCallbacks(new WifiSsidCallback());

    inetConnTypeCharacteristic = pService->createCharacteristic(
        INTERNET_CONNECTION_TYPE_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    //inetConnTypeCharacteristic->setCallbacks(new MyCallbacks());

    ssidCharacteristic = pService->createCharacteristic(
        SSID_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    ssidCharacteristic->setCallbacks(new WifiSsidCallback());
    ssidCharacteristic->setValue(Config::instance().getWifiSsid().c_str());

    wifiPassCharacteristic = pService->createCharacteristic(
        WIFI_PASWORD_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE);
    wifiPassCharacteristic->setCallbacks(new WifiPasswordCallback());
    wifiPassCharacteristic->setValue(Config::instance().getWifiPassword().c_str());

    registerTokenCharacteristic = pService->createCharacteristic(
        REGISTRATION_TOKEN_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE);
    //registerTokenCharacteristic->setCallbacks(new MyCallbacks());

    apiUrlCharacteristic = pService->createCharacteristic(
        API_URL_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    //apiUrlCharacteristic->setCallbacks(new MyCallbacks());

    devStateCharacteristic = pService->createCharacteristic(
        DEVICE_STATE_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ);
    //devStateCharacteristic->setCallbacks(new MyCallbacks());

    connStateCharacteristic = pService->createCharacteristic(
        CONNECTION_STATE_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ);
    //connStateCharacteristic->setCallbacks(new MyCallbacks());

    refreshDeviceCharacteristic = pService->createCharacteristic(
        REFRESH_DEVICE_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE);
    refreshDeviceCharacteristic->setCallbacks(new RefreshDeviceCallback());

    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
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
}
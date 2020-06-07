#pragma once

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "BluetoothHandler.h"

#define SERVICE_UUID "f1eb6601-af50-4cf3-9f6e-4e1c6fb8e88c"

#define DEVICE_PASSWORD_CHARACTERISTIC_UUID                                    \
  "126b5b11-6590-4229-8026-ba30ad032133"
#define INTERNET_CONNECTION_TYPE_CHARACTERISTIC_UUID                           \
  "7d9059a5-f426-4f2f-9050-3c88036beb1b"
#define SSID_CHARACTERISTIC_UUID "45bee5c6-8043-4c6e-b497-fad68a340b70"
#define WIFI_PASWORD_CHARACTERISTIC_UUID "0851dd07-b59f-40c6-8114-357c7dff694c"
#define REGISTRATION_TOKEN_CHARACTERISTIC_UUID                                 \
  "cb520851-1fd3-446e-a590-e777ddd0232c"
#define API_URL_CHARACTERISTIC_UUID "20418184-e336-4409-a04d-61d7cf31f56b"
#define DEVICE_STATE_CHARACTERISTIC_UUID "f363fc8f-92dd-4e0b-ae26-90e3e17e6560"
#define CONNECTION_STATE_CHARACTERISTIC_UUID                                   \
  "ba71dcda-609f-4b3e-8095-11662afa4c5f"
#define REFRESH_DEVICE_CHARACTERISTIC_UUID                                     \
  "2ca7df70-42df-482e-81cf-468e0fcec5dc"
#define CLEAR_DATA_CHARACTERISTIC_UUID "9023e6f3-223d-4c6c-bd39-ebca35d7e8d0"

class Bluetooth {
public:
  static void start(BluetoothHandler *bluetoothHandler);
  static void reloadValues();
  static BluetoothHandler *bluetoothHandler;

private:
  static BLECharacteristic *devPasswordCharacteristic;
  static BLECharacteristic *inetConnTypeCharacteristic;
  static BLECharacteristic *ssidCharacteristic;
  static BLECharacteristic *wifiPassCharacteristic;
  static BLECharacteristic *registerTokenCharacteristic;
  static BLECharacteristic *apiUrlCharacteristic;
  static BLECharacteristic *devStateCharacteristic;
  static BLECharacteristic *connStateCharacteristic;
  static BLECharacteristic *refreshDeviceCharacteristic;
  static BLECharacteristic *clearDataCharacteristic;
};
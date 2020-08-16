#include "communication/bluetooth/chunker/receiver/BluetoothChunkReceiver.h"

BluetoothChunkReceiver::BluetoothChunkReceiver(BLEService* pService, const char* cuuid, const uint32_t access) {
  characteristic = pService->createCharacteristic(cuuid, access);
  characteristic->setCallbacks(new BLEChunkerReceiverCallback(this));
};

BluetoothChunkReceiver::~BluetoothChunkReceiver() {
  free(characteristic);
};

bool BluetoothChunkReceiver::startTransaction() {
  if(activeTransaction) {
    unsigned long currentTimestamp = millis();
    if(abs(currentTimestamp - timestamp) > BT_CHUNKER_TIMEOUT * 1000) {
      buffer.clear();
      activeTransaction = false;
    }
    else {
      Logger::debug("[BluetoothChunkReceiver::startTransaction()] Unable to start transaction");
      return false;
    }
  }

  Logger::debug("[BluetoothChunkReceiver::startTransaction()] Started transaction");
  activeTransaction = true;
  timestamp = millis();
  return true;
}

void BluetoothChunkReceiver::endTransaction() {
  Logger::debug("[BluetoothChunkReceiver::endTransaction()] Ended transaction");
  activeTransaction = false;
  buffer.clear();
  value = buildData();
  callback->callback();
}

bool BluetoothChunkReceiver::add(std::string data) {
  if(!activeTransaction) return false;

  if(buffer.size() < maxBufferSize) {
    buffer.push_back(data);
    timestamp = millis();
    return true;
  }
  return false;
}

std::string BluetoothChunkReceiver::buildData() {
  if(buffer.empty()) return "";

  std::string data = "";
  std::vector<std::string>::iterator it = buffer.begin();
  while (it != buffer.end()) {
    data += *it;
    ++it;
  }

  return data;
}

void BluetoothChunkReceiver::setPermission(esp_gatt_perm_t perm) {
  characteristic->setAccessPermissions(perm);
}

bool BluetoothChunkReceiver::setValue(std::string value) {
  if(activeTransaction) {
    Logger::debug("[BluetoothChunkReceiver::setValue()] Unable to set value");
    return false;
  }
  characteristic->setValue(value);
  return true;
}

std::string BluetoothChunkReceiver::getValue() {
  return value;
}

void BluetoothChunkReceiver::setCallback(BluetoothChunkCallback<BluetoothChunkReceiver>*_callback) {
  callback = _callback;
  callback->setChunker(this);
}

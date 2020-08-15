#include "communication/bluetooth/chunker/sender/BluetoothChunkSender.h"

BluetoothChunkSender::BluetoothChunkSender(BLEService* pService, const char* cuuid, const uint32_t access) {
  characteristic = pService->createCharacteristic(cuuid, access);
//   characteristic->setCallbacks(new BLEChunkerReceiverCallback(this));
};

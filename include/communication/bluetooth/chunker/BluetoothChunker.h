#pragma once

#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <iterator>
#include <BLEDevice.h>
#include "maintenance/Logger.h"
#include "communication/bluetooth/chunker/BluetoothChunkCallback.h"

#define BT_CHUNKER_TIMEOUT 10 // timeout in sec
#define BT_MTU 20 // in bytes

class BluetoothChunker {

public:
	BluetoothChunker() {};
	~BluetoothChunker() {};

	virtual bool startTransaction() = 0;
  virtual void endTransaction() = 0;
	virtual bool setValue(std::string value) = 0;
  virtual std::string getValue() = 0;

	bool isActiveTransaction() {
		return activeTransaction;
	};

protected:
	bool activeTransaction = false;
	BLECharacteristic* characteristic;
	std::string value;

};

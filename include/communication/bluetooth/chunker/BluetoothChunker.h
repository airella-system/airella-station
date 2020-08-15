#pragma once

#define BT_CHUNKER_TIMEOUT 10 // timeout in sec

class BluetoothChunker {
public:
	BluetoothChunker() {};
	~BluetoothChunker() {};

	bool activeTransaction = false;
};
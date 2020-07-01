#pragma once

struct WeatherSensorConfig {
	unsigned char i2cBusNum = 1;
	unsigned char sensorAddress = 0x76;
	unsigned char dataLinePin = 21;
	unsigned char clockLinePin = 22;
};

struct AirSensorConfig {
	unsigned char serialNumber = 1;
	unsigned char powerPin = 26;
	unsigned char uartRx = 2;
	unsigned char uartTx = 13;
};

struct PowerSensorConfig {};
struct HeaterConfig {};
struct StorageConfig {};

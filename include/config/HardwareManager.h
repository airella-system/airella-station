#pragma once

struct WeatherSensorConfig {};

struct AirSensorConfig {
	unsigned char serialNumber = 1;
	unsigned char powerPin = 26;
	unsigned char uartRx = 2;
	unsigned char uartTx = 13;
};

struct PowerSensorConfig {};
struct HeaterConfig {};
struct StorageConfig {};

class HardwareManager {
public:
  	static WeatherSensorConfig getWeatherSensorConfig();
  	static AirSensorConfig getAirSensorConfig();
  	static PowerSensorConfig getPowerSensorConfig();
  	static HeaterConfig getHeaterConfig();
  	static StorageConfig getStorageConfig();
};
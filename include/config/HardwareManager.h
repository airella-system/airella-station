#pragma once

struct WeatherSensorConfig {};
struct AirSensorConfig {};
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
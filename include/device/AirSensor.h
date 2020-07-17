#pragma once

#include "config/HardwareConfig.h"
#include "device/Sensor.h"

#define AIR_SENSOR_DATA_SIZE 32  // 0x42 + 0x4d  + 28 bytes data + 2 bytes checksum = 32 bytes

struct AirSensorMeasurement {
  uint8_t start_1;        // Start char 1 0x42 (fixed)
  uint8_t start_2;        // Start char 2 0x4d (fixed)
  uint16_t frame_length;  // Frame length = 2x13 + 2 (data + parity)

  uint16_t pm1;    // PM1.0 concentration (CF = 1, standard particles) Unit ug/m^3
  uint16_t pm2_5;  // PM2.5 concentration (CF = 1, standard particulates) Unit ug/m^3
  uint16_t pm10;   // PM10 concentration (CF = 1, standard particulate matter)
                   // Unit ug/m^3

  uint16_t pm_1_0_atmos;   // PM1.0 concentration (in the atmosphere) Unit ug/m^3
  uint16_t pm_2_5_atmos;   // PM2.5 concentration (in the atmosphere) Unit ug/m^3
  uint16_t pm_10_0_atmos;  // PM10 concentration (in the atmosphere) Unit ug/m^3

  uint16_t raw_gt_0_3;   // Particles in 0.1 liter of air > 0.3um
  uint16_t raw_gt_0_5;   // Particles in 0.1 liter of air > 0.5um
  uint16_t raw_gt_1_0;   // Particles in 0.1 liter of air > 1.0um
  uint16_t raw_gt_2_5;   // Particles in 0.1 liter of air > 2.5um
  uint16_t raw_gt_5_0;   // Particles in 0.1 liter of air > 5.0um
  uint16_t raw_gt_10_0;  // Particles in 0.1 liter of air > 10um

  uint8_t version_number;  // Version number
  uint8_t error_code;      // Error code

  uint16_t checksum;  // Sum of each byte from start_1 ... error_code
};

union AirSensorBuffer {
  unsigned char bytes[AIR_SENSOR_DATA_SIZE];
  uint16_t words[AIR_SENSOR_DATA_SIZE / 2];
  AirSensorMeasurement values;
};

class AirSensor : public Sensor {
 public:
  AirSensor();
  ~AirSensor();
  void powerOn();
  void powerOff();
  void calibrate();
  void measurement();
  void singleMeasurement(bool logging /* = true */);
  AirSensorMeasurement getMeasurementData() const;
  uint16_t getPM1() const;
  uint16_t getPM2_5() const;
  uint16_t getPM10() const;
  uint16_t getPM_1_0_atmos() const;
  uint16_t getPM_2_5_atmos() const;
  uint16_t getPM_10_0_atmos() const;
  uint16_t getRawGreaterThan_0_3() const;
  uint16_t getRawGreaterThan_0_5() const;
  uint16_t getRawGreaterThan_1_0() const;
  uint16_t getRawGreaterThan_2_5() const;
  uint16_t getRawGreaterThan_5_0() const;
  uint16_t getRawGreaterThan_10_0() const;
  uint8_t getHWVersion() const;
  uint8_t getErrorCode() const;

 private:
  bool isPowerOn;
  HardwareSerial serial;
  AirSensorBuffer sensorBuffer;
  AirSensorConfig config;
  bool dataReady;
  unsigned char lastByte;
  unsigned char nextByte;
  int bufferIndex;

  void updateBuffer();
  void convertSensorData();
  bool isValidChecksum();
  uint16_t uint16FromBufferData(unsigned char *buff, int loc);
  void dumpBytes() const;
};
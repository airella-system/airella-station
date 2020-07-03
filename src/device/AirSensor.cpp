#include "device/AirSensor.h"

AirSensor::AirSensor() : serial(config.serialNumber) {
  Logger::info("[AirSensor] Initalizing ...");
  isPowerOn = false;
  dataReady = false;
  lastByte = 0;
  nextByte = 0;
  powerPin = 0;
  bufferIndex = 0;

  pinMode(config.powerPin, OUTPUT);
  digitalWrite(config.powerPin, LOW);
  serial.begin(9600, SERIAL_8N1, config.uartRx, config.uartTx, false, 1000);

  initialized = true;
  Logger::info("[AirSensor] Air sensor is active.");
}

AirSensor::~AirSensor() {
  serial.end();
  Logger::info("[AirSensor] Air sensor is close.");
}

void AirSensor::powerOn() {
  Logger::info("[AirSensor] Air sensor power ON.");
  digitalWrite(powerPin, HIGH);
}

void AirSensor::powerOff() {
  Logger::info("[AirSensor] Air sensor power OFF.");
  digitalWrite(powerPin, LOW);
}

AirSensorMeasurement AirSensor::getMeasurementData() const {
  return sensorBuffer.values;
}

void AirSensor::measurement() {
  Logger::info("[AirSensor] New measurement.");
  for (int i = 0; i < AIR_SENSOR_DATA_SIZE; i++) {
    updateBuffer();
  }
}

void AirSensor::updateBuffer() {
  dataReady = false;
  if (serial.available()) {
    nextByte = serial.read();

    if (nextByte == 0x4d && lastByte == 0x42) {
      sensorBuffer.bytes[0] = 0x42;
      bufferIndex = 1;
    }
    sensorBuffer.bytes[bufferIndex] = nextByte;
    bufferIndex++;
    lastByte = nextByte;
  }

  if (bufferIndex == AIR_SENSOR_DATA_SIZE) {
    if (sensorBuffer.bytes[0] == 0x42 && sensorBuffer.bytes[1] == 0x4d) {
      convertSensorData();

      if (isValidChecksum()) {
        dataReady = true;
      } else {
        Logger::info("[AirSensor] Invalid data checksum");
      }
    } else {
      Logger::info("[AirSensor] Malformed first byte");
    }
    bufferIndex = 0;
  }
}

void AirSensor::convertSensorData() {
  int loc = 0;
  // first word (bytes 0 and 1) are 8 bit
  loc += 1;
  // next 13 words are 16-bit ints
  for (int i = 0; i < 13; i++) {
    sensorBuffer.words[loc] = uint16FromBufferData(sensorBuffer.bytes, loc * 2);
    loc++;
  }
  // next word is 2 8-bit bytes
  loc += 1;
  // final checksum word is 16-bit
  sensorBuffer.words[loc] = uint16FromBufferData(sensorBuffer.bytes, loc * 2);
}

bool AirSensor::isValidChecksum() {
  uint16_t sum = 0;
  for (int i = 0; i < (AIR_SENSOR_DATA_SIZE - 2); i++) {
    sum += sensorBuffer.bytes[i];
  }

  if (sum == sensorBuffer.values.checksum) {
    return true;
  }

  return false;
}

uint16_t AirSensor::uint16FromBufferData(unsigned char *buff, int loc) {
  if (loc > (AIR_SENSOR_DATA_SIZE - 2) || loc < 2) {
    return -1;
  }
  return ((buff[loc] << 8) + buff[loc + 1]);
}

void AirSensor::dumpBytes() const {
  for (int i = 0; i < AIR_SENSOR_DATA_SIZE; i++) {
    String out(sensorBuffer.bytes[i]);
    Logger::debug(&out);
  }
}

uint16_t AirSensor::getPM1() const {
  return sensorBuffer.values.pm1;
}

uint16_t AirSensor::getPM2_5() const {
  return sensorBuffer.values.pm2_5;
}

uint16_t AirSensor::getPM10() const {
  return sensorBuffer.values.pm10;
}

uint16_t AirSensor::getPM_1_0_atmos() const {
  return sensorBuffer.values.pm_1_0_atmos;
}

uint16_t AirSensor::getPM_2_5_atmos() const {
  return sensorBuffer.values.pm_2_5_atmos;
}

uint16_t AirSensor::getPM_10_0_atmos() const {
  return sensorBuffer.values.pm_10_0_atmos;
}

uint16_t AirSensor::getRawGreaterThan_0_3() const {
  return sensorBuffer.values.raw_gt_0_3;
}

uint16_t AirSensor::getRawGreaterThan_0_5() const {
  return sensorBuffer.values.raw_gt_0_5;
}

uint16_t AirSensor::getRawGreaterThan_1_0() const {
  return sensorBuffer.values.raw_gt_1_0;
}

uint16_t AirSensor::getRawGreaterThan_2_5() const {
  return sensorBuffer.values.raw_gt_2_5;
}

uint16_t AirSensor::getRawGreaterThan_5_0() const {
  return sensorBuffer.values.raw_gt_5_0;
}

uint16_t AirSensor::getRawGreaterThan_10_0() const {
  return sensorBuffer.values.raw_gt_10_0;
}

uint8_t AirSensor::getHWVersion() const {
  return sensorBuffer.values.version_number;
}

uint8_t AirSensor::getErrorCode() const {
  return sensorBuffer.values.error_code;
}
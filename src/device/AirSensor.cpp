#include "device/AirSensor.h"
#include "config/PinManager.h"
#include "maintaince/Logger.h"

bool AirSensor::initialized = false;
bool AirSensor::isPowerOn = false;
AirSensorBuffer AirSensor::sensorBuffer;
HardwareSerial AirSensor::serial = HardwareSerial(1);
bool AirSensor::dataReady = false;
unsigned char AirSensor::lastByte = 0;
unsigned char AirSensor::nextByte = 0;
int AirSensor::bufferIndex = 0;

void AirSensor::init() {
  if (initialized) {
    Logger::warning("[AirSensor] Air sensor is already initialized.");
  }

  pinMode(PinManager::getAirSensorLed(), OUTPUT);
  pinMode(PinManager::getAirSensorPowerPin(), OUTPUT);
  digitalWrite(PinManager::getAirSensorPowerPin(), LOW);

  UartConfig config = PinManager::getUartForAirSensor();
  serial.begin(9600, SERIAL_8N1, config.rx, config.tx, false, 1000);
  initialized = true;
  Logger::info("[AirSensor] Air sensor is initialized.");
}

void AirSensor::powerOn() {
  Logger::info("[AirSensor] Air sensor power ON.");
  digitalWrite(PinManager::getAirSensorPowerPin(), HIGH);
}

void AirSensor::powerOff() {
  Logger::info("[AirSensor] Air sensor power OFF.");
  digitalWrite(PinManager::getAirSensorPowerPin(), LOW);
}

bool AirSensor::isInit() { return initialized; }

AirSensorMeasurement AirSensor::getMeasurementData() {
  return sensorBuffer.values;
}

void AirSensor::measurement() {
  Logger::info("[AirSensor] New measurement.");
  for (int i = 0; i < AIR_SENSOR_DATA_SIZE; i++) {
    updateBuffer();
  }
}

void AirSensor::updateBuffer() {
  if (!AirSensor::initialized) {
    Logger::warning("[AirSensor] Error: must call AirSensor::init()");
    return;
  }

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

void AirSensor::dumpBytes() {
  for (int i = 0; i < AIR_SENSOR_DATA_SIZE; i++) {
    String out(sensorBuffer.bytes[i]);
    Logger::debug(&out);
  }
}

uint16_t AirSensor::getPM1() { return sensorBuffer.values.pm1; }

uint16_t AirSensor::getPM2_5() { return sensorBuffer.values.pm2_5; }

uint16_t AirSensor::getPM10() { return sensorBuffer.values.pm10; }

uint16_t AirSensor::getPM_1_0_atmos() {
  return sensorBuffer.values.pm_1_0_atmos;
}

uint16_t AirSensor::getPM_2_5_atmos() {
  return sensorBuffer.values.pm_2_5_atmos;
}

uint16_t AirSensor::getPM_10_0_atmos() {
  return sensorBuffer.values.pm_10_0_atmos;
}

uint16_t AirSensor::getRawGreaterThan_0_3() {
  return sensorBuffer.values.raw_gt_0_3;
}

uint16_t AirSensor::getRawGreaterThan_0_5() {
  return sensorBuffer.values.raw_gt_0_5;
}

uint16_t AirSensor::getRawGreaterThan_1_0() {
  return sensorBuffer.values.raw_gt_1_0;
}

uint16_t AirSensor::getRawGreaterThan_2_5() {
  return sensorBuffer.values.raw_gt_2_5;
}

uint16_t AirSensor::getRawGreaterThan_5_0() {
  return sensorBuffer.values.raw_gt_5_0;
}

uint16_t AirSensor::getRawGreaterThan_10_0() {
  return sensorBuffer.values.raw_gt_10_0;
}

uint8_t AirSensor::getHWVersion() { return sensorBuffer.values.version_number; }

uint8_t AirSensor::getErrorCode() { return sensorBuffer.values.error_code; }
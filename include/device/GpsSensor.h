#pragma once

#include "config/HardwareConfig.h"
#include "device/Sensor.h"

class GpsSensor : public Sensor {
 public:
  GpsSensor(HardwareSerial* serial);
  ~GpsSensor();
  void setupSerial();
  bool fetchLocation();
  double getLatitude();
  double getLongitude();

 private:
  HardwareSerial* serial;
  unsigned char bufferIterator = 0;
  unsigned long startFetchingTime;
  static const int FETCH_LOCATION_TIMEOUT = 10000;
  static const int BUFFER_SIZE = 83; //NMEA 0183 has messages with 82 size, but we need 1 more character for NULL
  char buffer[BUFFER_SIZE];

  double latitude;
  double longitude;

  float nmea_longitude;
  float nmea_latitude;    
  float utc_time;
  char ns, ew;
  int lock;
  int satelites;
  float hdop;
  float msl_altitude;
  char msl_units;

  double getFloatFromDecimalInteger(int integer);

  bool fetchHeaderInFetchLocationFlow();
  bool fetchMessageTypeInFetchLocationFlow();
  bool checkMessageTypeInFetchLocationFlow();
  bool fetchRestMessageInFetchLocationFlow();
  bool readMessageInFetchLocationFlow();
};
#include "device/GpsSensor.h"

GpsSensor::GpsSensor(HardwareSerial* serial) {
  Logger::info("[GpsSensor] Initalizing ...");
  setTextState("GPS|INIT");
  this->serial = serial;
  initialized = true;
  Logger::info("[GpsSensor] GPS is active.");
}

void GpsSensor::setupSerial() {
  this->serial->updateBaudRate(4800);
}

bool GpsSensor::fetchLocation() {
  Logger::info("[GpsSensor] Fetching location...");

  this->startFetchingTime = millis();

  while ((millis() - startFetchingTime) < FETCH_LOCATION_TIMEOUT) {
    if (fetchHeaderInFetchLocationFlow()) {
      return true;
    }
  }
  setTextState("GPS|ERROR");
  return false;
}

bool GpsSensor::fetchHeaderInFetchLocationFlow() {
  Logger::info("[GpsSensor] Fetching next header...");
  while ((millis() - startFetchingTime) < FETCH_LOCATION_TIMEOUT) {
    if (serial->available()) {
      if (serial->read() == '$') {
        this->bufferIterator = 0;
        this->buffer[this->bufferIterator++] = '$';
        return fetchMessageTypeInFetchLocationFlow();
      }
    }
  }
  return false;
}

bool GpsSensor::fetchMessageTypeInFetchLocationFlow() {
  Logger::info("[GpsSensor] Fetching message type...");
  int letters = 0;
  while ((millis() - startFetchingTime) < FETCH_LOCATION_TIMEOUT) {
    if (serial->available()) {
      char letter = serial->read();
      this->buffer[this->bufferIterator++] = letter;
      letters++;
      if (letters == 5) {
        return checkMessageTypeInFetchLocationFlow();
      }
    }
  }
  return false;
}

bool GpsSensor::checkMessageTypeInFetchLocationFlow() {
  Logger::info("[GpsSensor] Checking message type...");
  char const* title = "$GPGGA";
  if (strncmp(this->buffer, title, strlen(title)) == 0) {
    return fetchRestMessageInFetchLocationFlow();
  }
  Logger::info("[GpsSensor] Desired message type not found...");
  return false;
}

bool GpsSensor::fetchRestMessageInFetchLocationFlow() {
  Logger::info("[GpsSensor] Fetching rest of message...");
  while ((millis() - startFetchingTime) < FETCH_LOCATION_TIMEOUT) {
    if (serial->available()) {
      char letter = serial->read();
      this->buffer[this->bufferIterator++] = letter;
      if (letter == '\n') {
        return readMessageInFetchLocationFlow();
      } else if (this->bufferIterator >= BUFFER_SIZE) {
        return false;
      }
    }
  }
  return false;
}

bool GpsSensor::readMessageInFetchLocationFlow() {
  Logger::info("[GpsSensor] Reading message...");
  this->buffer[this->bufferIterator] = '\0';
  Logger::info((String("[GpsSensor] GPGGA message: ") + String(this->buffer)).c_str());

  int utcTime_HHMMSS;
  int utcTimeDecimal_SS;
  int latitude_DDmm;
  int latitudeDecimal_mm;
  char latitideDirection;
  int longitude_DDDmm;
  int longitudeDecimal_mm;
  char longitudeDirection;
  int quality;
  int numOfSatelites;
  float horizontalDilution;
  float altitude;
  char altitudeUnits;
  float undulation;
  char undulationUnits;
  int checkSum;

  if (sscanf(this->buffer, "$GPGGA,%d.%d,%d.%d,%c,%d.%d,%c,%d,%d,%f,%f,%c,%f,%c,%*[^*]*%d", &utcTime_HHMMSS,
             &utcTimeDecimal_SS, &latitude_DDmm, &latitudeDecimal_mm, &latitideDirection, &longitude_DDDmm,
             &longitudeDecimal_mm, &longitudeDirection, &quality, &numOfSatelites, &horizontalDilution, &altitude,
             &altitudeUnits, &undulation, &undulationUnits, &checkSum) >= 1) {

    double wut = ((latitude_DDmm % 100) + getFloatFromDecimalInteger(latitudeDecimal_mm));
    this->latitude =
        (latitude_DDmm / 100) + ((latitude_DDmm % 100) + getFloatFromDecimalInteger(latitudeDecimal_mm)) / 60.0;
    this->longitude =
        (longitude_DDDmm / 100) + ((longitude_DDDmm % 100) + getFloatFromDecimalInteger(longitudeDecimal_mm)) / 60.0;

    if (latitideDirection == 'S') {
      this->latitude = -this->latitude;
    }
    if (longitudeDirection == 'W') {
      this->longitude = -this->longitude;
    }

    Logger::info(
        (String("[GpsSensor] Final latitude and longitude: ") + String(this->latitude, 10) + ", " + String(this->longitude, 10))
            .c_str());

    return true;
  }

  return false;
}

// convert integer 123 to 0.123 double
double GpsSensor::getFloatFromDecimalInteger(int integer) {
  int nDigits = floor(log10(abs(integer))) + 1;
  int power = pow(10, nDigits);
  return integer / (double) power;
}

double GpsSensor::getLatitude() {
  return this->latitude;
}

double GpsSensor::getLongitude() {
  return this->longitude;
}
#include "device/Storage.h"

void Storage::tryToInit() {
  if(initialized) return;
  Logger::info("[Storage] Initalizing ...");

  if (!SD.begin()) {
    Logger::debug("Card Mount Failed");
    Logger::warning("[Storage] SD mount failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Logger::warning("[Storage] No SD card attached");
    return;
  }

  initialized = true;
  Logger::info("[Storage] Initalized is OK.");
}

Storage::~Storage() {
  SD.end();
}

FS *Storage::getStorage() {
  return &SD;
}

bool Storage::write(const char *message, const char *path, const char *mode, bool logging /* = true */) const {
  File file = SD.open(path, mode);
  if (!file) {
    if (logging) Logger::warning("[Storage] unable to open file for writing");
    return false;
  }
  if (!file.print(message)) {
    if (logging) Logger::warning("[Storage] unable write to file");
    return false;
  }
  file.close();
  return true;
};

bool Storage::write(const char *message, const char *path, bool logging /* = true */) const {
  return write(message, path, FILE_WRITE, logging);
}

bool Storage::append(const char *message, const char *path, bool logging /* = true */) const {
  return write(message, path, FILE_APPEND, logging);
}

String Storage::read(const char *path, bool logging /* = true */) const {
  File file = SD.open(path);
  if (!file) {
    if (logging) Logger::warning("[Storage] unable to open file");
    return String();
  }
  String fileContent = file.readString();
  file.close();
  return fileContent;
}

bool Storage::remove(const char *path) const {
  return SD.remove(path);
}

unsigned int Storage::getFileSize(const char *path) const {
  File file = SD.open(path);
  unsigned int fileSize = file.size();
  file.close();
  return fileSize;
}

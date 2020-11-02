#include "util/DataPersister.h"
#include "device/DeviceContainer.h"

DataPersister::DataPersister() {
  DeviceContainer.storage->tryToInit();
  storage = DeviceContainer.storage->getStorage();
  if(!storage->exists("/measurements")) {
    storage->mkdir("/measurements");
  }
  if(!storage->exists("/logs")) {
    storage->mkdir("/logs");
  }
}

void DataPersister::saveMeasurement(String& sensor, String data) {
  Logger::debug("[DataPersister::saveMeasurement()] Save new measurement data.");
  data = timeProvider.getDataTime().toString() + ": [" + sensor + "]" + data + "\n";
  DeviceContainer.storage->append(
    data.c_str(), 
    ("/measurements/" + timeProvider.getDate().toString() + ".txt").c_str()
  );
}

void DataPersister::saveLog(String& logMessage) {
  DeviceContainer.storage->append(
    logMessage.c_str(), 
    ("/logs/" + timeProvider.getDate().toString() + ".txt").c_str()
  );
}

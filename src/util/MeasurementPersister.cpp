#include "util/MeasurementPersister.h"
#include "device/DeviceContainer.h"

MeasurementPersister::MeasurementPersister() {
  DeviceContainer.storage->tryToInit();
  storage = DeviceContainer.storage->getStorage();
  if(!storage->exists("/measurements")) {
    storage->mkdir("/measurements");
  }
}

void MeasurementPersister::save(String& sensor, String data) {
  Logger::debug("[MeasurementPersister::save()] Save new measurement data.");
  data = timeProvider.getDataTime().toString() + ": [" + sensor + "]" + data + "\n";
  DeviceContainer.storage->append(
    data.c_str(), 
    ("/measurements/" + timeProvider.getDate().toString() + ".txt").c_str()
  );
}

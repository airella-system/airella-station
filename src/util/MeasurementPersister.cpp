#include "util/MeasurementPersister.h"

MeasurementPersister::MeasurementPersister() {
  storage = DeviceContainer.storage->getStorage();
  if(!storage->exists("/measurements")) {
    storage->mkdir("/measurements");
  }
};

void MeasurementPersister::save(String& data) {
  String fileName = "/measurements" + timeProvider.getDate().toString();
  if()
}

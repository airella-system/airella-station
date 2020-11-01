#include "util/MeasurementPersister.h"

MeasurementPersister::MeasurementPersister() {
  storage = DeviceContainer.storage->getStorage();
  if(!storage->exists("/measurements")) {
    storage->mkdir("/measurements");
  }
};

void MeasurementPersister::save(String& data) {
  DeviceContainer.storage->append(
    data.c_str(), 
    ("/measurements" + timeProvider.getDate().toString()).c_str()
  );
}

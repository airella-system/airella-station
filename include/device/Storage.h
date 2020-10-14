#pragma once

#include "SD.h"
#include "device/Sensor.h"

class Storage : public Sensor {
public:
    Storage();
    ~Storage();
    FS *getStorage();
    bool write(const char *message, const char *path, bool logging = true) const;
    bool append(const char *message, const char *path, bool logging = true) const;
    String read(const char *path, bool logging = true) const;
    bool remove(const char *path) const;

private:
    StorageConfig config;
    bool write(const char *message, const char *path, const char *mode, bool logging = true) const;

};
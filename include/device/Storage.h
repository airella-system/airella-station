#pragma once

#include "device/Sensor.h"
#include "SD.h"

struct StorageConfig {
    const unsigned char pin = 0;
};

class Storage : public Sensor{

public:
    Storage();
    ~Storage();

private:
    StorageConfig config;

};
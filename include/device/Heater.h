#pragma once

#include "device/Sensor.h"
#include <DallasTemperature.h>

#define U1TX 13
#define OW1 15
#define HEATER_ON 25


int heater = 10;

class Heater : public Sensor {
public:
    static void init();
    static void on();
    static void off();
    static void run();
    static void stop();
    static float getTemperature();
    static bool getHeaterState();
private:
    static float tempDev;
    static bool heaterIsOn;
    static TaskHandle_t termoThread;
};

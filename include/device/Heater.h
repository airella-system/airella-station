#pragma once

#include "device/Sensor.h"
#include <DallasTemperature.h>
#include <analogWrite.h>

#define U1TX 13
#define OW1 15
#define HEATER_ON 25
#define ONE_WIRE_MAX_DEV 2

struct HeaterStatus {
    bool heaterIsOn;
    bool threadIsRunning;
};

class Heater : public Sensor {

public:
    void init();
    void on();
    void off();
    void run();
    void stop();
    float getTemperature();
    HeaterStatus getHeaterState();

private:
    static float temperatureDevice[ONE_WIRE_MAX_DEV];
    static DeviceAddress deviceAddress[ONE_WIRE_MAX_DEV];
    static int numberOfDevices;
    static HeaterStatus heaterStatus;
    static TaskHandle_t termoThread;
    static OneWire communicationBus;
    static DallasTemperature thermometer;

    static String deviceAddressToString(DeviceAddress deviceAddress);
    static void thread(void *pvParameters);

};

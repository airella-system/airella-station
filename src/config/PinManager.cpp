#include "config/PinManager.h"

UartConfig PinManager::getUartForAirSensor() {
    return UartConfig(U1RX, U1TX);
};

unsigned char PinManager::getAirSensorPowerPin() {
    return PMS7K_ON;
};

unsigned char PinManager::getAirSensorLed() {
    return LED1;
}
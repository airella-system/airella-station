#include "config/HardwareManager.h"
#include "maintenance/Logger.h"

UartConfig HardwareManager::getUartForAirSensor() { 
    if(uartForAirSensor) {
        Logger::error("[HardwareManager::getUartForAirSensor] already taken.");
        return UartConfig();
    }

    uartForAirSensor = true;
    return UartConfig(U1RX, U1TX); 
};

void HardwareManager::releaseUartForAirSensor() {
    if(!uartForAirSensor) {
        Logger::warning("[HardwareManager::releaseUartForAirSensor] already released.");
        return;
    }
    uartForAirSensor = false;
}

OptionalConfig<unsigned char> HardwareManager::getAirSensorPowerPin() { 
    return getPin(PMS7K_ON, &uartForAirSensor, "[HardwareManager::getAirSensorPowerPin] already taken.");
};

void HardwareManager::releaseAirSensorPowerPin() {
    releasePin(&airSensorPowerPin, "[HardwareManager::getAirSensorPowerPin] already released.");
}

OptionalConfig<unsigned char> HardwareManager::getAirSensorLed() { 
    return getPin(LED1, &uartForAirLed, "[HardwareManager::getAirSensorLed] already taken.");
}

void HardwareManager::releaseAirSensorLed() {
    releasePin(&uartForAirLed, "[HardwareManager::releaseAirSensorLed] already released.");
}

OptionalConfig<unsigned char> HardwareManager::getBusNumForWeatherSensor() { 
    return getPin(I2C_BUS_NO, &i2cForWeatherSensor, "[HardwareManager::getBusNumForWeatherSensor] already taken.");
}

void HardwareManager::releaseBusNumForWeatherSensor() {
    releasePin(&i2cForWeatherSensor, "[HardwareManager::getBusNumForWeatherSensor] already released.");
}

OptionalConfig<unsigned char> HardwareManager::getPin(unsigned char pin, bool *pinPtr, const char *message) {
    if(pinPtr) {
        Logger::error(message);
        return OptionalConfig<unsigned char>();
    }
    return OptionalConfig<unsigned char>(pin); 
}

void HardwareManager::releasePin(bool *pinPtr, const char *message) {
    if(!pinPtr) {
        Logger::warning(message);
        return;
    }
    *pinPtr = false;
}

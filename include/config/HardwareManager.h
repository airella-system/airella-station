#pragma once

//pm sensor
#define U1RX 2
#define U1TX 13
#define LED1 14
#define PMS7K_ON 26

//weather sensor
#define BME_SDA 21
#define BME_SCL 22
#define I2C_BUS_NO 1

struct UartConfig {
	bool isOk = false;
	unsigned char rx = -1;
	unsigned char tx = -1;

	UartConfig() {};

	UartConfig(const unsigned char _rx, const unsigned char _tx)
		: isOk(true), rx(_rx), tx(_tx) {};
};

template <class T>
struct OptionalConfig {
	bool isOk = false;
	T value = 0;

	OptionalConfig() {};

	OptionalConfig(const T _value) : isOk(true), value(_value) {};
};

class HardwareManager {
public:
	static UartConfig getUartForAirSensor();
	static void releaseUartForAirSensor();

	static OptionalConfig<unsigned char> getAirSensorPowerPin();
	static void releaseAirSensorPowerPin();

	static OptionalConfig<unsigned char> getAirSensorLed();
	static void releaseAirSensorLed();

	static OptionalConfig<unsigned char> getBusNumForWeatherSensor();
	static void releaseBusNumForWeatherSensor();

private:
	static bool uartForAirSensor;
	static bool airSensorPowerPin;
	static bool uartForAirLed;
	static bool i2cForWeatherSensor;
	static OptionalConfig<unsigned char> getPin(unsigned char pin, bool *pinStatePtr, const char *message);
	static void releasePin(bool *pinStatePtr, const char *message);
};
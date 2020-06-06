#pragma once

#define U1RX 2
#define U1TX 13
#define LED1 14
#define PMS7K_ON 26

struct UartConfig {
  unsigned char rx = -1;
  unsigned char tx = -1;

  UartConfig(const unsigned char _rx, const unsigned char _tx)
      : rx(_rx), tx(_tx) {}
};

class PinManager {
public:
  static UartConfig getUartForAirSensor();
  static unsigned char getAirSensorPowerPin();
  static unsigned char getAirSensorLed();
};
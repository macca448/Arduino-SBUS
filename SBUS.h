#ifndef SBUS_H
#define SBUS_H

#include <Arduino.h>

class SBUS {
  public:
    // Constructor: Define the Serial port to use (e.g., Serial1)
    SBUS(HardwareSerial& serial);

    // Start SBUS communication
    void begin(uint8_t rxPin, uint8_t txPin, bool inv);
    void begin(uint8_t rxPin, uint8_t txPin);
    void begin();

    // Read and process data (must always be called in loop()
    void update();

    // Read the PWM value (1000-2000 us) of each channel independently (specify channel number 1 - 10)
    uint16_t getChannel(uint8_t channel);

    // Check for safety status of lost signal
    bool isFailsafe();
    bool isFrameLost();

    private:
    HardwareSerial& _serial;
    uint8_t _buffer[25];
    int _index;

    uint16_t _channelsRaw[16];
    uint16_t _channelsPWM[16]; 
    bool _failSafe; 
    bool _frameLost; 

    void parseSBUS();
};

#endif

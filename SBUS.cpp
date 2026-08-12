#include "SBUS.h"

SBUS::SBUS(HardwareSerial& serial) : _serial(serial), _index(0), _failSafe(false), _frameLost(false) {
  memset(_channelsRaw, 0, sizeof(_channelsRaw));
  // Set the Default when there is no signal to stick in the middle (1500), and the throttle channel 3 to the lowest (1000).
  for (int i = 0; i < 16; i++) {
    _channelsPWM[i] = (i == 2) ? 1000 : 1500;
  }
}

// Option 1: The Master Function (Handles target-specific features)
void SBUS::begin(uint8_t rxPin, uint8_t txPin, bool inv) {
  #if defined(ARDUINO_ARCH_ESP32)
    // ESP32 supports custom pins and internal software inversion natively
    _serial.begin(100000, SERIAL_8E2, rxPin, txPin, inv);

  #elif defined(ARDUINO_ARCH_RP2040)
    // Pico (RP2040) supports pin remapping, but does NOT support native inversion in begin()
    // It will ignore the 'inv' flag (you will use your NPN transistor)
    _serial.setRX(rxPin);
    _serial.setTX(txPin);
    _serial.begin(100000, SERIAL_8E2);

  #else
    // AVR (Mega 2560), Teensy, STM32, etc. 
    // These boards have strict hardwired serial pins and do not accept pin parameters.
    // They safely fallback to standard 2-argument initialization.
    _serial.begin(100000, SERIAL_8E2);
  #endif
}

// Option 2: Custom pins (Defaults to true/standard inversion where supported)
void SBUS::begin(uint8_t rxPin, uint8_t txPin) {
  begin(rxPin, txPin, false);
}

// Option 3: Parameterless (Completely automatic architecture/board defaults)
void SBUS::begin() {
  #if defined(ARDUINO_ARCH_ESP32)
    // Your exact custom ESP32 board configuration
    begin(4, 25, true); 

  #elif defined(ARDUINO_ARCH_RP2040)
    // Raspberry Pi Pico standard Serial1/Serial2 default pins
    // For example, if using Serial1, it defaults to RX=1, TX=0
    begin(1, 0, false); 

  #else
    // Mega 2560 / AVR fallback
    // Since it ignores pins anyway, we pass placeholder 0s to trigger Option 1
    begin(0, 0, false); 
  #endif
}


void SBUS::update() {
  while (_serial.available() > 0) {
    uint8_t b = _serial.read();
    
    if (_index == 0 && b != 0x0F) {
      continue; 
    }
    
    _buffer[_index++] = b;
    
    if (_index == 25) {
      uint8_t footer = _buffer[24];
      if (footer == 0x00 || footer == 0x04 || footer == 0x14 || footer == 0x24 || footer == 0x34) {
        parseSBUS();
      }
      _index = 0;
    }
  }
}

void SBUS::parseSBUS() {
  _channelsRaw[0]  = ((_buffer[1]       | _buffer[2]  << 8) & 0x07FF);
  _channelsRaw[1]  = ((_buffer[2] >> 3  | _buffer[3]  << 5) & 0x07FF);
  _channelsRaw[2]  = ((_buffer[3] >> 6  | _buffer[4]  << 2 | _buffer[5] << 10) & 0x07FF);
  _channelsRaw[3]  = ((_buffer[5] >> 1  | _buffer[6]  << 7) & 0x07FF);
  _channelsRaw[4]  = ((_buffer[6] >> 4  | _buffer[7]  << 4) & 0x07FF);
  _channelsRaw[5]  = ((_buffer[7] >> 7  | _buffer[8]  << 1 | _buffer[9] << 9) & 0x07FF);
  _channelsRaw[6]  = ((_buffer[9] >> 2  | _buffer[10] << 6) & 0x07FF);
  _channelsRaw[7]  = ((_buffer[10] >> 5 | _buffer[11] << 3) & 0x07FF);
  _channelsRaw[8]  = ((_buffer[12]      | _buffer[13] << 8) & 0x07FF);
  _channelsRaw[9]  = ((_buffer[13] >> 3 | _buffer[14] << 5) & 0x07FF);

  _frameLost = _buffer[23] & 0x04;
  _failSafe  = _buffer[23] & 0x08;

  // Convert the signal range of a real remote (309-1690) to exactly 1000-2000 US.
  for (int i = 0; i < 10; i++) {
    _channelsPWM[i] = map(_channelsRaw[i], 309, 1690, 1000, 2000);
    if (_channelsPWM[i] < 1000) _channelsPWM[i] = 1000;
    if (_channelsPWM[i] > 2000) _channelsPWM[i] = 2000;
  }
}

uint16_t SBUS::getChannel(uint8_t channel) {
  // Force reading to be limited to entries 1 through 10 (reverting to index 0-9 of the array).
  if (channel < 1 || channel > 10) return 1500; 
  return _channelsPWM[channel - 1];
}

bool SBUS::isFailsafe()  { return _failSafe; }
bool SBUS::isFrameLost() { return _frameLost; }

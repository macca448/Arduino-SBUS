# SBUS Library for Arduino
Library for decoding SBUS signals from a receiver.
  
It converts the raw values ​​(309-1690) into the standard range of 1000-2000 us for up to 10 channels.

Hardware Serial only with support for ESP32, Pi-Pico and AVR's.



## How to use
### Generic
```cpp
#include <SBUS.h>
SBUS sbus(Serial1);

void setup() {
  //Uses boards library defined pins for Serial1 and no signal inversion for ESP32
  sbus.begin(); 
}
void loop() {
  sbus.update();
  uint16_t ch3 = sbus.getChannel(3);
}
```
### Re-define TX/RX pin's (ESP32 RP2040)
```cpp
#include <SBUS.h>
SBUS sbus(Serial1);

void setup() {
  //By default this enables signal inversion for ESP32
  sbus.begin(/*rxPin*/ 12, /*txPin*/ 13); 
}
void loop() {
  sbus.update();
  uint16_t ch3 = sbus.getChannel(3);
}
```
### Define TX/RX pin's and signal inversion (ESP32)
```cpp
#include <SBUS.h>
SBUS sbus(Serial2);

void setup() {
  //By default this enables signal inversion for ESP32
  sbus.begin(/*rxPin*/ 16, /*txPin*/ 17, /*inversion*/ false); 
}
void loop() {
  sbus.update();
  uint16_t ch3 = sbus.getChannel(3);
}
```

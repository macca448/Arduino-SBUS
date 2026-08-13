#include <SBUS.h>

// Create an SBUS object bound to the Arduino's Serial1 port
SBUS sbus(Serial1);

unsigned long prevPrint = 0;

void setup() {
Serial.begin(115200);

// Initialize the SBUS library using defaults (100000, SERIAL_8E2) and the boards defined pins for Serial1
sbus.begin();

Serial.println("==========================================");
Serial.println(" SBUS Named Channels Full Example ");
Serial.println("==========================================");
}

void loop() {
	// Most important: must call update() every loop iteration To continuously intercept and collect data from the receiver.
	sbus.update();

	// Get all 10 channel signals as independent variables
	uint16_t ch1_aileron = sbus.getChannel(1); // Right stick (left-right)
	uint16_t ch2_elevator = sbus.getChannel(2); // Right stick (up-down)
	uint16_t ch3_throttle = sbus.getChannel(3); // Left stick (up-down) -> Boat throttle
	uint16_t ch4_rudder = sbus.getChannel(4); // Left stick (left-right) -> Boat rudder

	uint16_t ch5_swA = sbus.getChannel(5); // SwA switch
	uint16_t ch6_swB = sbus.getChannel(6); // SwB switch
	uint16_t ch7_swC = sbus.getChannel(7); // SwC switch
	uint16_t ch8_swD = sbus.getChannel(8); // SwD switch

	uint16_t ch9_aux1 = sbus.getChannel(9); // Aux 1 channel (e.g., VrA/VrB knob)
	uint16_t ch10_aux2 = sbus.getChannel(10); // Aux 2 channel

	// -----------------------------------------------------------------
	// [Code area for controlling the boat]

	// You can write control conditions for variables ch1 - ch10 here, for example:
	// if (ch5_swA > 1800) { execute when SwA switch is flipped down }
	// -----------------------------------------------------------------

	unsigned long now = millis();
	// Print the signal value to the Serial Monitor every 200ms
	if (now - prevPrint >= 200) {
		prevPrint = now;

		// Check for signal loss before printing
		
		if (sbus.isFailsafe()) {
			Serial.println("\n[RC STATUS] !!! FAILSAFE ACTIVE !!! (Signal Lost)");
			Serial.println("-----------------------------------------");
		} else {
			Serial.println("\n=== [RC NAMED CHANNELS MONITOR] ===");

			if (sbus.isFrameLost()) {
				Serial.println("[WARN] Frame Lost Detected!");
			}

			char buf[100];

			// 1. Display the main control stick group
			Serial.println("[Sticks]");
			sprintf(buf, " Aileron (CH1): %04dus | Elevator (CH2): %04dus", ch1_aileron, ch2_elevator); Serial.println(buf);
			sprintf(buf, " Throttle (CH3): %04dus | Rudder (CH4): %04dus", ch3_throttle, ch4_rudder); Serial.println(buf);

			// 2. Display a group of 2-stage/3-stage switches
			Serial.println("[Switches]");
			sprintf(buf, " SwA (CH5): %04dus | SwB (CH6): %04dus", ch5_swA, ch6_swB); Serial.println(buf);
			sprintf(buf, " SwC (CH7): %04dus | SwD (CH8): %04dus", ch7_swC, ch8_swD); Serial.println(buf);

			// 3. Display the auxiliary Aux channel group (knob or rotary)
			Serial.println("[Auxiliary]");
			sprintf(buf, " Aux1 (CH9): %04dus | Aux2 (CH10): %04dus", ch9_aux1, ch10_aux2); Serial.println(buf);

			Serial.println("-----------------------------------------");
		}
	}
}

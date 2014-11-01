#include "easyctrl.h"

// Create a monitored int called "interval", with an initial value of 500.
Monitored<int> interval("interval", 500);

// Create a read-only monitored int called "count"
Monitored<int> count("count", 0, false);

void setup() {
    // Initialize the serial port
	Serial.begin(115200);

    // Initialize easyctrl, communicating over the serial port.
	Easyctrl.begin("Blink", Serial);

    // Set the LED pin as an output
    pinMode(13, OUTPUT);
}

void loop() {
    while(1) {
        // Let easyctrl process commands and output changes
        // Note this isn't ideal, since we only do this once every 2*interval ms.
        Easyctrl.update();

        // Blink the LED
        digitalWrite(13, HIGH);
        delay(interval);
        digitalWrite(13, LOW);
        delay(interval);

        // Update the count
        count = count + 1;
    }
}

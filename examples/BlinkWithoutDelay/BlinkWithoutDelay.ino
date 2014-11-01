/* Blink without delay, with easyctrl

Based on the BlinkWithoutDelay Arduino example sketch.
*/

#include "easyctrl.h"

const int ledPin = 13;

// Create a monitored int called "interval", with an initial value of 500.
Monitored<int> interval("interval", 500);

// Create a read-only monitored int called "count"
Monitored<int> count("count", 0, false);

int ledState = LOW;
long previousMillis = 0;

void setup() {
    // Initialize the serial port
	Serial.begin(115200);

    // Initialize easyctrl, communicating over the serial port.
	Easyctrl.begin("blink", Serial);

    // Set the LED pin as an output
    pinMode(13, OUTPUT);
}

void loop() {
    // Let easyctrl process commands and output changes
    Easyctrl.update();

    unsigned long currentMillis = millis();

    if(currentMillis - previousMillis > interval) {
        previousMillis = currentMillis;

        if(ledState == LOW)
            ledState = HIGH;
        else
            ledState = LOW;

        digitalWrite(ledPin, ledState);
        
        // Update the count
        count = count + 1;
    }
}

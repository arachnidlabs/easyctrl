#include "easyctrl.h"

// Create a monitored buffer called "greeting"
MonitoredBuffer<40> greeting("greeting");

void on_name_change(MonitoredBuffer<20> *name) {
    strcpy(greeting, "Hello, ");
    strcat(greeting, *name);
    strcat(greeting, "!");
    greeting.changed();
}

// Create a monitored buffer called "name", which calls on_name_change if it's
// set remotely
MonitoredBuffer<20> name("name", on_name_change);

void setup() {
    // Initialize the serial port
	Serial.begin(115200);

    // Initialize easyctrl, communicating over the serial port.
	Easyctrl.begin("CallResponse", Serial);
}

void loop() {
    while(1) {
        // Let easyctrl process commands and output changes
        Easyctrl.update();
    }
}

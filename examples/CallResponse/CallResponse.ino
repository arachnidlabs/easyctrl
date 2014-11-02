#include "easyctrl.h"

/* CallResponse

This sketch demonstrates a couple of Easyctrl concepts: monitored buffers, and
change events.

A monitored buffer is a fixed length string whose contents are communicated to
the controlling device. Unlike primitives, not all changes to a monitored buffer
can be detected automatically, so you need to call `.changed()` on it to let it
know when you've modified it.

Change events allow you to take actions when the controlling device modifies
a monitored variable. In this example, we define an event handler, `on_name_change`,
which updates the greeting in response to a change to the name.

*/

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

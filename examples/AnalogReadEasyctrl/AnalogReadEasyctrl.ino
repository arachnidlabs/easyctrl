/*
  AnalogReadEasyctrl
  Reads an analog input on pin 0.
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.
 
 This example code is based on the Arduino example AnalogReadSerial
 */

#include "easyctrl.h"

Monitored<int> sensorValue("sensorValue");

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  Easyctrl.begin("AnalogReadEasyctrl", Serial);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  sensorValue = analogRead(A0);

  // Let easyctrl run
  Easyctrl.update();

  delay(1);        // delay in between reads for stability
}

/*
  digitalreadserial

  reads a digital input on pin p1.1, prints the result to the serial monitor

  this example code is in the public domain.

  http://www.arduino.cc/en/tutorial/digitalreadserial
*/

#include <serial.h>

// digital pin p1.1 has a pushbutton attached to it. give it a name:
int pushbutton = 11;

// the setup routine runs once when you press reset:
void setup() {
  // no need to init usbserial
  
  // make the pushbutton's pin an input:
  pinmode(pushbutton, input_pullup);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input pin:
  int buttonstate = digitalread(pushbutton);
  // print out the state of the button:
  usbserial_println(buttonstate);
  delay(1);        // delay in between reads for stability
}

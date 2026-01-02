/*
  analog input

  demonstrates analog input by reading an analog sensor on analog pin p1.1 and
  turning on and off a light emitting diode(led) connected to digital pin p3.3.
  the amount of time the led will be on and off depends on the value obtained
  by analogread().

  the circuit:
  - potentiometer
    center pin of the potentiometer to the analog input p1.1
    one side pin (either one) to ground
    the other side pin to +5v
  - led
    anode (long leg) attached to digital output p3.3
    cathode (short leg) attached to ground

  - note: because the simplech552 breakout board has a built-in led attached to
    pin p3.3 on the board, the led is optional.

  created by david cuartielles
  modified 30 aug 2011
  by tom igoe
  modified 13 jun 2020
  by deqing sun for use with ch55xduino
  modified 28 mar 2024
  by cesar bautista for use unit_sketchlab
  this example code is in the public domain.

  http://www.arduino.cc/en/tutorial/analoginput
*/
#define led_builtin 34

int sensorpin = 11;    // select the input pin for the potentiometer. you may use p1.1, p1.4, p1.5 and p3.2
int ledpin = led_builtin; // select the pin for the led
int sensorvalue = 0;  // variable to store the value coming from the sensor

void setup() {
  // declare the ledpin as an output:
  pinMode(ledpin, OUTPUT);
  //by default 8051 enable every pin's pull up resistor. disable pull-up to get full input range.
  pinMode(sensorpin, INPUT);
}

void loop() {
  // read the value from the sensor:
  sensorvalue = analogRead(sensorpin);
  // turn the ledpin on
  digitalWrite(ledpin, HIGH);
  // stop the program for <sensorvalue> milliseconds:
  delay(sensorvalue);
  // turn the ledpin off:
  digitalWrite(ledpin, LOW);
  // stop the program for for <sensorvalue> milliseconds:
  delay(sensorvalue);
}

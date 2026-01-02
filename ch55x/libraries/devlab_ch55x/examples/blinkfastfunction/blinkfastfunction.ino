/*
  blink

  turns an led on for one second, then off for one second, repeatedly.

  most arduinos have an on-board led you can control. on the simplech552
  it is attached to digital pin p3.3

  this example uses marco-based function to accelerate io access speed. 
  note these functions will not disable analog output(pwm)
  and they only accept constants.

  if you want to know what pin the on-board led is connected to on your arduino
  model, check the technical specs of your board at:
  https://www.arduino.cc/en/main/products

  modified 8 may 2014
  by scott fitzgerald
  modified 2 sep 2016
  by arturo guadalupi
  modified 8 sep 2016
  by colby newman
  modified 13 jun 2020
  by deqing sun for use with ch55xduino
  modified 28 mar 2024
  by cesar bautista for use unit_sketchlab
  this example code is in the public domain.

  http://www.arduino.cc/en/tutorial/blink
*/

//p33
#define led_builtin_port 3
#define led_builtin_pin 3



// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin led_builtin as an output.
  pinmodefast(led_builtin_port,led_builtin_pin, output);
}

// the loop function runs over and over again forever
void loop() {
  digitalwritefast(led_builtin_port,led_builtin_pin, high);   // turn the led on (high is the voltage level)
  delay(1000);                       // wait for a second
  digitalwritefast(led_builtin_port,led_builtin_pin, low);    // turn the led off by making the voltage low
  delay(1000);                       // wait for a second
}

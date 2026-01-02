/*
  Blink with Serial Output

  Turns an LED on for one second, then off for one second, repeatedly.
  Also prints the LED state to USB Serial.

  Most Arduinos have an on-board LED you can control. On the CH552 board
  it is attached to digital pin P3.3 (LED_BUILTIN = 34)

  This example code is in the public domain.

  Modified 13 Jun 2020 by DeQing Sun for use with CH55xduino
  Modified 02 Jan 2026 by Unit Electronics for Devlab CH55x

  http://www.arduino.cc/en/tutorial/blink
*/

void setup() {
  // Initialize digital pin LED_BUILTIN as an output
  pinMode(LED_BUILTIN, OUTPUT);
  
  // USB Serial is always available on CH552, no need to initialize
  USBSerial_println_s("Blink Example Started");
  USBSerial_println_s("LED will blink every second");
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // Turn the LED on
  USBSerial_println_s("LED: ON");
  delay(1000);                       // Wait for a second
  
  digitalWrite(LED_BUILTIN, LOW);    // Turn the LED off
  USBSerial_println_s("LED: OFF");
  delay(1000);                       // Wait for a second
}

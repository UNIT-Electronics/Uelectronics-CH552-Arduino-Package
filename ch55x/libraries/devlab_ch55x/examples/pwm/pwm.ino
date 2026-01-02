/*
  PWM - Pulse Width Modulation Example

  Demonstrates the use of hardware PWM to fade an LED.
  Gradually increases and decreases the LED brightness using PWM.
  Also prints the current PWM value to USB Serial.

  The CH552 has hardware PWM1 on P1.5 and PWM2 on P1.4
  For testing, this example uses pin 11 (P1.1) with analogWrite()
  Note: analogWrite() on non-PWM pins may not produce smooth fading

  Circuit:
  - LED with 220Ω resistor connected to P1.1 (pin 11)

  This example code is in the public domain.

  Modified 13 Jun 2020 by DeQing Sun for use with CH55xduino
  Modified 02 Jan 2026 by Unit Electronics for Devlab CH55x
*/

#include <devlab_ch55x.h>

// Test pin (default: 11 = P1.1)
#define LED_PIN 11

int brightness = 0;      // LED brightness (0-255)
int fadeAmount = 5;      // Amount to change brightness each step

void setup() {
  // Configure pin as output
  pinMode(LED_PIN, OUTPUT);
  
  USBSerial_println_s("PWM Fade Example Started");
  USBSerial_println_s("LED on pin 11 (P1.1)");
  USBSerial_println_s("PWM Range: 0-255");
  USBSerial_println_s("------------------------------------");
}

void loop() {
  // Set the LED brightness
  analogWrite(LED_PIN, brightness);
  
  // Print current PWM value
  USBSerial_print_s("PWM Value: ");
  USBSerial_print_u(brightness);
  
  // Print brightness percentage
  uint16_t percent = (brightness * 100) / 255;
  USBSerial_print_s(" (");
  USBSerial_print_u(percent);
  USBSerial_println_s("%)");
  
  // Change brightness for next iteration
  brightness = brightness + fadeAmount;
  
  // Reverse direction at the ends of the fade
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
    
    if (brightness <= 0) {
      USBSerial_println_s(">>> Fading UP <<<");
    } else {
      USBSerial_println_s(">>> Fading DOWN <<<");
    }
  }
  
  delay(30);  // Wait 30ms to see the fade effect
}

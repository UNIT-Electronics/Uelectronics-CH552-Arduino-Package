/*
  PWM Fade - LED Brightness Control

  Demonstrates the use of PWM to fade an LED.
  Gradually increases and decreases the LED brightness using analogWrite().
  Also prints the current PWM value to USB Serial.

  Circuit:
  - LED with 220Ω resistor connected to P3.4 (pin 34)
  - Connect LED anode (+) to P3.4 through resistor
  - Connect LED cathode (-) to GND

  Note:
  - analogWrite() values range from 0-255
  - 0 = LED off, 255 = LED full brightness
  - Works on any digital pin

  This example code is in the public domain.

  Modified 13 Jun 2020 by DeQing Sun for use with CH55xduino
  Modified 02 Jan 2026 by Unit Electronics for Devlab CH55x
*/

#include <devlab_ch55x.h>

// LED pin: P3.4 (pin 34)
#define LED_PIN 34

// PWM range: 50-255 (avoid very low values where LED turns off)
int brightness = 50;     // LED brightness (50-255)
int fadeAmount = 5;      // Amount to change brightness each step

void setup() {
  // Configure pin as output
  pinMode(LED_PIN, OUTPUT);
  
  USBSerial_println_s("PWM Fade Example");
  USBSerial_println_s("LED on pin 34 (P3.4)");
  USBSerial_println_s("PWM Range: 50-255");
  USBSerial_println_s("------------------------------------");
}

void loop() {
  // Set the LED brightness
  analogWrite(LED_PIN, brightness);
  
  // Print current PWM value
  USBSerial_print_s("PWM Value: ");
  USBSerial_print_u(brightness);
  
  // Print brightness percentage
  uint16_t percent = ((uint32_t)brightness * 100) / 255;
  USBSerial_print_s(" (");
  USBSerial_print_u(percent);
  USBSerial_println_s("%)");
  
  // Change brightness for next iteration
  brightness = brightness + fadeAmount;
  
  // Reverse direction at the ends of the fade
  if (brightness <= 50 || brightness >= 255) {
    fadeAmount = -fadeAmount;
    
    if (brightness <= 50) {
      USBSerial_println_s(">>> Fading UP <<<");
    } else {
      USBSerial_println_s(">>> Fading DOWN <<<");
    }
  }
  
  delay(30);  // Wait 30ms to see the fade effect
}

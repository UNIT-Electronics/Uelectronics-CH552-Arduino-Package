/*
  PWM Buzzer Example

  Demonstrates the use of PWM to generate different beep patterns on a buzzer.
  Uses analogWrite() with 50% duty cycle and different on/off timings
  to create various sound patterns.

  Circuit:
  - Passive buzzer connected to P3.4 (pin 34)
  - Connect buzzer (+) to P3.4 and (-) to GND

  Note:
  - analogWrite(pin, 128) generates ~50% duty cycle square wave
  - Different delay patterns create different beep sounds
  - For musical notes, a tone() function would be needed

  This example code is in the public domain.

  Modified 13 Jun 2020 by DeQing Sun for use with CH55xduino
  Modified 02 Jan 2026 by Unit Electronics for Devlab CH55x
*/

#include <devlab_ch55x.h>

// Buzzer pin: P3.4 (pin 34)
#define BUZZER_PIN 34

void beep(uint16_t duration) {
  // Turn on buzzer with 50% duty cycle
  analogWrite(BUZZER_PIN, 128);
  delay(duration);
  
  // Turn off buzzer
  analogWrite(BUZZER_PIN, 0);
}

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  
  USBSerial_println_s("PWM Buzzer Example");
  USBSerial_println_s("Buzzer on pin 34 (P3.4)");
  USBSerial_println_s("Playing beep patterns");
  USBSerial_println_s("------------------------------------");
  
  delay(1000);
}

void loop() {
  // Pattern 1: Short beeps
  USBSerial_println_s("Pattern 1: Short beeps");
  for (uint8_t i = 0; i < 3; i++) {
    beep(100);
    delay(100);
  }
  delay(1000);
  
  // Pattern 2: Long beep
  USBSerial_println_s("Pattern 2: Long beep");
  beep(500);
  delay(1000);
  
  // Pattern 3: Fast pulses
  USBSerial_println_s("Pattern 3: Fast pulses");
  for (uint8_t i = 0; i < 10; i++) {
    beep(50);
    delay(50);
  }
  delay(1000);
  
  // Pattern 4: Alarm pattern
  USBSerial_println_s("Pattern 4: Alarm");
  for (uint8_t i = 0; i < 2; i++) {
    beep(200);
    delay(200);
    beep(200);
    delay(500);
  }
  delay(2000);
  
  USBSerial_println_s("-------- Repeating --------");
}

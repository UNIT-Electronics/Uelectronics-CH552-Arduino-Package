/*
  Digital Input Example

  Reads a digital input from a button and prints the state to USB Serial.
  Also controls the built-in LED based on the button state.

  Circuit:
  - Pushbutton connected to P1.1
    One side to P1.1
    Other side to ground
  - Built-in LED on P3.3 will reflect button state

  Note: This example uses INPUT_PULLUP mode, so the button should connect to ground.
        When button is pressed, the pin reads LOW.
        When button is released, the pin reads HIGH (pulled up internally).

  This example code is in the public domain.

  Modified 13 Jun 2020 by DeQing Sun for use with CH55xduino
  Modified 02 Jan 2026 by Unit Electronics for Devlab CH55x

  http://www.arduino.cc/en/tutorial/button
*/

#define BUTTON_PIN 11         // P1.1 for button input
#define LED_BUILTIN 34        // P3.3 built-in LED

int buttonState = 0;          // Variable to store button state
int lastButtonState = 0;      // Previous button state
int pressCount = 0;           // Count button presses

void setup() {
  // Initialize button pin with internal pull-up resistor
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize LED pin as output
  pinMode(LED_BUILTIN, OUTPUT);
  
  USBSerial_println_s("Digital Input Example Started");
  USBSerial_println_s("Press the button connected to P1.1");
  USBSerial_println_s("Format: Button State | LED State | Press Count");
  USBSerial_println_s("------------------------------------");
}

void loop() {
  // Read the button state
  buttonState = digitalRead(BUTTON_PIN);
  
  // Check if button state changed (debouncing not implemented for simplicity)
  if (buttonState != lastButtonState) {
    delay(50);  // Simple debounce delay
    
    // Re-read after debounce
    buttonState = digitalRead(BUTTON_PIN);
    
    if (buttonState != lastButtonState) {
      // Print state change
      USBSerial_print_s("Button: ");
      
      if (buttonState == LOW) {
        // Button is pressed (connected to ground)
        USBSerial_print_s("PRESSED");
        digitalWrite(LED_BUILTIN, HIGH);  // Turn LED on
        pressCount++;
      } else {
        // Button is released
        USBSerial_print_s("RELEASED");
        digitalWrite(LED_BUILTIN, LOW);   // Turn LED off
      }
      
      // Print LED state
      USBSerial_print_s(" | LED: ");
      if (buttonState == LOW) {
        USBSerial_print_s("ON");
      } else {
        USBSerial_print_s("OFF");
      }
      
      // Print press count
      USBSerial_print_s(" | Presses: ");
      USBSerial_println_u(pressCount);
      
      // Save current state
      lastButtonState = buttonState;
    }
  }
  
  // Small delay for stability
  delay(10);
}

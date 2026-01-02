/*
  OLED Display Example - SSD1306

  Demonstrates using a 128x64 SSD1306 OLED display with I2C.
  Displays text messages on the screen.

  The CH552 I2C pins are:
  - SDA: P1.6
  - SCL: P1.7

  Circuit:
  - Connect OLED SDA to P1.6
  - Connect OLED SCL to P1.7
  - Connect OLED VCC to 3.3V or 5V (check your OLED specs)
  - Connect OLED GND to ground
  - Add 4.7kΩ pull-up resistors on SDA and SCL lines (if not built into OLED)

  This example code is in the public domain.

  Modified 02 Jan 2026 by Unit Electronics for Devlab CH55x
*/

// Uncomment to use custom I2C pins (default: SDA=P1.6/16, SCL=P1.7/15)
// #define PIN_SDA 16
// #define PIN_SCL 15

#include <devlab_ch55x.h>

uint8_t counter = 0;

void setup() {
  // Initialize I2C
  I2C_init();
  
  // Initialize OLED
  OLED_init();
  OLED_clear();
  
  // Print startup message to USB Serial
  USBSerial_println_s("OLED Display Example");
  USBSerial_println_s("SSD1306 128x64 OLED");
  USBSerial_println_s("I2C Address: 0x3C");
  USBSerial_println_s("");
  
  // Display welcome message on OLED
  OLED_print("UNIT Electronics");
  OLED_println("");
  OLED_println("CH552 Board");
  OLED_println("OLED Test");
  OLED_println("");
  OLED_println("Counter: 0");
  
  delay(2000);
}

void loop() {
  // Clear screen
  OLED_clear();
  
  // Display counter
  OLED_println("UNIT Electronics");
  OLED_println("");
  OLED_print("Counter: ");
  
  // Convert counter to string and display
  char buffer[10];
  uint8_t i = 0;
  uint8_t temp = counter;
  
  // Handle zero case
  if (temp == 0) {
    buffer[i++] = '0';
  } else {
    // Convert number to string (reverse order)
    uint8_t digits[3];
    uint8_t digitCount = 0;
    while (temp > 0) {
      digits[digitCount++] = temp % 10;
      temp /= 10;
    }
    // Reverse and add to buffer
    for (int8_t j = digitCount - 1; j >= 0; j--) {
      buffer[i++] = '0' + digits[j];
    }
  }
  buffer[i] = '\0';
  
  OLED_println(buffer);
  OLED_println("");
  OLED_println("P1.6=SDA P1.7=SCL");
  
  // Print to serial
  USBSerial_print_s("Counter: ");
  USBSerial_println_u(counter);
  
  // Increment counter
  counter++;
  if (counter > 255) {
    counter = 0;
  }
  
  delay(1000);
}

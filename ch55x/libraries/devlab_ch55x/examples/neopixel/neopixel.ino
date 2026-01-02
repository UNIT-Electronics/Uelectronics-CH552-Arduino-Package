/*
  NeoPixel (WS2812B) Example

  Controls a string of WS2812B RGB LEDs (NeoPixels).
  Cycles through different colors on a strip of NeoPixels.

  The CH552 can control NeoPixels on pins: 33 (P3.3), 34 (P3.4), 14 (P1.4)
  This example uses P3.3 (pin 33)

  Circuit:
  - NeoPixel DIN (Data In) to P3.3 through 330Ω resistor
  - NeoPixel VCC to 5V (external power recommended for many LEDs)
  - NeoPixel GND to ground
  
  Note: For more than a few LEDs, use an external 5V power supply.
        Add a 1000µF capacitor between NeoPixel VCC and GND.

  This example code is in the public domain.

  Modified 02 Jan 2026 by Unit Electronics for Devlab CH55x
*/

#include <devlab_ch55x.h>

#define NEOPIXEL_PIN 33  // P3.3 (can be 33, 34, or 14)
#define NUM_PIXELS 8     // Number of NeoPixels in the strip

// Color definitions (R, G, B)
const uint8_t colors[][3] = {
  {255, 0, 0},     // Red
  {0, 255, 0},     // Green
  {0, 0, 255},     // Blue
  {255, 255, 0},   // Yellow
  {255, 0, 255},   // Magenta
  {0, 255, 255},   // Cyan
  {255, 128, 0},   // Orange
  {128, 0, 255}    // Purple
};

const uint8_t numColors = 8;
uint8_t currentColor = 0;

void setup() {
  // Initialize NeoPixel pin
  NEO_init(NEOPIXEL_PIN);
  
  USBSerial_println_s("NeoPixel Example");
  USBSerial_println_s("================");
  USBSerial_print_s("Number of pixels: ");
  USBSerial_println_u(NUM_PIXELS);
  USBSerial_print_s("Pin: ");
  USBSerial_println_u(NEOPIXEL_PIN);
  
  delay(1000);
}

void loop() {
  // Display current color information
  USBSerial_print_s("Color ");
  USBSerial_print_u(currentColor + 1);
  USBSerial_print_s("/");
  USBSerial_print_u(numColors);
  USBSerial_print_s(" - RGB(");
  USBSerial_print_u(colors[currentColor][0]);
  USBSerial_print_s(", ");
  USBSerial_print_u(colors[currentColor][1]);
  USBSerial_print_s(", ");
  USBSerial_print_u(colors[currentColor][2]);
  USBSerial_println_s(")");
  
  // Write color to all pixels
  for (uint8_t i = 0; i < NUM_PIXELS; i++) {
    NEO_writeColor(colors[currentColor][0], 
                   colors[currentColor][1], 
                   colors[currentColor][2]);
  }
  
  // Latch data to display
  NEO_latch();
  
  // Move to next color
  currentColor++;
  if (currentColor >= numColors) {
    currentColor = 0;
  }
  
  delay(1000);  // Wait 1 second between colors
}

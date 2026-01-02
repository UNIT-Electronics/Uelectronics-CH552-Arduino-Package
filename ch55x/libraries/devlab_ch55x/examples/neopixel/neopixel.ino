/*
  NeoPixel (WS2812B) Example

  Controls a string of WS2812B RGB LEDs (NeoPixels).
  This example demonstrates the basic concept of NeoPixel control.

  The CH552 can control NeoPixels on most GPIO pins.
  This example uses P1.4

  Circuit:
  - NeoPixel DIN (Data In) to P1.4
  - NeoPixel VCC to 5V (external power recommended for many LEDs)
  - NeoPixel GND to ground
  
  Note: For more than a few LEDs, use an external 5V power supply.
        Add a 470Ω resistor between CH552 pin and NeoPixel data in.
        Add a 1000µF capacitor between NeoPixel VCC and GND.

  IMPORTANT: This example requires a NeoPixel library compatible with CH552.
  The standard Adafruit_NeoPixel library is not compatible with 8051 architecture.
  
  For CH552, you need to implement bit-bang timing or use a CH552-specific library.

  This example code is in the public domain.

  Modified 02 Jan 2026 by Unit Electronics for Devlab CH55x
*/

#define NEOPIXEL_PIN 14      // P1.4
#define NUM_PIXELS 8         // Number of NeoPixels in the strip

void setup() {
  // Initialize NeoPixel pin as output
  pinMode(NEOPIXEL_PIN, OUTPUT);
  digitalWrite(NEOPIXEL_PIN, LOW);
  
  USBSerial_println_s("NeoPixel Example");
  USBSerial_println_s("=================");
  USBSerial_print_s("Number of pixels: ");
  USBSerial_println_u(NUM_PIXELS);
  USBSerial_println_s("");
  USBSerial_println_s("Note: This example requires a NeoPixel");
  USBSerial_println_s("library compatible with CH552/8051.");
  USBSerial_println_s("");
  USBSerial_println_s("The standard Adafruit_NeoPixel library");
  USBSerial_println_s("is not compatible with this architecture.");
  USBSerial_println_s("");
  USBSerial_println_s("To control WS2812B LEDs on CH552:");
  USBSerial_println_s("1. Use a CH552-specific NeoPixel library");
  USBSerial_println_s("2. Or implement bit-bang protocol");
  USBSerial_println_s("3. Timing: 0.4us high + 0.85us low = 0");
  USBSerial_println_s("          0.8us high + 0.45us low = 1");
  USBSerial_println_s("");
  USBSerial_println_s("Pin configured: P1.4");
}

void loop() {
  // Blink LED to show program is running
  digitalWrite(LED_BUILTIN, HIGH);
  USBSerial_println_s("Built-in LED: ON");
  delay(1000);
  
  digitalWrite(LED_BUILTIN, LOW);
  USBSerial_println_s("Built-in LED: OFF");
  delay(1000);
  
  USBSerial_println_s("Waiting for NeoPixel library...");
  delay(3000);
}

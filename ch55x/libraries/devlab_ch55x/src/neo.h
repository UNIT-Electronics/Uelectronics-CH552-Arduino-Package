// ===================================================================================
// NeoPixel (Addressable LED) Functions for CH552 Arduino                     * v1.3 *
// ===================================================================================
//
// Basic control functions for 800kHz addressable LEDs (NeoPixel). A simplified 
// protocol is used which should work with most LEDs.
//
// Supported pins: 33 (P3.3), 34 (P3.4), 14 (P1.4)
//
// Usage:
//   NEO_init(pin);                        // Initialize NeoPixel on specified pin
//   NEO_writeColor(r, g, b);             // Write RGB color to single pixel
//   NEO_writeHue(hue, brightness);       // Write color using HSV (hue 0-191, brightness 0-2)
//   NEO_latch();                         // Latch data to display on LEDs
//
// Example:
//   NEO_init(33);                        // Use P3.3
//   NEO_writeColor(255, 0, 0);          // Red
//   NEO_latch();                         // Display
//
// System clock frequency must be at least 6 MHz.
//
// Further information:     https://github.com/wagiminator/ATtiny13-NeoController
// 2023 by Stefan Wagner:   https://github.com/wagiminator
// 2026 Modified by Unit Electronics for Arduino compatibility

#pragma once
#include <stdint.h>

// NeoPixel type: define NEO_GRB or NEO_RGB before including
#ifndef NEO_GRB
  #ifndef NEO_RGB
    #define NEO_GRB  // Default to GRB (WS2812B standard)
  #endif
#endif

// Initialize NeoPixel on specified pin (33, 34, or 14)
void NEO_init(uint8_t pin);

// Latch colors (wait for reset time)
#define NEO_latch() delayMicroseconds(281)

void NEO_sendByte(uint8_t data);                          // send a single byte to the pixels
void NEO_writeColor(uint8_t r, uint8_t g, uint8_t b);     // write color to a single pixel
void NEO_writeHue(uint8_t hue, uint8_t bright);           // hue (0..191), brightness (0..2)

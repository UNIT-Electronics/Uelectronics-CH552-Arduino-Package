// ===================================================================================
// NeoPixel (Addressable LED) Functions for CH552 Arduino                     * v1.2 *
// ===================================================================================
//
// Basic control functions for 800kHz addressable LEDs (NeoPixel). A simplified 
// protocol is used which should work with most LEDs.
//
// System clock frequency must be at least 6 MHz (CH552 typically runs at 24 MHz).
//
// Further information:     https://github.com/wagiminator/ATtiny13-NeoController
// 2023 by Stefan Wagner:   https://github.com/wagiminator
// 2026 Modified by Unit Electronics for Arduino compatibility

// ===================================================================================
// Libraries, Variables and Constants
// ===================================================================================
#include <Arduino.h>
#include "neo.h"

// Global variable to store which pin is being used
static uint8_t neo_pin_number = 33;

// Note: Assembly needs specific pin symbols
// We'll create separate functions for each common pin

// ===================================================================================
// Protocol Delays
// ===================================================================================
// There are three essential conditions:
// - T0H (HIGH-time for "0"-bit) must be max.  500ns
// - T1H (HIGH-time for "1"-bit) must be min.  625ns
// - TCT (total clock time) must be      min. 1150ns
// The bit transmission loop takes 11 clock cycles.
#if F_CPU == 24000000       // 24 MHz system clock
  #define T1H_DELAY \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop                     // 15 - 4 = 11 clock cycles for min 625ns
  #define TCT_DELAY \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop                     // 28 - 11 - 11 = 6 clock cycles for min 1150ns
#elif F_CPU == 16000000     // 16 MHz system clock
  #define T1H_DELAY \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop                     // 10 - 4 = 6 clock cycles for min 625ns
  #define TCT_DELAY \
    nop             \
    nop                     // 19 - 6 - 11 = 2 clock cycles for min 1150ns
#elif F_CPU == 12000000     // 12 MHz system clock
  #define T1H_DELAY \
    nop             \
    nop             \
    nop             \
    nop                     // 8 - 4 = 4 clock cycles for min 625ns
  #define TCT_DELAY         // 14 - 4 - 11 < 0 clock cycles for min 1150ns
#elif F_CPU == 6000000      // 13 MHz system clock
  #define T1H_DELAY         // 4 - 4 = 0 clock cycles for min 625ns
  #define TCT_DELAY         // 7 - 0 - 11 < 0 clock cycles for min 1150ns
#else
  #error Unsupported system clock frequency for NeoPixels!
#endif

// ===================================================================================
// Send a Data Byte - P3.3 version
// ===================================================================================
static void NEO_sendByte_P3_3(uint8_t data) { 
  data;
  __asm
    .even
    mov  r7, #8
    xch  a, dpl
    01$:
    rlc  a
    setb _P3_3
    mov  _P3_3, c
    T1H_DELAY
    clr  _P3_3
    TCT_DELAY
    djnz r7, 01$
  __endasm;
}

// ===================================================================================
// Send a Data Byte - P3.4 version
// ===================================================================================
static void NEO_sendByte_P3_4(uint8_t data) { 
  data;
  __asm
    .even
    mov  r7, #8
    xch  a, dpl
    01$:
    rlc  a
    setb _P3_4
    mov  _P3_4, c
    T1H_DELAY
    clr  _P3_4
    TCT_DELAY
    djnz r7, 01$
  __endasm;
}

// ===================================================================================
// Send a Data Byte - P1.4 version
// ===================================================================================
static void NEO_sendByte_P1_4(uint8_t data) { 
  data;
  __asm
    .even
    mov  r7, #8
    xch  a, dpl
    01$:
    rlc  a
    setb _P1_4
    mov  _P1_4, c
    T1H_DELAY
    clr  _P1_4
    TCT_DELAY
    djnz r7, 01$
  __endasm;
}

// ===================================================================================
// Send a Data Byte - Router function
// ===================================================================================
void NEO_sendByte(uint8_t data) {
  switch(neo_pin_number) {
    case 33: NEO_sendByte_P3_3(data); break;
    case 34: NEO_sendByte_P3_4(data); break;
    case 14: NEO_sendByte_P1_4(data); break;
    default: NEO_sendByte_P3_3(data); break; // Default to P3.3
  }
}

// ===================================================================================
// Initialize NeoPixel with specific pin
// ===================================================================================
void NEO_init(uint8_t pin) {
  neo_pin_number = pin;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

// ===================================================================================
// Write Color to a Single Pixel
// ===================================================================================
void NEO_writeColor(uint8_t r, uint8_t g, uint8_t b) {
  #if defined (NEO_GRB)
    NEO_sendByte(g); NEO_sendByte(r); NEO_sendByte(b);
  #elif defined (NEO_RGB)
    NEO_sendByte(r); NEO_sendByte(g); NEO_sendByte(b);
  #else
    #error Wrong or missing NeoPixel type definition!
  #endif
}

// ===================================================================================
// Write Hue Value (0..191) and Brightness (0..2) to a Single Pixel
// ===================================================================================
void NEO_writeHue(uint8_t hue, uint8_t bright) {
  uint8_t phase = hue >> 6;
  uint8_t step  = (hue & 63) << bright;
  uint8_t nstep = (63 << bright) - step;
  switch(phase) {
    case 0:   NEO_writeColor(nstep,  step,     0); break;
    case 1:   NEO_writeColor(    0, nstep,  step); break;
    case 2:   NEO_writeColor( step,     0, nstep); break;
    default:  break;
  }
}

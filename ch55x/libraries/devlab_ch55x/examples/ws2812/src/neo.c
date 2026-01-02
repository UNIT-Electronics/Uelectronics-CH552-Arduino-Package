// ===================================================================================
// neopixel (addressable led) functions for ch551, ch552 and ch554            * v1.1 *
// ===================================================================================
//
// basic control functions for 800khz addressable leds (neopixel). a simplified 
// protocol is used which should work with most leds.
//
// the following must be defined in config.h:
// pin_neo - pin connected to data-in of the pixel strip (via a ~330 ohms resistor).
// neo_grb - type of pixel: neo_grb or neo_rgb
// system clock frequency must be at least 6 mhz.
//
// further information:     https://github.com/wagiminator/attiny13-neocontroller
// 2023 by stefan wagner:   https://github.com/wagiminator

// ===================================================================================
// libraries, variables and constants
// ===================================================================================
#include "neo.h"

#define neopin pin_asm(pin_neo)     // convert pin_neo for inline assembly

// ===================================================================================
// protocol delays
// ===================================================================================
// there are three essential conditions:
// - t0h (high-time for "0"-bit) must be max.  500ns
// - t1h (high-time for "1"-bit) must be min.  625ns
// - tct (total clock time) must be      min. 1150ns
// the bit transmission loop takes 11 clock cycles.
#if f_cpu == 24000000       // 24 mhz system clock
  #define t1h_delay \
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
  #define tct_delay \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop                     // 28 - 11 - 11 = 6 clock cycles for min 1150ns
#elif f_cpu == 16000000     // 16 mhz system clock
  #define t1h_delay \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop                     // 10 - 4 = 6 clock cycles for min 625ns
  #define tct_delay \
    nop             \
    nop                     // 19 - 6 - 11 = 2 clock cycles for min 1150ns
#elif f_cpu == 12000000     // 12 mhz system clock
  #define t1h_delay \
    nop             \
    nop             \
    nop             \
    nop                     // 8 - 4 = 4 clock cycles for min 625ns
  #define tct_delay         // 14 - 4 - 11 < 0 clock cycles for min 1150ns
#elif f_cpu == 6000000      // 13 mhz system clock
  #define t1h_delay         // 4 - 4 = 0 clock cycles for min 625ns
  #define tct_delay         // 7 - 0 - 11 < 0 clock cycles for min 1150ns
#else
  #error unsupported system clock frequency for neopixels!
#endif

// ===================================================================================
// send a data byte to the pixels string
// ===================================================================================
// this is the most time sensitive part. outside of the function, it must be 
// ensured that interrupts are disabled and that the time between the 
// transmission of the individual bytes is less than the pixel's latch time.
void neo_sendbyte(uint8_t data) { 
  data;                 // stop unreferenced argument warning
  __asm
    .even
    mov  r7, #8         ; 2 clk - 8 bits to transfer
    xch  a, dpl         ; 2 clk - data byte -> accu
    01$:
    rlc  a              ; 1 clk - data bit -> carry (msb first)
    setb neopin         ; 2 clk - neo pin high
    mov  neopin, c      ; 2 clk - "0"-bit? -> neo pin low now
    t1h_delay           ; x clk - th1 delay
    clr  neopin         ; 2 clk - "1"-bit? -> neo pin low a little later
    tct_delay           ; y clk - tct delay
    djnz r7, 01$        ; 2/4|5|6 clk - repeat for all bits
  __endasm;
}

// ===================================================================================
// write color to a single pixel
// ===================================================================================
void neo_writecolor(uint8_t r, uint8_t g, uint8_t b) {
  #if defined (neo_grb)
    neo_sendbyte(g); neo_sendbyte(r); neo_sendbyte(b);
  #elif defined (neo_rgb)
    neo_sendbyte(r); neo_sendbyte(g); neo_sendbyte(b);
  #else
    #error wrong or missing neopixel type definition!
  #endif
}

// ===================================================================================
// write hue value (0..191) and brightness (0..2) to a single pixel
// ===================================================================================
void neo_writehue(uint8_t hue, uint8_t bright) {
  uint8_t phase = hue >> 6;
  uint8_t step  = (hue & 63) << bright;
  uint8_t nstep = (63 << bright) - step;
  switch(phase) {
    case 0:   neo_writecolor(nstep,  step,     0); break;
    case 1:   neo_writecolor(    0, nstep,  step); break;
    case 2:   neo_writecolor( step,     0, nstep); break;
    default:  break;
  }
}

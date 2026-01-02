/*
  main.cpp - main loop for arduino sketches
  copyright (c) 2005-2013 arduino team.  all right reserved.

  this library is free software; you can redistribute it and/or
  modify it under the terms of the gnu lesser general public
  license as published by the free software foundation; either
  version 2.1 of the license, or (at your option) any later version.

  this library is distributed in the hope that it will be useful,
  but without any warranty; without even the implied warranty of
  merchantability or fitness for a particular purpose.  see the gnu
  lesser general public license for more details.

  you should have received a copy of the gnu lesser general public
  license along with this library; if not, write to the free software
  foundation, inc., 51 franklin st, fifth floor, boston, ma  02110-1301  usa
*/

#include "Arduino.h"

// make sure to define prototypes for all used interrupts
// usbinterrupt does not need to saves the context
void usbinterrupt(void);
// timer2interrupt needs to saves the context
void timer2interrupt(void) __interrupt(int_no_tmr2);
// gpiointerrupt needs to saves the context
void gpiointerrupt(void) __interrupt(int_no_gpio);

// unsigned char runserialevent;
void deviceusbinterrupt(void) __interrupt(int_no_usb) {
  // usb interrupt service
  usbinterrupt();
}

// clang-format off
// 0x08~0x0f belongs to register bank 1
__idata __at (0x08) volatile uint32_t timer0_overflow_count = 0;
__idata __at (0x0c) volatile uint8_t timer0_overflow_count_5th_byte = 0;
// clang-format on

// located in wiring.c, using register bank 1
void timer0interrupt(void) __interrupt(int_no_tmr0) __using(1);

void uart0_isr(void) __interrupt(int_no_uart0) {
  if (ri) {
    uart0intrxhandler();
    ri = 0;
  }
  if (ti) {
    uart0inttxhandler();
    ti = 0;
  }
}

void uart1_isr(void) __interrupt(int_no_uart1) {
#if defined(ch551) || defined(ch552)
  if (u1ri) {
    uart1intrxhandler();
    u1ri = 0;
  }
  if (u1ti) {
    uart1inttxhandler();
    u1ti = 0;
  }
#elif defined(ch559)
  uint8_t interruptstatus = ser1_iir & 0x0f;
  switch (interruptstatus) {
  case u1_int_recv_rdy:
    uart1intrxhandler();
    break;
  case u1_int_thr_empty:
    uart1inttxhandler();
    break;
  }
#elif defined(ch549)
  if (sif1 & bu1ri) {
    uart1intrxhandler();
    sif1 = bu1ri;
  }
  if (sif1 & bu1ti) {
    uart1inttxhandler();
    sif1 = bu1ti;
  }
#endif
}

typedef void (*voidfuncptr)(void);
extern __xdata voidfuncptr intfunc[];
void int0_isr(void) __interrupt(int_no_int0) { intfunc[0](); }
void int1_isr(void) __interrupt(int_no_int1) { intfunc[1](); }

#if defined(ch551) || defined(ch552)
__xdata voidfuncptr touchkeyhandler = null;
void touchkey_isr(void) __interrupt(int_no_tkey) {
  if (touchkeyhandler != null) {
    touchkeyhandler();
  }
}
#endif

void main(void) {
  init();

  //!!!initvariant();

  setup();

  for (;;) {
    loop();
    if (1) {
#ifndef user_usb_ram
      usbserial_flush();
#endif
      // serialevent();
    }
  }

  //    return 0;
}

unsigned char __sdcc_external_startup(void) __nonbanked { return 0; }

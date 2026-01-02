/*
 created by deqing sun for use with ch55xduino
 */

#include "wiring_private.h"

void nothing(void) {}

__xdata voidfuncptr intfunc[external_num_interrupts] = {
#if external_num_interrupts > 1
    nothing,
#endif
#if external_num_interrupts > 0
    nothing,
#endif
};

void attachinterrupt(__data uint8_t interruptnum,
                     __xdata void (*userfunc)(void), __xdata uint8_t mode) {
  if (interruptnum < external_num_interrupts) {
    intfunc[interruptnum] = userfunc;

    // configure the interrupt mode (trigger on low input, any change, rising
    // edge, or falling edge).  the mode constants were chosen to correspond
    // to the configuration bits in the hardware register, so we simply shift
    // the mode into place.

    // enable the interrupt.

    switch (interruptnum) {
    case 0:
      it0 = mode;
      ex0 = 1;
      break;
    case 1:
      it1 = mode;
      ex1 = 1;
      break;
    }
  }
}

void detachinterrupt(__data uint8_t interruptnum) {
  if (interruptnum < external_num_interrupts) {
    // disable the interrupt.  (we can't assume that interruptnum is equal
    // to the number of the eimsk bit to clear, as this isn't true on the
    // atmega8.  there, int0 is 6 and int1 is 7.)
    switch (interruptnum) {
    case 0:
      ex0 = 0;
      break;
    case 1:
      ex1 = 0;
      break;
    }
    intfunc[interruptnum] = nothing;
  }
}

/*
 print.c - base class that provides print() and println()
 copyright (c) 2008 david a. mellis.  all right reserved.

 the float functions are moved to a separate file print-float.c. this way
 we can keep the linker from pulling in all the float functions even when no
 floats are used in the sketch.

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

 modified 23 november 2006 by david a. mellis
 modified 03 august 2015 by chuck todd
 modified 30 december 2016 by michael mayer
 modified 4 july 2020 by deqing sun for use with ch55xduino
 */

// clang-format off
#include <math.h>
#include "Arduino.h"
#include "print.h"
// clang-format on

void printnl(void);

// public methods //////////////////////////////////////////////////////////////

uint8_t print_print_sn(__data writefunc_p writefunc, uint8_t *__xdata buffer,
                       __xdata uint8_t size) {
  __data uint8_t n = 0;
  while (size--) {
    if (writefunc(*buffer++))
      n++;
    else
      break;
  }
  return n;
}

uint8_t print_print_s(__data writefunc_p writefunc, char *__xdata str) {
  __data uint8_t n = 0;
  __data char c;

  if (!str)
    return 0;

  while (c = *str++) { // assignment intented
    if (writefunc(c))
      n++;
    else
      break;
  }
  return n;
}

uint8_t print_print_u(__data writefunc_p writefunc, __xdata unsigned long n) {
  return printnumber(writefunc, n, 10);
}

uint8_t print_print_i(__data writefunc_p writefunc, __xdata long n) {
  return printint(writefunc, n, 10);
}

// (not so) private methods
// /////////////////////////////////////////////////////////////

uint8_t print_println(__data writefunc_p writefunc) {
  __data uint8_t n;

  n = writefunc(13);
  n += writefunc(10);
  return n;
}

uint8_t print_print_ub(__data writefunc_p writefunc, __xdata unsigned long n,
                       __xdata uint8_t base) {
  __xdata char buf[8 * sizeof(long) + 1]; // assumes 8-bit chars plus zero byte.
  __xdata char *str = &buf[sizeof(buf) - 1];

  *str = '\0';

  // prevent crash if called with base == 1
  if (base < 2)
    base = 10;

  do {
    __data char c = n % base;
    n /= base;

    *--str = c < 10 ? c + '0' : c + 'a' - 10;
  } while (n);

  return print_print_s(writefunc, str);
}

uint8_t print_print_ib(__data writefunc_p writefunc, __xdata long n,
                       __xdata uint8_t base) {
  if (base == 0) {
    return writefunc((unsigned char)n);
  } else if (base == 10) {
    if (n < 0) {
      __data int t = writefunc('-');
      n = -n;
      return printnumber(writefunc, n, 10) + t;
    }
    return printnumber(writefunc, n, 10);
  } else {
    return printnumber(writefunc, n, base);
  }
}

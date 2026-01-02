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
 */

// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
#include "Arduino.h"
#include <math.h>

#include "print.h"

// public methods //////////////////////////////////////////////////////////////

uint8_t print_print_fd(__data writefunc_p writefunc, __xdata double number,
                       __xdata uint8_t digits) {
  __xdata uint8_t n = 0;
  __xdata uint8_t i;
  __xdata unsigned long int_part;
  __xdata double remainder, rounding;
  __xdata unsigned int toprint;

  if (isnan(number))
    return printstr(writefunc, "nan");
  if (isinf(number))
    return printstr(writefunc, "inf");
  if (number > 4294967040.0)
    return printstr(writefunc, "ovf"); // constant determined empirically
  if (number < -4294967040.0)
    return printstr(writefunc, "ovf"); // constant determined empirically

  // handle negative numbers
  if (number < 0.0) {
    n += writefunc('-');
    number = -number;
  }

  // round correctly so that print(1.999, 2) prints as "2.00"
  rounding = 0.5;
  for (i = 0; i < digits; ++i)
    rounding /= 10.0;

  number += rounding;

  // extract the integer part of the number and print it
  int_part = (unsigned long)number;
  remainder = number - (double)int_part;
  n += printnumber(writefunc, int_part, 10);

  // print the decimal point, but only if there are digits beyond
  if (digits > 0) {
    n += writefunc('.');
  }

  // extract digits from the remainder one at a time
  while (digits-- > 0) {
    remainder *= 10.0;
    toprint = (unsigned int)(remainder);
    n += printnumber(writefunc, toprint, 10);
    remainder -= toprint;
  }

  return n;
}

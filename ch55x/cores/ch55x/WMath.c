/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  part of the wiring project - http://wiring.org.co
  copyright (c) 2004-06 hernando barragan
  modified 13 august 2006, david a. mellis for arduino - http://www.arduino.cc/

  this library is free software; you can redistribute it and/or
  modify it under the terms of the gnu lesser general public
  license as published by the free software foundation; either
  version 2.1 of the license, or (at your option) any later version.

  this library is distributed in the hope that it will be useful,
  but without any warranty; without even the implied warranty of
  merchantability or fitness for a particular purpose.  see the gnu
  lesser general public license for more details.

  you should have received a copy of the gnu lesser general
  public license along with this library; if not, write to the
  free software foundation, inc., 59 temple place, suite 330,
  boston, ma  02111-1307  usa
*/

#ifdef __cplusplus
extern "c" {
#include "stdlib.h"
}
#else
#include "stdlib.h"
#endif

void randomseed(__data unsigned long seed) {
  if (seed != 0) {
    srand(seed);
  }
}

long random(__data long howbig) {
  if (howbig == 0) {
    return 0;
  }
  return rand() % howbig;
}

long random_minmax(__data long howsmall, __xdata long howbig) {
  __data long diff;

  if (howsmall >= howbig) {
    return howsmall;
  }
  diff = howbig - howsmall;
  return random(diff) + howsmall;
}

long map(__data long x, __xdata long in_min, __xdata long in_max,
         __xdata long out_min, __xdata long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// unsigned int makeword(unsigned int w) { return w; }
// unsigned int makeword(unsigned char h, unsigned char l) { return (h << 8) |
// l; }

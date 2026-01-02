/*
 * copyright (c) 2023 by deqing sun <ds@thinkcreate.us> (c version for ch552
 * port) servo library for arduino.
 *
 * this file is free software; you can redistribute it and/or modify
 * it under the terms of either the gnu general public license version 2
 * or the gnu lesser general public license version 2.1, both as
 * published by the free software foundation.
 */

#include "servo.h"

void digitalwritehighdirectlut(uint8_t pin);
void digitalwritelowdirectlut(uint8_t pin);

#if f_cpu > 24000000
#error current clock is too fast for this version of library. please use 24m or lower.
#endif

// each 5536 is 60000tick, 2.5ms in 24mhz
volatile __xdata uint16_t listrcap2[16] = {5536, 5536, 5536, 5536,
                                           5536, 5536, 5536, 5536};
volatile __xdata uint8_t listrcap2ptr = 0;
volatile __xdata uint8_t listrcap2limit = (8 + 0);
//*8 *9 pin not exist
volatile __xdata uint8_t servopin[16] = {9, 9, 9, 9, 9, 9, 9, 9};
volatile __xdata uint8_t servopinnext = 9;
volatile __xdata uint8_t servopinprevious = 9;

__xdata uint16_t servo_min = 1000;
__xdata uint16_t servo_max = 2000;

void timer2interrupt(void) __interrupt {
  if (tf2) {
    tf2 = 0;

    __idata uint8_t listrcap2ptrcache = listrcap2ptr;
    __idata uint16_t listrcap2cache = listrcap2[listrcap2ptrcache];

    rcap2l = listrcap2cache & 0xff;
    rcap2h = listrcap2cache >> 8;

    __idata uint8_t nextpincache = servopinnext;
    __idata uint8_t previouspincache = servopinprevious;

    digitalwritelowdirectlut(previouspincache);
    digitalwritehighdirectlut(nextpincache);
    servopinprevious = nextpincache;
    servopinnext = servopin[listrcap2ptrcache];

    listrcap2ptrcache++;
    if (listrcap2ptrcache >= listrcap2limit) {
      listrcap2ptrcache = 0;
    }

    listrcap2ptr = listrcap2ptrcache;
  }
}

void servo_wait_till_no_action() {
  while (1) {
    et2 = 0;
    __idata uint8_t listrcap2ptrcache = listrcap2ptr;
    et2 = 1;
    if (listrcap2ptrcache < 7) {
      break;
    }
  }
}

void servo_init() {
  listrcap2limit = 8;
  __idata uint16_t valuercap2_2_5ms = (65536 - f_cpu * 0.0025);
  for (__idata uint8_t i = 0; i < 8; i++) {
    listrcap2[i] = valuercap2_2_5ms;
  }
  for (__idata uint8_t i = 0; i < 8; i++) {
    servopin[i] = 9;
  }

  t2con = 0x00;
  // btmr_clk may be set by uart0, we keep it as is.
  t2mod |= btmr_clk | bt2_clk; // use fsys for t2

  tl2 = 0;
  th2 = 0;
  rcap2l = 0;
  rcap2h = 0;

  et2 = 1;

  tr2 = 1;
}

uint8_t servo_search_pin(uint8_t pin) {
  __idata uint8_t listrcap2limitcache = listrcap2limit;
  for (__idata uint8_t i = 8; i < listrcap2limitcache; i++) {
    if (servopin[i] == pin) {
      return i;
    }
  }
  return 0;
}

bool servo_attach(uint8_t pin) {
  if (pin > 37) {
    return false;
  }
  __idata uint8_t pinmod10 = pin % 10;
  if (pinmod10 == 9) {
    return false;
  }
  if (pinmod10 == 8) {
    return false;
  }
  if (listrcap2limit >= 16) {
    return false;
  }
  if (servo_search_pin(pin) != 0) {
    return false;
  }
  servo_wait_till_no_action();
  __idata uint16_t valuercap2_1_5ms = (65536 - f_cpu * 0.0015);
  __idata uint8_t listrcap2limitcache = listrcap2limit;
  servopin[listrcap2limitcache] = pin;
  listrcap2[listrcap2limitcache] = valuercap2_1_5ms;
  listrcap2limitcache++;
  listrcap2limit = listrcap2limitcache;
  return true;
}

bool servo_detach(uint8_t pin) {
  __idata uint8_t pinindex = servo_search_pin(pin);
  if (pinindex == 0) {
    return false;
  }
  servo_wait_till_no_action();
  et2 = 0;
  __idata uint8_t listrcap2limitcache = listrcap2limit;
  for (__idata uint8_t i = pinindex; i < listrcap2limitcache - 1; i++) {
    servopin[i] = servopin[i + 1];
    listrcap2[i] = listrcap2[i + 1];
  }
  listrcap2limitcache--;
  listrcap2limit = listrcap2limitcache;
  et2 = 1;
  return true;
}

bool servo_writemicroseconds(uint8_t pin, __xdata uint16_t pulseus) {
  __idata uint8_t pinindex = servo_search_pin(pin);
  if (pinindex == 0) {
    return false;
  }
  __idata uint16_t value = (65536 - ((f_cpu / 1000000) * pulseus));
  et2 = 0;
  listrcap2[pinindex] = value;
  et2 = 1;
  return true;
}

bool servo_write(uint8_t pin, __xdata int16_t value) {
  __idata uint8_t pinindex = servo_search_pin(pin);
  __idata uint16_t pulsevalue;
  if (pinindex == 0) {
    return false;
  }
  if (value <= 200) {
    // value is angle when value <= 200
    if (value < 0) {
      value = 0;
    }
    if (value > 180) {
      value = 180;
    }
    uint16_t pulseus =
        servo_min + ((servo_max - servo_min) * ((uint32_t)value)) / 180;
    pulsevalue = (65536 - ((f_cpu / 1000000) * (pulseus)));
  } else {
    // value is pulseus when value > 200
    pulsevalue = (65536 - ((f_cpu / 1000000) * value));
  }
  et2 = 0;
  listrcap2[pinindex] = pulsevalue;
  et2 = 1;
  return true;
}

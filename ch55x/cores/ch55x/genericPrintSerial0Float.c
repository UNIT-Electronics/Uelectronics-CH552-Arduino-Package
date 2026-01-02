/*
 created by deqing sun for use with ch55xduino
 need sdcc 13402 or higher version
 float has a separate file to avoid unnecessary linking
 */

#include "Arduino.h"

void serial0_print_f_func(__data float f) { print_print_f(serial0_write, f); }

void serial0_print_fd_func(__data float f, __xdata uint8_t digits) {
  print_print_fd(serial0_write, f, digits);
}

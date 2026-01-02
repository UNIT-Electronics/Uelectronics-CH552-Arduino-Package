/*
 created by deqing sun for use with ch55xduino
 need sdcc 13402 or higher version
 float has a separate file to avoid unnecessary linking
 */

#include "Arduino.h"

void serial1_print_f_func(__data float f) { print_print_f(serial1_write, f); }

void serial1_print_fd_func(__data float f, __xdata uint8_t digits) {
  print_print_fd(serial1_write, f, digits);
}

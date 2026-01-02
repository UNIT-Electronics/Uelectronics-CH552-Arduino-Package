/*
 created by deqing sun for use with ch55xduino
 need sdcc 13402 or higher version
 float has a separate file to avoid unnecessary linking
 */

#include "arduino.h"

void usbserial_print_f_func(__data float f) {
  print_print_f(usbserial_write, f);
}

void usbserial_print_fd_func(__data float f, __xdata uint8_t digits) {
  print_print_fd(usbserial_write, f, digits);
}

/*
 created by deqing sun for use with ch55xduino
 need sdcc 13402 or higher version
 */

#include "arduino.h"

void printnothing() { // only appear in this file
}

void usbserial_print_i_func(__data long i) {
  print_print_i(usbserial_write, i);
}
void usbserial_print_ib_func(__data long i, __xdata uint8_t base) {
  print_print_ib(usbserial_write, i, base);
}
void usbserial_print_u_func(__data unsigned long u) {
  print_print_u(usbserial_write, u);
}
void usbserial_print_ub_func(__data unsigned long u, __xdata uint8_t base) {
  print_print_ib(usbserial_write, u, base);
}
void usbserial_print_s_func(char *__data s) {
  print_print_s(usbserial_write, s);
}
void usbserial_print_sn_func(char *__data s, __xdata uint8_t size) {
  print_print_sn(usbserial_write, s, size);
}

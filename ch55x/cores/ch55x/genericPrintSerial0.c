/*
 created by deqing sun for use with ch55xduino
 need sdcc 13402 or higher version
 */

#include "Arduino.h"

void serial0_print_i_func(__data long i) { print_print_i(serial0_write, i); }
void serial0_print_ib_func(__data long i, __xdata uint8_t base) {
  print_print_ib(serial0_write, i, base);
}
void serial0_print_u_func(__data unsigned long u) {
  print_print_u(serial0_write, u);
}
void serial0_print_ub_func(__data unsigned long u, __xdata uint8_t base) {
  print_print_ib(serial0_write, u, base);
}
void serial0_print_s_func(char *__data s) { print_print_s(serial0_write, s); }
void serial0_print_sn_func(char *__data s, __xdata uint8_t size) {
  print_print_sn(serial0_write, s, size);
}

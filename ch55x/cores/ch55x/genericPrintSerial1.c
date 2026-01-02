/*
 created by deqing sun for use with ch55xduino
 need sdcc 13402 or higher version
 */

#include "Arduino.h"

void serial1_print_i_func(__data long i) { print_print_i(serial1_write, i); }
void serial1_print_ib_func(__data long i, __xdata uint8_t base) {
  print_print_ib(serial1_write, i, base);
}
void serial1_print_u_func(__data unsigned long u) {
  print_print_u(serial1_write, u);
}
void serial1_print_ub_func(__data unsigned long u, __xdata uint8_t base) {
  print_print_ib(serial1_write, u, base);
}
void serial1_print_s_func(char *__data s) { print_print_s(serial1_write, s); }
void serial1_print_sn_func(char *__data s, __xdata uint8_t size) {
  print_print_sn(serial1_write, s, size);
}

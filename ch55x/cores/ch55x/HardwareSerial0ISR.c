/*
 created by deqing sun for use with ch55xduino
 */

#include "HardwareSerial.h"

__xdata uint8_t
    receive_uart0_buf[serial0_rx_buffer_size]; // arduino style serial buffer
__xdata uint8_t
    transmit_uart0_buf[serial0_tx_buffer_size]; // arduino style serial buffer
volatile __xdata uint8_t uart0_rx_buffer_head = 0;
volatile __xdata uint8_t uart0_rx_buffer_tail = 0;
volatile __xdata uint8_t uart0_tx_buffer_head = 0;
volatile __xdata uint8_t uart0_tx_buffer_tail = 0;
volatile __bit uart0_flag_sending = 0;

void uart0intrxhandler() {
  __data uint8_t nexthead = (uart0_rx_buffer_head + 1) % serial0_rx_buffer_size;

  if (nexthead != uart0_rx_buffer_tail) {
    receive_uart0_buf[uart0_rx_buffer_head] = sbuf;
    uart0_rx_buffer_head = nexthead;
  }
}

void uart0inttxhandler() {
  if (uart0_flag_sending) {
    if (uart0_tx_buffer_head == uart0_tx_buffer_tail) {
      // do no more
      uart0_flag_sending &= 0;
    } else {
      sbuf = transmit_uart0_buf[uart0_tx_buffer_tail];
      uart0_tx_buffer_tail =
          (uart0_tx_buffer_tail + 1) % serial0_tx_buffer_size;
    }
  }
}

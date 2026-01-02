#include "HardwareSerial.h"

__xdata uint8_t
    receive_uart1_buf[serial1_rx_buffer_size]; // arduino style serial buffer
__xdata uint8_t
    transmit_uart1_buf[serial1_tx_buffer_size]; // arduino style serial buffer
volatile __xdata uint8_t uart1_rx_buffer_head = 0;
volatile __xdata uint8_t uart1_rx_buffer_tail = 0;
volatile __xdata uint8_t uart1_tx_buffer_head = 0;
volatile __xdata uint8_t uart1_tx_buffer_tail = 0;
volatile __bit uart1_flag_sending = 0;

void uart1intrxhandler() {
  __data uint8_t nexthead = (uart1_rx_buffer_head + 1) % serial1_rx_buffer_size;

  if (nexthead != uart1_rx_buffer_tail) {
#if defined(ch551) || defined(ch552)
    receive_uart1_buf[uart1_rx_buffer_head] = sbuf1;
#elif defined(ch559)
    receive_uart1_buf[uart1_rx_buffer_head] = ser1_rbr;
#elif defined(ch549)
    receive_uart1_buf[uart1_rx_buffer_head] = sbuf1;
#endif
    uart1_rx_buffer_head = nexthead;
  }
}

void uart1inttxhandler() {
  if (uart1_flag_sending) {
    if (uart1_tx_buffer_head == uart1_tx_buffer_tail) {
      // do no more
      uart1_flag_sending &= 0;
    } else {
#if defined(ch551) || defined(ch552)
      sbuf1 = transmit_uart1_buf[uart1_tx_buffer_tail];
#elif defined(ch559)
      ser1_thr = transmit_uart1_buf[uart1_tx_buffer_tail];
#elif defined(ch549)
      sbuf1 = transmit_uart1_buf[uart1_tx_buffer_tail];
#endif
      uart1_tx_buffer_tail =
          (uart1_tx_buffer_tail + 1) % serial1_tx_buffer_size;
    }
  }
}

/*
 created by deqing sun for use with ch55xduino
 */

#include "hardwareserial.h"

__xdata unsigned char serial0initialized;

extern __xdata uint8_t receive_uart0_buf[];  // arduino style serial buffer
extern __xdata uint8_t transmit_uart0_buf[]; // arduino style serial buffer
extern volatile __xdata uint8_t uart0_rx_buffer_head;
extern volatile __xdata uint8_t uart0_rx_buffer_tail;
extern volatile __xdata uint8_t uart0_tx_buffer_head;
extern volatile __xdata uint8_t uart0_tx_buffer_tail;
extern volatile __bit uart0_flag_sending;

// extern wait functions
void delaymicroseconds(__data uint16_t us);

uint8_t serial0(void) { return serial0initialized; }

void serial0_begin(__data unsigned long baud) {

  __data uint32_t x;
  __data uint8_t x2;

  x = 10 * f_cpu / baud / 16; // make sure it doesn't overflow when baudrate is
                              // changed, default 9600
  x2 = ((uint16_t)x) % 10;
  x /= 10;
  if (x2 >= 5)
    x++; // round

  sm0 = 0;
  sm1 = 1;
  sm2 = 0; // use mode 0 for serial 0
  // use timer1 for baudrate generator
  rclk = 0; // uart0 receive clk
  tclk = 0; // uart0 send clk
  pcon |= smod;

  tmod = tmod & ~bt1_gate & ~bt1_ct & ~mask_t1_mod |
         bt1_m1; // 0x20,timer1 as 8 bit autoload timer
  t2mod = t2mod | btmr_clk |
          bt1_clk; // timer1 clk selection. trade off: btmr_clk is low, will
                   // make uart0 not accurate. btmr_clk is high, make t2 timeout
                   // is short, multiple timeout must be allowed in 1st edge, as
                   // reset may take long
  th1 = 0 - x;     // baud/12 is real rate
  tr1 = 1;         // start timer1
  ti = 0;
  ren = 1; // enable serial 0 receive

  es = 1; // enable serial 0 interrupt

  serial0initialized = 1;
}

uint8_t serial0_write(__data uint8_t senddat) {
  __data uint8_t interrupton = ea;
  ea = 0;
  if ((uart0_tx_buffer_head == uart0_tx_buffer_tail) &&
      (uart0_flag_sending == 0)) { // start to send
    uart0_flag_sending = 1;
    sbuf = senddat;
    if (interrupton)
      ea = 1;
    return 1;
  }

  __data uint8_t nextheadpos =
      ((uint8_t)(uart0_tx_buffer_head + 1)) % serial0_tx_buffer_size;

  __data uint16_t waitwritecount = 0;
  while ((nextheadpos == uart0_tx_buffer_tail)) { // wait max 100ms or discard
    if (interrupton)
      ea = 1;
    waitwritecount++;
    delaymicroseconds(5);
    if (waitwritecount >= 20000)
      return 0;
  }
  transmit_uart0_buf[uart0_tx_buffer_head] = senddat;

  uart0_tx_buffer_head = nextheadpos;

  if (interrupton)
    ea = 1;

  return 1;
}

void serial0_flush(void) {
  while (uart0_flag_sending)
    ;
}

uint8_t serial0_available(void) {
  __data uint8_t rxbuflength =
      ((uint8_t)(serial0_rx_buffer_size + uart0_rx_buffer_head -
                 uart0_rx_buffer_tail)) %
      serial0_rx_buffer_size;
  return rxbuflength;
}

uint8_t serial0_read(void) {
  __data uint8_t rxbuflength =
      ((uint8_t)(serial0_rx_buffer_size + uart0_rx_buffer_head -
                 uart0_rx_buffer_tail)) %
      serial0_rx_buffer_size;
  if (rxbuflength > 0) {
    __data uint8_t result = receive_uart0_buf[uart0_rx_buffer_tail];
    uart0_rx_buffer_tail =
        (((uint8_t)(uart0_rx_buffer_tail + 1)) % serial0_rx_buffer_size);
    return result;
  }
  return 0;
}

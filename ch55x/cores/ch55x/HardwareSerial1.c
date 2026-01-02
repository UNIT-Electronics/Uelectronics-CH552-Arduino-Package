#include "hardwareserial.h"

__xdata unsigned char serial1initialized;

extern __xdata uint8_t receive_uart1_buf[];  // arduino style serial buffer
extern __xdata uint8_t transmit_uart1_buf[]; // arduino style serial buffer
extern volatile __xdata uint8_t uart1_rx_buffer_head;
extern volatile __xdata uint8_t uart1_rx_buffer_tail;
extern volatile __xdata uint8_t uart1_tx_buffer_head;
extern volatile __xdata uint8_t uart1_tx_buffer_tail;
extern volatile __bit uart1_flag_sending;

// extern wait functions
void delaymicroseconds(__data uint16_t us);

uint8_t serial1(void) { return serial1initialized; }

void serial1_begin(__data unsigned long baud) {

#if defined(ch551) || defined(ch552)
  u1sm0 = 0;
  u1smod = 1; // use mode 1 for serial 1
  u1ren = 1;  // enable serial 1 receive

  sbaud1 = 256 - f_cpu / 16 / baud;

  ie_uart1 = 1;
  ea = 1; // enable serial 1 interrupt
#elif defined(ch559)
  __data uint32_t x;
  __data uint8_t x2;
  ser1_lcr |= blcr_dlab; // change baudrate
  ser1_div = 1;
  x = 10 * f_cpu * 2 / 1 / 16 / baud;
  x2 = ((uint16_t)x) % 10;
  x /= 10;
  if (x2 >= 5)
    x++; // round
  ser1_dlm = x >> 8;
  ser1_dll = x & 0xff;
  ser1_lcr &= ~blcr_dlab;                   // prevent changing baudrate
  xbus_aux |= bale_clk_en;                  // make rs485en = 0
  ser1_lcr = blcr_word_sz1 | blcr_word_sz0; // no break, no parity, 8n1
  ser1_ier =
      bier_pin_mod1 | bier_thr_empty | bier_recv_rdy; // rxd1:p2.6 txd1:p2.7
  ser1_mcr |= bmcr_out2;
  ie_uart1 = 1;
  ea = 1;
#elif defined(ch549)
  scon1 = bu1ren | bu1smod;
  sbaud1 = 256 - f_cpu / 16 / baud;
  sif1 = bu1ti | bu1ri; // clear interrupt flags
  ie_uart1 = 1;
  ea = 1;
#endif
  serial1initialized = 1;
}

uint8_t serial1_write(__data uint8_t senddat) {
  __data uint8_t interrupton = ea;
  ea = 0;

  if ((uart1_tx_buffer_head == uart1_tx_buffer_tail) &&
      (uart1_flag_sending == 0)) { // start to send
    uart1_flag_sending = 1;
#if defined(ch551) || defined(ch552)
    sbuf1 = senddat;
#elif defined(ch559)
    ser1_thr = senddat;
#elif defined(ch549)
    sbuf1 = senddat;
#endif
    if (interrupton)
      ea = 1;
    return 1;
  }

  __data uint8_t nextheadpos =
      ((uint8_t)(uart1_tx_buffer_head + 1)) % serial1_tx_buffer_size;

  __data uint16_t waitwritecount = 0;
  while ((nextheadpos == uart1_tx_buffer_tail)) { // wait max 100ms or discard
    if (interrupton)
      ea = 1;
    waitwritecount++;
    delaymicroseconds(5);
    if (waitwritecount >= 20000)
      return 0;
  }
  transmit_uart1_buf[uart1_tx_buffer_head] = senddat;

  uart1_tx_buffer_head = nextheadpos;

  if (interrupton)
    ea = 1;

  return 1;
}

void serial1_flush(void) {
  while (uart1_flag_sending)
    ;
}

uint8_t serial1_available(void) {
  __data uint8_t rxbuflength =
      ((uint8_t)(serial1_rx_buffer_size + uart1_rx_buffer_head -
                 uart1_rx_buffer_tail)) %
      serial1_rx_buffer_size;
  return rxbuflength;
}

uint8_t serial1_read(void) {
  __data uint8_t rxbuflength =
      ((uint8_t)(serial1_rx_buffer_size + uart1_rx_buffer_head -
                 uart1_rx_buffer_tail)) %
      serial1_rx_buffer_size;
  if (rxbuflength > 0) {
    __data uint8_t result = receive_uart1_buf[uart1_rx_buffer_tail];
    uart1_rx_buffer_tail =
        (((uint8_t)(uart1_rx_buffer_tail + 1)) % serial1_rx_buffer_size);
    return result;
  }
  return 0;
}

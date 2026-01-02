/*
 created by deqing sun for use with ch55xduino
 */

#ifndef user_usb_ram

// clang-format off
#include <stdint.h>
#include <stdbool.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
// clang-format on

extern __xdata uint8_t ep0buffer[];
extern __xdata uint8_t ep2buffer[];

#define line_codeing_size 7
__xdata uint8_t linecoding[line_codeing_size] = {
    0x00, 0xe1, 0x00, 0x00,
    0x00, 0x00, 0x08}; // initialize for baudrate 57600, 1 stopbit, no parity,
                       // eight data bits

volatile __xdata uint8_t usbbytecountep2 =
    0; // bytes of received data on usb endpoint
volatile __xdata uint8_t usbbufoutpointep2 = 0; // data pointer for fetching

volatile __bit uppoint2busyflag = 0; // flag of whether upload pointer is busy
volatile __xdata uint8_t controllinestate = 0;

__xdata uint8_t usbwritepointer = 0;

void delaymicroseconds(__data uint16_t us);

void resetcdcparameters() {

  usbbytecountep2 = 0; // bytes of received data on usb endpoint
  uppoint2busyflag = 0;
}

void setlinecodinghandler() {
  for (__data uint8_t i = 0;
       i < ((line_codeing_size <= usb_rx_len) ? line_codeing_size : usb_rx_len);
       i++) {
    linecoding[i] = ep0buffer[i];
  }

  //!!!!!config_uart0(linecoding);
}

uint16_t getlinecodinghandler() {
  __data uint16_t returnlen;

  returnlen = line_codeing_size;
  for (__data uint8_t i = 0; i < returnlen; i++) {
    ep0buffer[i] = linecoding[i];
  }

  return returnlen;
}

void setcontrollinestatehandler() {
  controllinestate = ep0buffer[2];

  // we check dtr state to determine if host port is open (bit 0 of linestate).
  if (((controllinestate & 0x01) == 0) &&
      (*((__xdata uint32_t *)linecoding) ==
       1200)) { // both linecoding and sdcc are little-endian

#if boot_load_addr == 0x3800
    usb_ctrl = 0;
    ea = 0; // disabling all interrupts is required.
    tmod = 0;
    delaymicroseconds(50000);
    delaymicroseconds(50000);

    __asm__("lcall #0x3800"); // jump to bootloader code

    while (1)
      ;
#elif defined(ch559) && (boot_load_addr == 0xf400)
    usb_ctrl = 0;
    ea = 0; // disabling all interrupts is required.
    delaymicroseconds(50000);
    delaymicroseconds(50000);

    __asm__("lcall #0xf400"); // jump to bootloader code

    while (1)
      ;
#elif boot_load_addr == 0xf400
    // todo: not working well, ch549 doesn't support direct jump
#endif
  }
}

uint8_t usbserial_wait_uppoint2busyflag_clear() {
  __data uint16_t waitwritecount = 0;
  while (uppoint2busyflag) { // wait for 250ms or give up, on my mac it takes
                             // about 256us
    waitwritecount++;
    delaymicroseconds(5);
    if (waitwritecount >= 50000)
      return 0;
  }
  return 1;
}

bool usbserial() {
  __data bool result = false;
  if (controllinestate > 0)
    result = true;
  // delay(10); not doing it for now
  return result;
}

void usbserial_flush(void) {
  if (!uppoint2busyflag && usbwritepointer > 0) {
    uep2_t_len = usbwritepointer;
    uep2_ctrl = uep2_ctrl & ~mask_uep_t_res | uep_t_res_ack; // respond ack
    uppoint2busyflag = 1;

    if (usbwritepointer ==
        max_packet_size) { // write empty packet for end transmission. needed
                           // for windows.
      if (usbserial_wait_uppoint2busyflag_clear()) {
        uep2_t_len = 0;
        uep2_ctrl = uep2_ctrl & ~mask_uep_t_res | uep_t_res_ack; // respond ack
        uppoint2busyflag = 1;
      }
    }
    usbwritepointer = 0;
  }
}

uint8_t usbserial_write(__data char c) { // 3 bytes generic pointer
  if (controllinestate > 0) {
    while (true) {
      if (usbserial_wait_uppoint2busyflag_clear() == 0)
        return 0;
      if (usbwritepointer < max_packet_size) {
        ep2buffer[max_packet_size + usbwritepointer] = c;
        usbwritepointer++;
        return 1;
      } else {
        usbserial_flush(); // go back to first while
      }
    }
  }
  return 0;
}

uint8_t
usbserial_print_n(uint8_t *__xdata buf,
                  __xdata int len) { // 3 bytes generic pointer, not using
                                     // usbserial_write for a bit efficiency
  if (controllinestate > 0) {
    while (len > 0) {
      if (usbserial_wait_uppoint2busyflag_clear() == 0)
        return 0;
      while (len > 0) {
        if (usbwritepointer < max_packet_size) {
          ep2buffer[max_packet_size + usbwritepointer] = *buf++;
          usbwritepointer++;
          len--;
        } else {
          usbserial_flush(); // go back to first while
          break;
        }
      }
    }
  }
  return 0;
}

uint8_t usbserial_available() { return usbbytecountep2; }

char usbserial_read() {
  if (usbbytecountep2 == 0)
    return 0;
  __data char data = ep2buffer[usbbufoutpointep2];
  usbbufoutpointep2++;
  usbbytecountep2--;
  if (usbbytecountep2 == 0) {
    uep2_ctrl = uep2_ctrl & ~mask_uep_r_res | uep_r_res_ack;
  }
  return data;
}

void usb_ep2_in() {
  uep2_t_len = 0; // no data to send anymore
  uep2_ctrl =
      uep2_ctrl & ~mask_uep_t_res | uep_t_res_nak; // respond nak by default
  uppoint2busyflag = 0;                            // clear busy flag
}

void usb_ep2_out() {
  if (u_tog_ok) // discard unsynchronized packets
  {
    usbbytecountep2 = usb_rx_len;
    usbbufoutpointep2 = 0; // reset data pointer for fetching
    if (usbbytecountep2)
      uep2_ctrl = uep2_ctrl & ~mask_uep_r_res |
                  uep_r_res_nak; // respond nak after a packet. let main code
                                 // change response after handling.
  }
}

#endif

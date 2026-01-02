// ===================================================================================
// usb cdc functions for ch551, ch552 and ch554
// ===================================================================================

#include "ch554.h"
#include "usb.h"
#include "usb_cdc.h"
#include "usb_descr.h"
#include "usb_handler.h"

// ===================================================================================
// variables and defines
// ===================================================================================

// initialize line coding
__xdata cdc_line_coding_type cdc_linecodingb = {
  .baudrate = 57600,        // baudrate 57600
  .stopbits = 0,            // 1 stopbit
  .parity   = 0,            // no parity
  .databits = 8             // 8 databits
};

volatile __xdata uint8_t cdc_controllinestate  = 0; // control line state
volatile __xdata uint8_t cdc_ep2_readbytecount = 0; // number of data bytes in in buffer
volatile __xdata uint8_t cdc_ep2_readpointer   = 0; // data pointer for fetching
volatile __bit cdc_ep2_writebusyflag = 0;           // flag of whether upload pointer is busy
__xdata uint8_t cdc_ep2_writepointer = 0;           // data pointer for writing

#define cdc_dtr_flag  (cdc_controllinestate & 1)
#define cdc_rts_flag  ((cdc_controllinestate >> 1) & 1)

// ===================================================================================
// front end functions
// ===================================================================================

// setup usb-cdc
void cdc_init(void) {
  usb_init();
  uep1_t_len  = 0;
  uep2_t_len  = 0;
}

// check number of bytes in the in buffer
uint8_t cdc_available(void) {
  return cdc_ep2_readbytecount;
}

// check if out buffer is ready to be written
__bit cdc_ready(void) {
  return(cdc_dtr_flag && !cdc_ep2_writebusyflag);
}

// flush the out buffer
void cdc_flush(void) {
  if(!cdc_ep2_writebusyflag && cdc_ep2_writepointer > 0) {  // not busy and buffer not empty?
    uep2_t_len = cdc_ep2_writepointer;                      // number of bytes in out buffer
    uep2_ctrl  = uep2_ctrl & ~mask_uep_t_res | uep_t_res_ack; // respond ack
    cdc_ep2_writebusyflag = 1;                              // busy for now
    cdc_ep2_writepointer  = 0;                              // reset write pointer
  }
}

// write single character to out buffer
void cdc_write(char c) {
  while(!cdc_ready());                                      // wait for ready to write
  ep2_buffer[max_packet_size + cdc_ep2_writepointer++] = c; // write character
  if(cdc_ep2_writepointer == ep2_size) cdc_flush();         // flush if buffer full
}

// write string to out buffer
void cdc_print(char* str) {
  while(*str) cdc_write(*str++);                            // write each char of string
}

// write string with newline to out buffer and flush
void cdc_println(char* str) {
  cdc_print(str);                                           // write string
  cdc_write('\n');                                          // write new line
  cdc_flush();                                              // flush out buffer
}

// read single character from in buffer
char cdc_read(void) {
  char data;
  while(!cdc_available());                                  // wait for data
  data = ep2_buffer[cdc_ep2_readpointer++];                 // get character
  if(--cdc_ep2_readbytecount == 0)                          // dec number of bytes in buffer
    uep2_ctrl = uep2_ctrl & ~mask_uep_r_res | uep_r_res_ack;// request new data if empty
  return data;
}

// get dtr flag
__bit cdc_getdtr(void) {
  return cdc_dtr_flag;
}

// get rts flag
__bit cdc_getrts(void) {
  return cdc_rts_flag;
}

// ===================================================================================
// cdc-specific usb handler functions
// ===================================================================================

// cdc class requests
#define set_line_coding         0x20  // host configures line coding
#define get_line_coding         0x21  // host reads configured line coding
#define set_control_line_state  0x22  // generates rs-232/v.24 style control signals

// set line coding handler
void cdc_setlinecoding(void) {
  uint8_t i;
  for(i=0; i<((sizeof(cdc_linecodingb)<=usb_rx_len)?sizeof(cdc_linecodingb):usb_rx_len); i++)
    ((uint8_t*)&cdc_linecodingb)[i] = ep0_buffer[i]; // receive line coding from host
}

// get line coding handler
uint8_t cdc_getlinecoding(void) {
  uint8_t i;
  for(i=0; i<sizeof(cdc_linecodingb); i++)
    ep0_buffer[i] = ((uint8_t*)&cdc_linecodingb)[i]; // transmit line coding to host
  return sizeof(cdc_linecodingb);
}

// set control line state handler
void cdc_setcontrollinestate(void) {
  cdc_controllinestate = ep0_buffer[2];     // read control line state
}

// setup cdc endpoints
void cdc_setup(void) {
  uep1_dma    = ep1_addr;                   // ep1 data transfer address
  uep2_dma    = ep2_addr;                   // ep2 data transfer address
  uep1_ctrl   = buep_auto_tog               // ep1 auto flip sync flag
              | uep_t_res_nak;              // ep1 in transaction returns nak
  uep2_ctrl   = buep_auto_tog               // ep2 auto flip sync flag
              | uep_t_res_nak               // ep2 in transaction returns nak
              | uep_r_res_ack;              // ep2 out transaction returns ack
  uep2_3_mod  = buep2_rx_en | buep2_tx_en;  // ep2 double buffer (0x0c)
  uep4_1_mod  = buep1_tx_en;                // ep1 tx enable (0x40)
}

// reset cdc parameters
void cdc_reset(void) {
  uep1_ctrl = buep_auto_tog | uep_t_res_nak;
  uep2_ctrl = buep_auto_tog | uep_t_res_nak | uep_r_res_ack;
  cdc_ep2_readbytecount = 0;                // reset received bytes counter
  cdc_ep2_writebusyflag = 0;                // reset write busy flag
}

// handle non-standard control requests
uint8_t cdc_control(void) {
  if((usb_setupbuf->brequesttype & usb_req_typ_mask) == usb_req_typ_class) {
    switch(usb_setupbuf->brequest) {
      case get_line_coding:                 // 0x21  currently configured
        return cdc_getlinecoding();
      case set_control_line_state:          // 0x22  generates rs-232/v.24 style control signals
        cdc_setcontrollinestate();
        return 0;
      case set_line_coding:                 // 0x20  configure
        return 0;            
      default:
        return 0xff;                        // command not supported
    }
  }
  else return 0xff;
}

// endpoint 0 out handler
void cdc_ep0_out(void) {
  if(setupreq == set_line_coding) {         // set line coding
    if(u_tog_ok) {
      cdc_setlinecoding();
      uep0_t_len = 0;
      uep0_ctrl |= uep_r_res_ack | uep_t_res_ack;           // send 0-length packet
    }
  }
  else {
    uep0_t_len = 0;
    uep0_ctrl |= uep_r_res_ack | uep_t_res_nak;             // respond nak
  }
}

// endpoint 1 in handler
void cdc_ep1_in(void) {
  uep1_t_len = 0;
  uep1_ctrl = uep1_ctrl & ~ mask_uep_t_res | uep_t_res_nak; // default nak
}

// endpoint 2 in handler (bulk data transfer to host)
void cdc_ep2_in(void) {
  uep2_t_len = 0;                                           // no data to send anymore
  uep2_ctrl = uep2_ctrl & ~mask_uep_t_res | uep_t_res_nak;  // respond nak by default
  cdc_ep2_writebusyflag = 0;                                // clear busy flag
}

// endpoint 2 out handler (bulk data transfer from host)
void cdc_ep2_out(void) {
  if(u_tog_ok) {                                            // discard unsynchronized packets
    cdc_ep2_readbytecount = usb_rx_len;                     // set number of received data bytes
    cdc_ep2_readpointer = 0;                                // reset read pointer for fetching
    if(cdc_ep2_readbytecount) 
      uep2_ctrl = uep2_ctrl & ~mask_uep_r_res | uep_r_res_nak; // respond nak after a packet. let main code change response after handling.
  }
}

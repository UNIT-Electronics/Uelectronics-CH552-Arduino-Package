/*
 created by deqing sun for use with ch55xduino
 */

#ifndef user_usb_ram

#include "usbhandler.h"

#include "usbconstant.h"

// cdc functions:
void resetcdcparameters();
void setlinecodinghandler();
uint16_t getlinecodinghandler();
void setcontrollinestatehandler();
void usb_ep2_in();
void usb_ep2_out();

// clang-format off
__xdata __at (ep0_addr) uint8_t ep0buffer[8];
__xdata __at (ep1_addr) uint8_t ep1buffer[8];       //on page 47 of data sheet, the receive buffer need to be min(possible packet size+2,64)
__xdata __at (ep2_addr) uint8_t ep2buffer[128];     //in and out buffer, must be even address
// clang-format on

__data uint16_t setuplen;
__data uint8_t setupreq;
volatile __xdata uint8_t usbconfig;

__code uint8_t *__data pdescr;

inline void nop_process(void) {}

void usb_ep0_setup() {
  __data uint8_t len = usb_rx_len;
  if (len == (sizeof(usb_setup_req))) {
    setuplen = ((uint16_t)usbsetupbuf->wlengthh << 8) | (usbsetupbuf->wlengthl);
    len = 0; // default is success and upload 0 length
    setupreq = usbsetupbuf->brequest;
    if ((usbsetupbuf->brequesttype & usb_req_typ_mask) !=
        usb_req_typ_standard) // not standard request
    {

      // here is the commnunication starts, refer to usbfunctionsetup of usbtiny
      // or usb_setup in usbtiny

      switch ((usbsetupbuf->brequesttype & usb_req_typ_mask)) {
      case usb_req_typ_vendor: {
        switch (setupreq) {
        default:
          len = 0xff; // command not supported
          break;
        }
        break;
      }
      case usb_req_typ_class: {
        switch (setupreq) {
        case get_line_coding: // 0x21  currently configured
          len = getlinecodinghandler();
          break;
        case set_control_line_state: // 0x22  generates rs-232/v.24 style
                                     // control signals
          setcontrollinestatehandler();
          break;
        case set_line_coding: // 0x20  configure
          break;

        default:
          len = 0xff; // command not supported
          break;
        }
        break;
      }
      default:
        len = 0xff; // command not supported
        break;
      }

    } else // standard request
    {
      switch (setupreq) // request ccftype
      {
      case usb_get_descriptor:
        switch (usbsetupbuf->wvalueh) {
        case 1: // device descriptor
          pdescr = (__code uint8_t *)
              devicedescriptor; // put device descriptor into outgoing buffer
          len = sizeof(usb_descriptor_device_t);
          break;
        case 2: // configure descriptor
          pdescr = (__code uint8_t *)configurationdescriptor;
          len = sizeof(usb_descriptor_configuration_t);
          break;
        case 3:
          switch (usbsetupbuf->wvaluel) {
          case 0:
            pdescr = (__code uint8_t *)languagedescriptor;
            break;
          case 1:
            pdescr = (__code uint8_t *)manufacturerdescriptor;
            break;
          case 2:
            pdescr = (__code uint8_t *)productdescriptor;
            break;
          case 3:
            pdescr = (__code uint8_t *)serialdescriptor;
            break;
          case 4:
            pdescr = (__code uint8_t *)cdcdescriptor;
            break;
          default:
            len = 0xff;
            break;
          }
          len = pdescr[0];
          break;
        default:
          len = 0xff; // unsupported descriptors or error
          break;
        }
        if (len != 0xff) {
          if (setuplen > len) {
            setuplen = len; // limit length
          }
          len = setuplen >= default_endp0_size
                    ? default_endp0_size
                    : setuplen; // transmit length for this packet
          for (__data uint8_t i = 0; i < len; i++) {
            ep0buffer[i] = pdescr[i];
          }
          setuplen -= len;
          pdescr += len;
        }
        break;
      case usb_set_address:
        setuplen = usbsetupbuf->wvaluel; // save the assigned address
        break;
      case usb_get_configuration:
        ep0buffer[0] = usbconfig;
        if (setuplen >= 1) {
          len = 1;
        }
        break;
      case usb_set_configuration:
        usbconfig = usbsetupbuf->wvaluel;
        break;
      case usb_get_interface:
        break;
      case usb_set_interface:
        break;
      case usb_clear_feature: // clear feature
        if ((usbsetupbuf->brequesttype & 0x1f) ==
            usb_req_recip_device) // clear the device featuee.
        {
          if ((((uint16_t)usbsetupbuf->wvalueh << 8) | usbsetupbuf->wvaluel) ==
              0x01) {
            if (configurationdescriptor.config.configattributes & 0x20) {
              // wake up
            } else {
              len = 0xff; // failed
            }
          } else {
            len = 0xff; // failed
          }
        } else if ((usbsetupbuf->brequesttype & usb_req_recip_mask) ==
                   usb_req_recip_endp) // endpoint
        {
          switch (usbsetupbuf->windexl) {
          case 0x84:
            uep4_ctrl =
                uep4_ctrl & ~(buep_t_tog | mask_uep_t_res) | uep_t_res_nak;
            break;
          case 0x04:
            uep4_ctrl =
                uep4_ctrl & ~(buep_r_tog | mask_uep_r_res) | uep_r_res_ack;
            break;
          case 0x83:
            uep3_ctrl =
                uep3_ctrl & ~(buep_t_tog | mask_uep_t_res) | uep_t_res_nak;
            break;
          case 0x03:
            uep3_ctrl =
                uep3_ctrl & ~(buep_r_tog | mask_uep_r_res) | uep_r_res_ack;
            break;
          case 0x82:
            uep2_ctrl =
                uep2_ctrl & ~(buep_t_tog | mask_uep_t_res) | uep_t_res_nak;
            break;
          case 0x02:
            uep2_ctrl =
                uep2_ctrl & ~(buep_r_tog | mask_uep_r_res) | uep_r_res_ack;
            break;
          case 0x81:
            uep1_ctrl =
                uep1_ctrl & ~(buep_t_tog | mask_uep_t_res) | uep_t_res_nak;
            break;
          case 0x01:
            uep1_ctrl =
                uep1_ctrl & ~(buep_r_tog | mask_uep_r_res) | uep_r_res_ack;
            break;
          default:
            len = 0xff; // unsupported endpoint
            break;
          }
        } else {
          len = 0xff; // unsupported for non-endpoint
        }
        break;
      case usb_set_feature: // set feature
        if ((usbsetupbuf->brequesttype & 0x1f) ==
            usb_req_recip_device) // set  the device featuee.
        {
          if ((((uint16_t)usbsetupbuf->wvalueh << 8) | usbsetupbuf->wvaluel) ==
              0x01) {
            if (configurationdescriptor.config.configattributes & 0x20) {
              // suspend

              // while ( xbus_aux & buart0_tx );    //wait till uart0 sending
              // complete safe_mod = 0x55; safe_mod = 0xaa; wake_ctrl =
              // bwak_by_usb | bwak_rxd0_lo | bwak_rxd1_lo; //wake up by usb or
              // rxd0/1 signal pcon |= pd; //sleep safe_mod = 0x55; safe_mod =
              // 0xaa; wake_ctrl = 0x00;
            } else {
              len = 0xff; // failed
            }
          } else {
            len = 0xff; // failed
          }
        } else if ((usbsetupbuf->brequesttype & 0x1f) ==
                   usb_req_recip_endp) // endpoint
        {
          if ((((uint16_t)usbsetupbuf->wvalueh << 8) | usbsetupbuf->wvaluel) ==
              0x00) {
            switch (((uint16_t)usbsetupbuf->windexh << 8) |
                    usbsetupbuf->windexl) {
            case 0x84:
              uep4_ctrl = uep4_ctrl & (~buep_t_tog) |
                          uep_t_res_stall; // set endpoint4 in stall
              break;
            case 0x04:
              uep4_ctrl = uep4_ctrl & (~buep_r_tog) |
                          uep_r_res_stall; // set endpoint4 out stall
              break;
            case 0x83:
              uep3_ctrl = uep3_ctrl & (~buep_t_tog) |
                          uep_t_res_stall; // set endpoint3 in stall
              break;
            case 0x03:
              uep3_ctrl = uep3_ctrl & (~buep_r_tog) |
                          uep_r_res_stall; // set endpoint3 out stall
              break;
            case 0x82:
              uep2_ctrl = uep2_ctrl & (~buep_t_tog) |
                          uep_t_res_stall; // set endpoint2 in stall
              break;
            case 0x02:
              uep2_ctrl = uep2_ctrl & (~buep_r_tog) |
                          uep_r_res_stall; // set endpoint2 out stall
              break;
            case 0x81:
              uep1_ctrl = uep1_ctrl & (~buep_t_tog) |
                          uep_t_res_stall; // set endpoint1 in stall
              break;
            case 0x01:
              uep1_ctrl = uep1_ctrl & (~buep_r_tog) |
                          uep_r_res_stall; // set endpoint1 out stall
            default:
              len = 0xff; // failed
              break;
            }
          } else {
            len = 0xff; // failed
          }
        } else {
          len = 0xff; // failed
        }
        break;
      case usb_get_status:
        ep0buffer[0] = 0x00;
        ep0buffer[1] = 0x00;
        if (setuplen >= 2) {
          len = 2;
        } else {
          len = setuplen;
        }
        break;
      default:
        len = 0xff; // failed
        break;
      }
    }
  } else {
    len = 0xff; // wrong packet length
  }
  if (len == 0xff) {
    setupreq = 0xff;
    uep0_ctrl =
        buep_r_tog | buep_t_tog | uep_r_res_stall | uep_t_res_stall; // stall
  } else if (len <=
             default_endp0_size) // tx data to host or send 0-length packet
  {
    uep0_t_len = len;
    uep0_ctrl = buep_r_tog | buep_t_tog | uep_r_res_ack |
                uep_t_res_ack; // expect data1, answer ack
  } else {
    uep0_t_len = 0; // tx data to host or send 0-length packet
    uep0_ctrl = buep_r_tog | buep_t_tog | uep_r_res_ack |
                uep_t_res_ack; // expect data1, answer ack
  }
}

void usb_ep0_in() {
  switch (setupreq) {
  case usb_get_descriptor: {
    __data uint8_t len = setuplen >= default_endp0_size
                             ? default_endp0_size
                             : setuplen; // send length
    for (__data uint8_t i = 0; i < len; i++) {
      ep0buffer[i] = pdescr[i];
    }
    // memcpy( ep0buffer, pdescr, len );
    setuplen -= len;
    pdescr += len;
    uep0_t_len = len;
    uep0_ctrl ^= buep_t_tog; // switch between data0 and data1
  } break;
  case usb_set_address:
    usb_dev_ad = usb_dev_ad & buda_gp_bit | setuplen;
    uep0_ctrl = uep_r_res_ack | uep_t_res_nak;
    break;
  default:
    uep0_t_len = 0; // end of transaction
    uep0_ctrl = uep_r_res_ack | uep_t_res_nak;
    break;
  }
}

void usb_ep0_out() {
  if (setupreq == set_line_coding) // set line coding
  {
    if (u_tog_ok) {
      setlinecodinghandler();
      uep0_t_len = 0;
      uep0_ctrl |= uep_r_res_ack | uep_t_res_ack; // send 0-length packet
    }
  } else {
    uep0_t_len = 0;
    uep0_ctrl |= uep_r_res_ack | uep_t_res_nak; // respond nak
  }
}

void usb_ep1_in() {
  uep1_t_len = 0;
  uep1_ctrl = uep1_ctrl & ~mask_uep_t_res | uep_t_res_nak; // default nak
}

#pragma save
#pragma nooverlay
void usbinterrupt(void) { // inline not really working in multiple files in sdcc
  if (uif_transfer) {
    // dispatch to service functions
    __data uint8_t callindex = usb_int_st & mask_uis_endp;
    switch (usb_int_st & mask_uis_token) {
    case uis_token_out: { // sdcc will take iram if array of function pointer is
                          // used.
      switch (callindex) {
      case 0:
        ep0_out_callback();
        break;
      case 1:
        ep1_out_callback();
        break;
      case 2:
        ep2_out_callback();
        break;
      case 3:
        ep3_out_callback();
        break;
      case 4:
        ep4_out_callback();
        break;
      default:
        break;
      }
    } break;
    case uis_token_sof: { // sdcc will take iram if array of function pointer is
                          // used.
      switch (callindex) {
      case 0:
        ep0_sof_callback();
        break;
      case 1:
        ep1_sof_callback();
        break;
      case 2:
        ep2_sof_callback();
        break;
      case 3:
        ep3_sof_callback();
        break;
      case 4:
        ep4_sof_callback();
        break;
      default:
        break;
      }
    } break;
    case uis_token_in: { // sdcc will take iram if array of function pointer is
                         // used.
      switch (callindex) {
      case 0:
        ep0_in_callback();
        break;
      case 1:
        ep1_in_callback();
        break;
      case 2:
        ep2_in_callback();
        break;
      case 3:
        ep3_in_callback();
        break;
      case 4:
        ep4_in_callback();
        break;
      default:
        break;
      }
    } break;
    case uis_token_setup: { // sdcc will take iram if array of function pointer
                            // is used.
      switch (callindex) {
      case 0:
        ep0_setup_callback();
        break;
      case 1:
        ep1_setup_callback();
        break;
      case 2:
        ep2_setup_callback();
        break;
      case 3:
        ep3_setup_callback();
        break;
      case 4:
        ep4_setup_callback();
        break;
      default:
        break;
      }
    } break;
    }

    uif_transfer = 0; // clear interrupt flag
  }

  // device mode usb bus reset
  if (uif_bus_rst) {
    uep0_ctrl = uep_r_res_ack | uep_t_res_nak;
    uep1_ctrl = buep_auto_tog |
                uep_t_res_nak; // endpoint 1 automatically flips the sync flag,
                               // and in transaction returns nak
    uep2_ctrl = buep_auto_tog | uep_t_res_nak |
                uep_r_res_ack; // endpoint 2 automatically flips the sync flag,
                               // in transaction returns nak, out returns ack
    // uep4_ctrl = uep_t_res_nak | uep_r_res_ack;  //buep_auto_tog only work for
    // endpoint 1,2,3

    usb_dev_ad = 0x00;
    uif_suspend = 0;
    uif_transfer = 0;
    uif_bus_rst = 0; // clear interrupt flag

    usbconfig = 0;

    resetcdcparameters();
  }

  // usb bus suspend / wake up
  if (uif_suspend) {
    uif_suspend = 0;
    if (usb_mis_st & bums_suspend) { // suspend

      // while ( xbus_aux & buart0_tx );                    // wait for tx
      // safe_mod = 0x55;
      // safe_mod = 0xaa;
      // wake_ctrl = bwak_by_usb | bwak_rxd0_lo;    // wake up by usb or rxd0
      // pcon |= pd; // chip sleep safe_mod = 0x55; safe_mod = 0xaa; wake_ctrl =
      // 0x00;

    } else {             // unexpected interrupt, not supposed to happen !
      usb_int_fg = 0xff; // clear interrupt flag
    }
  }
}
#pragma restore

void usbdevicecfg() {
  usb_ctrl = 0x00;            // clear usb control register
  usb_ctrl &= ~buc_host_mode; // this bit is the device selection mode
  usb_ctrl |= buc_dev_pu_en | buc_int_busy |
              buc_dma_en; // usb device and internal pull-up enable,
                          // automatically return to nak before interrupt flag
                          // is cleared during interrupt
  usb_dev_ad = 0x00;      // device address initialization
  //     usb_ctrl |= buc_low_speed;
  //     udev_ctrl |= bud_low_speed; //run for 1.5m
  usb_ctrl &= ~buc_low_speed;
  udev_ctrl &= ~bud_low_speed; // select full speed 12m mode, default mode

#if defined(ch551) || defined(ch552) || defined(ch549)
  udev_ctrl = bud_pd_dis; // disable dp/dm pull-down resistor
#endif
#if defined(ch559)
  udev_ctrl = bud_dp_pd_dis; // disable dp/dm pull-down resistor
#endif
  udev_ctrl |= bud_port_en; // enable physical port
}

void usbdeviceintcfg() {
  usb_int_en |= buie_suspend;  // enable device hang interrupt
  usb_int_en |= buie_transfer; // enable usb transfer completion interrupt
  usb_int_en |= buie_bus_rst;  // enable device mode usb bus reset interrupt
  usb_int_fg |= 0x1f;          // clear interrupt flag
  ie_usb = 1;                  // enable usb interrupt
  ea = 1;                      // enable global interrupts
}

void usbdeviceendpointcfg() {
#if defined(ch559)
  // ch559 use differend endianness for these registers
  uep0_dma_h = ((uint16_t)ep0buffer >> 8); // endpoint 0 data transfer address
  uep0_dma_l = ((uint16_t)ep0buffer >> 0); // endpoint 0 data transfer address
  uep1_dma_h = ((uint16_t)ep1buffer >> 8); // endpoint 1 data transfer address
  uep1_dma_l = ((uint16_t)ep1buffer >> 0); // endpoint 1 data transfer address
  uep2_dma_h = ((uint16_t)ep2buffer >> 8); // endpoint 2 data transfer address
  uep2_dma_l = ((uint16_t)ep2buffer >> 0); // endpoint 2 data transfer address
#else
  uep0_dma = (uint16_t)ep0buffer; // endpoint 0 data transfer address
  uep1_dma = (uint16_t)ep1buffer; // endpoint 1 data transfer address
  uep2_dma = (uint16_t)ep2buffer; // endpoint 2 data transfer address
#endif

  uep2_3_mod = 0x0c; // endpoint2 double buffer
  uep1_ctrl =
      buep_auto_tog | uep_t_res_nak; // endpoint 1 automatically flips the sync
                                     // flag, and in transaction returns nak
  uep2_ctrl = buep_auto_tog | uep_t_res_nak |
              uep_r_res_ack; // endpoint 2 automatically flips the sync flag, in
                             // transaction returns nak, out returns ack

  uep4_1_mod = 0x40; // endpoint1 tx enable
  uep0_ctrl =
      uep_r_res_ack | uep_t_res_nak; // manual flip, out transaction returns
                                     // ack, in transaction returns nak
}

#endif

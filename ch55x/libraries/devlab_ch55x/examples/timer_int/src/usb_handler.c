// ===================================================================================
// usb handler for ch551, ch552 and ch554
// ===================================================================================

#include "ch554.h"
#include "usb_handler.h"

uint16_t setuplen;
uint8_t  setupreq, usbconfig;
__code uint8_t *pdescr;

// ===================================================================================
// fast copy function
// ===================================================================================
// copy descriptor *pdescr to ep0 using double pointer
// (thanks to ralph doncaster)
#pragma callee_saves usb_ep0_copydescr
void usb_ep0_copydescr(uint8_t len) {
  len;                          // stop unreferenced argument warning
  __asm
    push ar7                    ; r7 -> stack
    mov  r7, dpl                ; r7 <- len
    inc  _xbus_aux              ; select dptr1
    mov  dptr, #_ep0_buffer     ; dptr1 <- ep0_buffer
    dec  _xbus_aux              ; select dptr0
    mov  dpl, _pdescr           ; dptr0 <- *pdescr
    mov  dph, (_pdescr + 1)
    01$:
    clr  a                      ; acc <- #0
    movc a, @a+dptr             ; acc <- *pdescr[dptr0]
    inc  dptr                   ; inc dptr0
    .db  0xa5                   ; acc -> ep0_buffer[dptr1] & inc dptr1
    djnz r7, 01$                ; repeat len times
    pop  ar7                    ; r7 <- stack
  __endasm;
}

// ===================================================================================
// endpoint handler
// ===================================================================================

void usb_ep0_setup(void) {
  uint8_t len = usb_rx_len;
  if(len == (sizeof(usb_setup_req))) {
    setuplen = ((uint16_t)usb_setupbuf->wlengthh<<8) | (usb_setupbuf->wlengthl);
    len = 0;                                      // default is success and upload 0 length
    setupreq = usb_setupbuf->brequest;

    if( (usb_setupbuf->brequesttype & usb_req_typ_mask) != usb_req_typ_standard ) {
      #ifdef usb_ctrl_ns_handler
      len = usb_ctrl_ns_handler();                // non-standard request
      #else
      len = 0xff;                                 // command not supported
      #endif
    }

    else {                                        // standard request
      switch(setupreq) {                          // request ccftype
        case usb_get_descriptor:
          switch(usb_setupbuf->wvalueh) {

            case usb_descr_typ_device:            // device descriptor
              pdescr = (uint8_t*)&devdescr;       // put descriptor into out buffer
              len = sizeof(devdescr);             // descriptor length
              break;

            case usb_descr_typ_config:            // configuration descriptor
              pdescr = (uint8_t*)&cfgdescr;       // put descriptor into out buffer
              len = sizeof(cfgdescr);             // descriptor length
              break;

            case usb_descr_typ_string:
              switch(usb_setupbuf->wvaluel) {      // string descriptor index
                case 0:   pdescr = usb_str_descr_i0; break;
                case 1:   pdescr = usb_str_descr_i1; break;
                case 2:   pdescr = usb_str_descr_i2; break;
                case 3:   pdescr = usb_str_descr_i3; break;
                #ifdef usb_str_descr_i4
                case 4:   pdescr = usb_str_descr_i4; break;
                #endif
                #ifdef usb_str_descr_i5
                case 5:   pdescr = usb_str_descr_i5; break;
                #endif
                #ifdef usb_str_descr_i6
                case 6:   pdescr = usb_str_descr_i6; break;
                #endif
                #ifdef usb_str_descr_i7
                case 7:   pdescr = usb_str_descr_i7; break;
                #endif
                #ifdef usb_str_descr_i8
                case 8:   pdescr = usb_str_descr_i8; break;
                #endif
                #ifdef usb_str_descr_i9
                case 9:   pdescr = usb_str_descr_i9; break;
                #endif
                default:  pdescr = usb_str_descr_ix; break;
              }
              len = pdescr[0];                    // descriptor length
              break;

            #ifdef usb_report_descr
            case usb_descr_typ_report:
              if(usb_setupbuf->wvaluel == 0) {
                pdescr = usb_report_descr;
                len = usb_report_descr_len;
              }
              else len = 0xff;
              break;
            #endif

            default:
              len = 0xff;                         // unsupported descriptors or error
              break;
          }

          if(len != 0xff) {
            if(setuplen > len) setuplen = len;    // limit length
            len = setuplen >= ep0_size ? ep0_size : setuplen;
            usb_ep0_copydescr(len);               // copy descriptor to ep0
            setuplen -= len;
            pdescr += len;
          }
          break;

        case usb_set_address:
          setuplen = usb_setupbuf->wvaluel;        // save the assigned address
          break;

        case usb_get_configuration:
          ep0_buffer[0] = usbconfig;
          if (setuplen >= 1) len = 1;
          break;

        case usb_set_configuration:
          usbconfig = usb_setupbuf->wvaluel;
          break;

        case usb_get_interface:
          break;

        case usb_set_interface:
          break;

        case usb_clear_feature:
          if( (usb_setupbuf->brequesttype & 0x1f) == usb_req_recip_device ) {
            if( ( ( (uint16_t)usb_setupbuf->wvalueh << 8 ) | usb_setupbuf->wvaluel ) == 0x01 ) {
              if( ((uint8_t*)&cfgdescr)[7] & 0x20) {
                // wake up
              }
              else len = 0xff;               // failed
            }
            else len = 0xff;                 // failed
          }
          else if( (usb_setupbuf->brequesttype & usb_req_recip_mask) == usb_req_recip_endp ) {
            switch(usb_setupbuf->windexl) {
              #ifdef ep4_in_callback
              case 0x84:
                uep4_ctrl = uep4_ctrl & ~ ( buep_t_tog | mask_uep_t_res ) | uep_t_res_nak;
                break;
              #endif
              #ifdef ep4_out_callback
              case 0x04:
                uep4_ctrl = uep4_ctrl & ~ ( buep_r_tog | mask_uep_r_res ) | uep_r_res_ack;
                break;
              #endif
              #ifdef ep3_in_callback
              case 0x83:
                uep3_ctrl = uep3_ctrl & ~ ( buep_t_tog | mask_uep_t_res ) | uep_t_res_nak;
                break;
              #endif
              #ifdef ep3_out_callback
              case 0x03:
                uep3_ctrl = uep3_ctrl & ~ ( buep_r_tog | mask_uep_r_res ) | uep_r_res_ack;
                break;
              #endif
              #ifdef ep2_in_callback
              case 0x82:
                uep2_ctrl = uep2_ctrl & ~ ( buep_t_tog | mask_uep_t_res ) | uep_t_res_nak;
                break;
              #endif
              #ifdef ep2_out_callback
              case 0x02:
                uep2_ctrl = uep2_ctrl & ~ ( buep_r_tog | mask_uep_r_res ) | uep_r_res_ack;
                break;
              #endif
              #ifdef ep1_in_callback
              case 0x81:
                uep1_ctrl = uep1_ctrl & ~ ( buep_t_tog | mask_uep_t_res ) | uep_t_res_nak;
                break;
              #endif
              #ifdef ep1_out_callback
              case 0x01:
                uep1_ctrl = uep1_ctrl & ~ ( buep_r_tog | mask_uep_r_res ) | uep_r_res_ack;
                break;
              #endif
              default:
                len = 0xff;                 // unsupported endpoint
                break;
            }
          }
          else len = 0xff;                  // unsupported for non-endpoint
          break;

        case usb_set_feature:
          if( (usb_setupbuf->brequesttype & 0x1f) == usb_req_recip_device ) {
            if( ( ( (uint16_t)usb_setupbuf->wvalueh << 8 ) | usb_setupbuf->wvaluel ) == 0x01 ) {
              if( !(((uint8_t*)&cfgdescr)[7] & 0x20) ) len = 0xff;  // failed
            }
            else len = 0xff;                                        // failed
          }
          else if( (usb_setupbuf->brequesttype & 0x1f) == usb_req_recip_endp ) {
            if( ( ( (uint16_t)usb_setupbuf->wvalueh << 8 ) | usb_setupbuf->wvaluel ) == 0x00 ) {
              switch( ( (uint16_t)usb_setupbuf->windexh << 8 ) | usb_setupbuf->windexl ) {
                #ifdef ep4_in_callback
                case 0x84:
                  uep4_ctrl = uep4_ctrl & (~buep_t_tog) | uep_t_res_stall;// set ep4 in stall 
                  break;
                #endif
                #ifdef ep4_out_callback
                case 0x04:
                  uep4_ctrl = uep4_ctrl & (~buep_r_tog) | uep_r_res_stall;// set ep4 out stall 
                  break;
                #endif
                #ifdef ep3_in_callback
                case 0x83:
                  uep3_ctrl = uep3_ctrl & (~buep_t_tog) | uep_t_res_stall;// set ep3 in stall 
                  break;
                #endif
                #ifdef ep3_out_callback
                case 0x03:
                  uep3_ctrl = uep3_ctrl & (~buep_r_tog) | uep_r_res_stall;// set ep3 out stall 
                  break;
                #endif
                #ifdef ep2_in_callback
                case 0x82:
                  uep2_ctrl = uep2_ctrl & (~buep_t_tog) | uep_t_res_stall;// set ep2 in stall 
                  break;
                #endif
                #ifdef ep2_out_callback
                case 0x02:
                  uep2_ctrl = uep2_ctrl & (~buep_r_tog) | uep_r_res_stall;// set ep2 out stall 
                  break;
                #endif
                #ifdef ep1_in_callback
                case 0x81:
                  uep1_ctrl = uep1_ctrl & (~buep_t_tog) | uep_t_res_stall;// set ep1 in stall 
                  break;
                #endif
                #ifdef ep1_out_callback
                case 0x01:
                  uep1_ctrl = uep1_ctrl & (~buep_r_tog) | uep_r_res_stall;// set ep1 out stall
                  break;
                #endif
                default:
                  len = 0xff;               // failed
                  break;
              }
            }
            else len = 0xff;                // failed
          }
          else len = 0xff;                  // failed
          break;

        case usb_get_status:
          ep0_buffer[0] = 0x00;
          ep0_buffer[1] = 0x00;
          if(setuplen >= 2) len = 2;
          else len = setuplen;
          break;

        default:
          len = 0xff;                       // failed
          break;
      }
    }
  }
  else len = 0xff;                          // wrong packet length

  if(len == 0xff) {
    setupreq = 0xff;
    uep0_ctrl = buep_r_tog | buep_t_tog | uep_r_res_stall | uep_t_res_stall;//stall
  }
  else if(len <= ep0_size) {      // tx data to host or send 0-length packet
    uep0_t_len = len;
    uep0_ctrl = buep_r_tog | buep_t_tog | uep_r_res_ack | uep_t_res_ack;// expect data1, answer ack
  }
  else {
    uep0_t_len = 0;  // tx data to host or send 0-length packet
    uep0_ctrl = buep_r_tog | buep_t_tog | uep_r_res_ack | uep_t_res_ack;// expect data1, answer ack
  }
}

void usb_ep0_in(void) {
  uint8_t len;
  switch(setupreq) {

    case usb_get_descriptor:
      len = setuplen >= ep0_size ? ep0_size : setuplen;
      usb_ep0_copydescr(len);                     // copy descriptor to ep0                                
      setuplen  -= len;
      pdescr    += len;
      uep0_t_len = len;
      uep0_ctrl ^= buep_t_tog;                    // switch between data0 and data1
      break;

    case usb_set_address:
      usb_dev_ad = usb_dev_ad & buda_gp_bit | setuplen;
      uep0_ctrl  = uep_r_res_ack | uep_t_res_nak;
      break;

    default:
      uep0_t_len = 0;                             // end of transaction
      uep0_ctrl  = uep_r_res_ack | uep_t_res_nak;
      break;
  }
}

void usb_ep0_out(void) {
  uep0_t_len = 0;
  uep0_ctrl |= uep_r_res_ack | uep_t_res_nak;     // respond nak
}

// ===================================================================================
// usb interrupt service routine
// ===================================================================================

#pragma save
#pragma nooverlay
void usb_interrupt(void) {   // inline not really working in multiple files in sdcc
  if(uif_transfer) {
    // dispatch to service functions
    uint8_t callindex = usb_int_st & mask_uis_endp;
    switch (usb_int_st & mask_uis_token) {
      case uis_token_out:
        switch (callindex) {
          case 0: ep0_out_callback(); break;
          #ifdef ep1_out_callback
          case 1: ep1_out_callback(); break;
          #endif
          #ifdef ep2_out_callback
          case 2: ep2_out_callback(); break;
          #endif
          #ifdef ep3_out_callback
          case 3: ep3_out_callback(); break;
          #endif
          #ifdef ep4_out_callback
          case 4: ep4_out_callback(); break;
          #endif
          default: break;
        }
        break;
      case uis_token_sof:
        switch (callindex) {
          #ifdef ep0_sof_callback
          case 0: ep0_sof_callback(); break;
          #endif
          #ifdef ep1_sof_callback
          case 1: ep1_sof_callback(); break;
          #endif
          #ifdef ep2_sof_callback
          case 2: ep2_sof_callback(); break;
          #endif
          #ifdef ep3_sof_callback
          case 3: ep3_sof_callback(); break;
          #endif
          #ifdef ep4_sof_callback
          case 4: ep4_sof_callback(); break;
          #endif
          default: break;
        }
        break;
      case uis_token_in:
        switch (callindex) {
          case 0: ep0_in_callback(); break;
          #ifdef ep1_in_callback
          case 1: ep1_in_callback(); break;
          #endif
          #ifdef ep2_in_callback
          case 2: ep2_in_callback(); break;
          #endif
          #ifdef ep3_in_callback
          case 3: ep3_in_callback(); break;
          #endif
          #ifdef ep4_in_callback
          case 4: ep4_in_callback(); break;
          #endif
          default: break;
        }
        break;
      case uis_token_setup:
        switch (callindex) {
          case 0: ep0_setup_callback(); break;
          #ifdef ep1_setup_callback
          case 1: ep1_setup_callback(); break;
          #endif
          #ifdef ep2_setup_callback
          case 2: ep2_setup_callback(); break;
          #endif
          #ifdef ep3_setup_callback
          case 3: ep3_setup_callback(); break;
          #endif
          #ifdef ep4_setup_callback
          case 4: ep4_setup_callback(); break;
          #endif
          default: break;
        }
        break;
    }
    uif_transfer = 0;                       // clear interrupt flag
  }
    
  // device mode usb bus reset
  if(uif_bus_rst) {
    uep0_ctrl = uep_r_res_ack | uep_t_res_nak;

    #ifdef usb_reset_handler
    usb_reset_handler();
    #endif

    usb_dev_ad   = 0x00;
    uif_suspend  = 0;
    uif_transfer = 0;
    uif_bus_rst  = 0;                       // clear interrupt flag
  }
    
  // usb bus suspend / wake up
  if (uif_suspend) {
    uif_suspend = 0;
    if ( !(usb_mis_st & bums_suspend) ) usb_int_fg = 0xff;  // clear interrupt flag
  }
}
#pragma restore

// ===================================================================================
// usb init function
// ===================================================================================

void usb_init(void) {
  usb_ctrl    = buc_dev_pu_en               // usb internal pull-up enable
              | buc_int_busy                // return nak if usb int flag not clear
              | buc_dma_en;                 // dma enable
  udev_ctrl   = bud_pd_dis                  // disable udp/udm pulldown resistor
              | bud_port_en;                // enable port, full-speed

  #if f_cpu < 12000000                      // set low-speed mode if sysfreq < 12 mhz
  usb_ctrl   |= buc_low_speed;
  udev_ctrl  |= bud_low_speed;
  #endif

  uep0_dma    = ep0_addr;                   // ep0 data transfer address
  uep0_ctrl   = uep_r_res_ack               // ep0 manual flip, out transaction returns ack
              | uep_t_res_nak;              // ep0 in transaction returns nak

  #ifdef usb_init_handler
  usb_init_handler();
  #endif

  usb_int_en |= buie_suspend                // enable device hang interrupt
              | buie_transfer               // enable usb transfer completion interrupt
              | buie_bus_rst;               // enable device mode usb bus reset interrupt

  usb_int_fg |= 0x1f;                       // clear interrupt flag
  ie_usb      = 1;                          // enable usb interrupt
  ea          = 1;                          // enable global interrupts

  uep0_t_len  = 0;                          // must be zero at start
}

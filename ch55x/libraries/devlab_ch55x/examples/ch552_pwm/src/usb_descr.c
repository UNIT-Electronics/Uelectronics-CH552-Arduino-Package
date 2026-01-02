// ===================================================================================
// usb descriptors
// ===================================================================================

#include "config.h"
#include "usb_descr.h"

// ===================================================================================
// device descriptor
// ===================================================================================
__code usb_dev_descr devdescr = {
  .blength            = sizeof(devdescr),       // size of the descriptor in bytes: 18
  .bdescriptortype    = usb_descr_typ_device,   // device descriptor: 0x01
  .bcdusb             = 0x0110,                 // usb specification: usb 1.1
  .bdeviceclass       = 0,                      // interface will define class
  .bdevicesubclass    = 0,                      // unused
  .bdeviceprotocol    = 0,                      // unused
  .bmaxpacketsize0    = ep0_size,               // maximum packet size for endpoint 0
  .idvendor           = usb_vendor_id,          // vid
  .idproduct          = usb_product_id,         // pid
  .bcddevice          = usb_device_version,     // device version
  .imanufacturer      = 1,                      // index of manufacturer string descr
  .iproduct           = 2,                      // index of product string descriptor
  .iserialnumber      = 3,                      // index of serial number string descr
  .bnumconfigurations = 1                       // number of possible configurations
};

// ===================================================================================
// configuration descriptor
// ===================================================================================
__code usb_cfg_descr_cdc cfgdescr = {

  // configuration descriptor
  .config = {
    .blength            = sizeof(usb_cfg_descr),  // size of the descriptor in bytes
    .bdescriptortype    = usb_descr_typ_config,   // configuration descriptor: 0x02
    .wtotallength       = sizeof(cfgdescr),       // total length in bytes
    .bnuminterfaces     = 2,                      // number of interfaces: 2
    .bconfigurationvalue= 1,                      // value to select this configuration
    .iconfiguration     = 0,                      // no configuration string descriptor
    .bmattributes       = 0x80,                   // attributes = bus powered, no wakeup
    .maxpower           = usb_max_power_ma / 2    // in 2ma units
  },

  // interface association descriptor
  .association = {
    .blength            = sizeof(usb_iad_descr),  // size of the descriptor in bytes
    .bdescriptortype    = usb_descr_typ_iad,      // interf association descr: 0x0b
    .bfirstinterface    = 0,                      // first interface
    .binterfacecount    = 2,                      // total number of interfaces
    .bfunctionclass     = usb_dev_class_comm,     // function class: cdc (0x02)
    .bfunctionsubclass  = 2,                      // 2: abstract control model (acm)
    .bfunctionprotocol  = 1,                      // 1: at command protocol
    .ifunction          = 4                       // index of string descriptor
  },

  // interface descriptor: interface 0 (cdc)
  .interface0 = {
    .blength            = sizeof(usb_itf_descr),  // size of the descriptor in bytes: 9
    .bdescriptortype    = usb_descr_typ_interf,   // interface descriptor: 0x04
    .binterfacenumber   = 0,                      // number of this interface: 0
    .balternatesetting  = 0,                      // value used to select alternative setting
    .bnumendpoints      = 1,                      // number of endpoints used: 1
    .binterfaceclass    = usb_dev_class_comm,     // interface class: cdc (0x02)
    .binterfacesubclass = 2,                      // 2: abstract control model (acm)
    .binterfaceprotocol = 1,                      // 1: at command protocol
    .iinterface         = 4                       // index of string descriptor
  },

  // functional descriptors for interface 0
  .functional = {
    0x05,0x24,0x00,0x10,0x01,                     // header functional descriptor
    0x05,0x24,0x01,0x00,0x00,                     // call management functional descriptor
    0x04,0x24,0x02,0x02,                          // direct line management functional descriptor
    0x05,0x24,0x06,0x00,0x01                      // union functional descriptor: cdc if0, data if1
  },

  // endpoint descriptor: endpoint 1 (cdc upload, interrupt)
  .ep1in = {
    .blength            = sizeof(usb_endp_descr), // size of the descriptor in bytes: 7
    .bdescriptortype    = usb_descr_typ_endp,     // endpoint descriptor: 0x05
    .bendpointaddress   = usb_endp_addr_ep1_in,   // endpoint: 1, direction: in (0x81)
    .bmattributes       = usb_endp_type_inter,    // transfer type: interrupt (0x03)
    .wmaxpacketsize     = ep1_size,               // max packet size
    .binterval          = 1                       // polling intervall in ms
  },

  // interface descriptor: interface 1 (data)
  .interface1 = {
    .blength            = sizeof(usb_itf_descr),  // size of the descriptor in bytes: 9
    .bdescriptortype    = usb_descr_typ_interf,   // interface descriptor: 0x04
    .binterfacenumber   = 1,                      // number of this interface: 1
    .balternatesetting  = 0,                      // value used to select alternative setting
    .bnumendpoints      = 2,                      // number of endpoints used: 2
    .binterfaceclass    = usb_dev_class_data,     // interface class: data (0x0a)
    .binterfacesubclass = 0,                      // interface sub class
    .binterfaceprotocol = 0,                      // interface protocol
    .iinterface         = 4                       // index of string descriptor
  },

  // endpoint descriptor: endpoint 2 (out)
  .ep2out = {
    .blength            = sizeof(usb_endp_descr), // size of the descriptor in bytes: 7
    .bdescriptortype    = usb_descr_typ_endp,     // endpoint descriptor: 0x05
    .bendpointaddress   = usb_endp_addr_ep2_out,  // endpoint: 2, direction: out (0x02)
    .bmattributes       = usb_endp_type_bulk,     // transfer type: bulk (0x02)
    .wmaxpacketsize     = ep2_size,               // max packet size
    .binterval          = 0                       // polling intervall (ignored for bulk)
  },

  // endpoint descriptor: endpoint 2 (in)
  .ep2in = {
    .blength            = sizeof(usb_endp_descr), // size of the descriptor in bytes: 7
    .bdescriptortype    = usb_descr_typ_endp,     // endpoint descriptor: 0x05
    .bendpointaddress   = usb_endp_addr_ep2_in,   // endpoint: 2, direction: in (0x82)
    .bmattributes       = usb_endp_type_bulk,     // transfer type: bulk (0x02)
    .wmaxpacketsize     = ep2_size,               // max packet size
    .binterval          = 0                       // polling intervall (ignored for bulk)
  }
};

// ===================================================================================
// string descriptors
// ===================================================================================

// language descriptor (index 0)
__code uint16_t langdescr[] = {
  ((uint16_t)usb_descr_typ_string << 8) | sizeof(langdescr), 0x0409 };  // us english

// manufacturer string descriptor (index 1)
__code uint16_t manufdescr[] = {
  ((uint16_t)usb_descr_typ_string << 8) | sizeof(manufdescr), manufacturer_str };

// product string descriptor (index 2)
__code uint16_t proddescr[] = {
  ((uint16_t)usb_descr_typ_string << 8) | sizeof(proddescr), product_str };

// serial string descriptor (index 3)
__code uint16_t serdescr[] = {
  ((uint16_t)usb_descr_typ_string << 8) | sizeof(serdescr), serial_str };

// interface string descriptor (index 4)
__code uint16_t interfdescr[] = {
  ((uint16_t)usb_descr_typ_string << 8) | sizeof(interfdescr), interface_str };

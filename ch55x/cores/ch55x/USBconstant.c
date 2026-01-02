/*
 created by deqing sun for use with ch55xduino
 */

#ifndef user_usb_ram

#include "USBconstant.h"

// device descriptor
__code usb_descriptor_device_t devicedescriptor = {
    .header = {.size = sizeof(usb_descriptor_device_t), .type = dtype_device},

    .usbspecification = version_bcd(1, 1, 0),
    .class = 0xef, // miscellaneous
    .subclass = 0x02,
    .protocol = 0x01, // interface association descriptor

    .endpoint0size = default_endp0_size,

    .vendorid = 0x1209,
    .productid = 0xc550,
    .releasenumber = version_bcd(1, 0, 1),

    .manufacturerstrindex = 1,
    .productstrindex = 2,
    .serialnumstrindex = 3,

    .numberofconfigurations = 1};

/** configuration descriptor structure. this descriptor, located in flash
 * memory, describes the usage of the device in one of its supported
 * configurations, including information about any device interfaces and
 * endpoints. the descriptor is read out by the usb host during the enumeration
 * process when selecting a configuration so that the host may correctly
 * communicate with the usb device.
 */
__code usb_descriptor_configuration_t configurationdescriptor = {
    .config = {.header = {.size = sizeof(usb_descriptor_configuration_header_t),
                          .type = dtype_configuration},

               .totalconfigurationsize = sizeof(usb_descriptor_configuration_t),
               .totalinterfaces = 2,

               .configurationnumber = 1,
               .configurationstrindex = no_descriptor,

               .configattributes = (usb_config_attr_reserved),

               .maxpowerconsumption = usb_config_power_ma(200)},

    .cdc_iad = {.header = {.size =
                               sizeof(usb_descriptor_interface_association_t),
                           .type = dtype_interfaceassociation},

                .firstinterfaceindex = interface_id_cdc_cci,
                .totalinterfaces = 2,

                .class = cdc_cscp_cdcclass,
                .subclass = cdc_cscp_acmsubclass,
                .protocol = cdc_cscp_atcommandprotocol,

                .iadstrindex = 4},

    .cdc_cci_interface = {.header = {.size = sizeof(usb_descriptor_interface_t),
                                     .type = dtype_interface},

                          .interfacenumber = interface_id_cdc_cci,
                          .alternatesetting = 0,

                          .totalendpoints = 1,

                          .class = cdc_cscp_cdcclass,
                          .subclass = cdc_cscp_acmsubclass,
                          .protocol = cdc_cscp_atcommandprotocol,

                          .interfacestrindex = 4},
    // refer to usbcdc11.pdf
    .cdc_functional_header =
        {
            .header = {.size = sizeof(usb_cdc_descriptor_functionalheader_t),
                       .type = cdc_dtype_csinterface},
            .subtype = cdc_dsubtype_csinterface_header,

            .cdcspecification = version_bcd(1, 1, 0),
        },
    // todo: check cdc_dsubtype_csinterface_callmanagement difference?
    .cdc_functional_acm =
        {
            .header = {.size = sizeof(usb_cdc_descriptor_functionalacm_t),
                       .type = cdc_dtype_csinterface},
            .subtype = cdc_dsubtype_csinterface_acm,

            .capabilities = 0x02, // no send_break, yes  set_line_coding,
                                  // set_control_line_state, get_line_coding,
                                  // and the notification serial_state.
        },

    .cdc_functional_union =
        {
            .header = {.size = sizeof(usb_cdc_descriptor_functionalunion_t),
                       .type = cdc_dtype_csinterface},
            .subtype = cdc_dsubtype_csinterface_union,

            .masterinterfacenumber = interface_id_cdc_cci,
            .slaveinterfacenumber = interface_id_cdc_dci,
        },

    .cdc_notificationendpoint =
        {.header = {.size = sizeof(usb_descriptor_endpoint_t),
                    .type = dtype_endpoint},

         .endpointaddress = cdc_notification_epaddr,
         .attributes =
             (ep_type_interrupt | endpoint_attr_no_sync | endpoint_usage_data),
         .endpointsize = cdc_notification_epsize,
         .pollingintervalms = 0x40},

    .cdc_dci_interface = {.header = {.size = sizeof(usb_descriptor_interface_t),
                                     .type = dtype_interface},

                          .interfacenumber = interface_id_cdc_dci,
                          .alternatesetting = 0,

                          .totalendpoints = 2,

                          .class = cdc_cscp_cdcdataclass,
                          .subclass = cdc_cscp_nodatasubclass,
                          .protocol = cdc_cscp_nodataprotocol,

                          .interfacestrindex = 4},

    .cdc_dataoutendpoint = {.header = {.size =
                                           sizeof(usb_descriptor_endpoint_t),
                                       .type = dtype_endpoint},

                            .endpointaddress = cdc_rx_epaddr,
                            .attributes =
                                (ep_type_bulk | endpoint_attr_no_sync |
                                 endpoint_usage_data),
                            .endpointsize = cdc_txrx_epsize,
                            .pollingintervalms = 0x00},

    .cdc_datainendpoint = {.header = {.size = sizeof(usb_descriptor_endpoint_t),
                                      .type = dtype_endpoint},

                           .endpointaddress = cdc_tx_epaddr,
                           .attributes = (ep_type_bulk | endpoint_attr_no_sync |
                                          endpoint_usage_data),
                           .endpointsize = cdc_txrx_epsize,
                           .pollingintervalms = 0x00}};

// string descriptors
__code uint8_t languagedescriptor[] = {0x04, 0x03, 0x09,
                                       0x04}; // language descriptor
__code uint16_t serialdescriptor[] = {
    // serial string descriptor
    (((5 + 1) * 2) | (dtype_string << 8)), 'c', 'h', '5', '5', 'x',
};
__code uint16_t productdescriptor[] = {
    // produce string descriptor
    (((10 + 1) * 2) | (dtype_string << 8)),
    'c',
    'h',
    '5',
    '5',
    'x',
    'd',
    'u',
    'i',
    'n',
    'o',
};

__code uint16_t cdcdescriptor[] = {
    (((10 + 1) * 2) | (dtype_string << 8)),
    'c',
    'd',
    'c',
    ' ',
    's',
    'e',
    'r',
    'i',
    'a',
    'l',
};

__code uint16_t manufacturerdescriptor[] = {
    // sdcc is little endian
    (((6 + 1) * 2) | (dtype_string << 8)), 'd', 'e', 'q', 'i', 'n', 'g',
};

#endif

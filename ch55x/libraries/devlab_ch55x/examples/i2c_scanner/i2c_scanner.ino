// SPDX-FileCopyrightText: 2026 Unit Electronics
//
// SPDX-License-Identifier: MIT
// --------------------------------------
// I2C Scanner for CH552
//
// Scans I2C bus for connected devices
// Modified from Arduino I2C Scanner
// --------------------------------------

// Uncomment to use custom I2C pins (default: SDA=P1.6/16, SCL=P1.7/15)
// #define PIN_SDA 16
// #define PIN_SCL 15

#include <devlab_ch55x.h>

void setup() {
  I2C_init();
  USBSerial_println_s("");
  USBSerial_println_s("I2C Scanner");
}

void loop() {
  uint8_t address;
  int nDevices;

  USBSerial_println_s("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    // Probe device - returns 1 if ACK received
    if (I2C_probe(address))
    {
      USBSerial_print_s("I2C device found at address 0x");
      if (address < 16)
        USBSerial_print_s("0");
      
      // Print hex
      uint8_t h = (address >> 4) & 0x0F;
      uint8_t l = address & 0x0F;
      
      USBSerial_print_c((h < 10) ? ('0' + h) : ('A' + h - 10));
      USBSerial_print_c((l < 10) ? ('0' + l) : ('A' + l - 10));
      USBSerial_println_s("  !");
      
      nDevices++;
    }
    delay(1);
  }
  
  if (nDevices == 0)
    USBSerial_println_s("No I2C devices found\n");
  else
    USBSerial_println_s("done\n");

  delay(5000);
}

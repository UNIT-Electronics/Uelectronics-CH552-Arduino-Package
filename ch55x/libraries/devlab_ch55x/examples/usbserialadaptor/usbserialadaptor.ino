/*
  usbserialadaptor

  a simple example turns ch552 to a serial adaptor, 
  relaying data between usb serial and serial0.

  baudrate of serial0 is determined by usb serial's rate.

  created 2020
  by deqing sun for use with ch55xduino

  this example code is in the public domain.

*/

extern __xdata uint8_t linecoding[]; //linecoding of cdc is located in this array
__xdata uint32_t oldbaudrate = 9600;

void setup() {
  serial0_begin(9600);
}

void loop() {
  if (usbserial_available()) {
    char serialchar = usbserial_read();
    serial0_write(serialchar);
  }
  if (serial0_available()) {
    char serialchar = serial0_read();
    usbserial_write(serialchar);
  }

  __xdata uint32_t currentbaudrate = *((__xdata uint32_t *)linecoding); //both linecoding and sdcc are little-endian

  if (oldbaudrate != currentbaudrate) {
    oldbaudrate = currentbaudrate;
    serial0_begin(currentbaudrate);
  }

}

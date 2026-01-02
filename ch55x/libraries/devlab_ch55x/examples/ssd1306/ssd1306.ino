// ===================================================================================
// arduino ide wrapper for ch55xduino
// ===================================================================================
//
// compilation instructions for the arduino ide:
// ---------------------------------------------
// - make sure you have installed ch55xduino: https://github.com/deqingsun/ch55xduino
// - copy the .ino and .c files as well as the /src folder together into one folder
//   and name it like the .ino file. open the .ino file in the arduino ide. go to 
//   "tools -> board -> ch55x boards -> ch552 board". under "tools" select the 
//   following board options:
//   - clock source:  16 mhz (internal)
//   - upload method: usb
//   - usb settings:  user code /w 266b usb ram
// - press boot button on the board and keep it pressed while connecting it via usb
//   with your pc.
// - click on "upload" immediatly afterwards.
// - to compile the firmware using the makefile, follow the instructions in the 
//   .c file.


unsigned char _sdcc_external_startup (void) __nonbanked {
  return 0;
}

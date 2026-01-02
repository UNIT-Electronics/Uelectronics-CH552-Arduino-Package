// sdcc doesn't support weak attribute. but function in sketch can override
// function in library. arduino compiles core as an archive and linked with the
// sketch

#ifdef user_usb_ram
void usbinterrupt(void) {
  // weak
}
#endif

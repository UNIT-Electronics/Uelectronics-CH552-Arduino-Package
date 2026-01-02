// sdcc doesn't support weak attribute. but function in sketch can override
// function in library. arduino compiles core as an archive and linked with the
// sketch
void gpiointerrupt(void) __interrupt {
  // weak
}

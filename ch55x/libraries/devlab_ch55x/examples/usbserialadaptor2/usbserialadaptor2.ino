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
void blinkpatterna();

void blinkpatternb();

void blinkpatternc();
char str[50];
#define led_builtin 33

void setup() {
  serial0_begin(9600);
  pinmode(led_builtin, output);

}

void loop() {
  if (usbserial_available()) {
    char serialchar = usbserial_read();

    serial0_write(serialchar);
    if (serialchar == 'a') {
      blinkpatterna();
    } else if (serialchar == 'b') {
      blinkpatternb();
    } else if (serialchar == 'c') {
      blinkpatternc();
    }
      
                        // wait for a second
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


void blinkpatterna() {

  const char *helloch552 = "respondo desde el ch552";
  
  for (int i = 0; i < strlen(helloch552); i++) {
    char serialchar = helloch552[i];
    digitalwrite(led_builtin, high);
    delay(20);
    digitalwrite(led_builtin, low);
    delay(20);
    
    usbserial_write(serialchar);
  }
  
}

void blinkpatternb() {
  digitalwrite(led_builtin, high);
  delay(100);
  digitalwrite(led_builtin, low);
  delay(100);
  digitalwrite(led_builtin, high);
  delay(100);
  digitalwrite(led_builtin, low);
  delay(100);
}

void blinkpatternc() {
   const char *helloworld = "hello world";
  
  for (int i = 0; i < strlen(helloworld); i++) {
    char serialchar = helloworld[i];
    digitalwrite(led_builtin, high);
    delay(200);
    digitalwrite(led_builtin, low);
    delay(200);
    
    serial0_write(serialchar);
  }
}

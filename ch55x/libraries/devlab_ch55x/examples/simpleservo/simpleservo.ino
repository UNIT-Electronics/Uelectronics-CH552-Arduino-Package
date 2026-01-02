#include <servo.h>

void setup() {
  servo_init();
  pinmode(12, output);
  servo_attach(12);
  pinmode(13, output);
  servo_attach(13);
}

void loop() {
  servo_writemicroseconds(12, 1000);
  servo_write(13,0);
  delay(3000);
  servo_writemicroseconds(12, 1500);
  servo_write(13,90);
  delay(3000);
  servo_writemicroseconds(12, 2000);
  servo_write(13,180);
  delay(3000);
}

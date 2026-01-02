#include <serial.h>

void setup() {
  // no need to init usbserial

  // configurar el pin 2 como entrada y habilitar la resistencia pull-up interna
  pinmode(11, input_pullup);
  pinmode(33, output);
}

void loop() {
  // leer el valor del botón en una variable
  int sensorval = digitalread(11);
  // imprimir el valor del botón en el monitor serial
  usbserial_println(sensorval);

  // dado que el pull-up significa que la lógica del botón está invertida,
  // encender el led cuando el botón está presionado y apagarlo cuando está abierto:
  if (sensorval == high) {
    digitalwrite(33, low);
  } else {
    digitalwrite(33, high);
  }

  delay(10);
}
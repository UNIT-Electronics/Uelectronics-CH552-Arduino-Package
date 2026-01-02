// ===================================================================================
// GPIO Functions for CH551, CH552 and CH554
// ===================================================================================

#pragma once
#include <Arduino.h>

// Pin manipulation macros
#define PIN_output(pin)     pinMode(pin, OUTPUT)
#define PIN_output_OD(pin)  pinMode(pin, OUTPUT); PIN_high(pin)
#define PIN_input(pin)      pinMode(pin, INPUT)
#define PIN_input_PU(pin)   pinMode(pin, INPUT_PULLUP)
#define PIN_high(pin)       digitalWrite(pin, HIGH)
#define PIN_low(pin)        digitalWrite(pin, LOW)
#define PIN_read(pin)       digitalRead(pin)
#define PIN_toggle(pin)     digitalWrite(pin, !digitalRead(pin))

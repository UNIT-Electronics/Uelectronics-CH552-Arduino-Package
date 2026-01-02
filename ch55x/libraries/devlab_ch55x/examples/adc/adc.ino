/*
  ADC - Analog to Digital Converter Example

  Reads an analog input on analog pin P1.1 and prints the value to USB Serial.
  Also turns an LED on when the value is above a threshold.

  The CH552 has 4 ADC channels available on pins: P1.1, P1.4, P1.5, and P3.2

  Circuit:
  - Potentiometer:
    Center pin to analog input P1.1
    One side pin to ground
    Other side pin to +5V
  - LED:
    Anode (long leg) to P3.3 (LED_BUILTIN)
    Cathode (short leg) to ground

  This example code is in the public domain.

  Modified 13 Jun 2020 by DeQing Sun for use with CH55xduino
  Modified 02 Jan 2026 by Unit Electronics for Devlab CH55x
*/

#define LED_BUILTIN 34
#define SENSOR_PIN 11     // Analog pin P1.1
#define THRESHOLD 512     // Threshold value (0-1023)

int sensorValue = 0;

void setup() {
  // Initialize LED pin as output
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Initialize sensor pin as input (disable pull-up for full analog range)
  pinMode(SENSOR_PIN, INPUT);
  
  USBSerial_println_s("ADC Example Started");
  USBSerial_println_s("Reading analog value from P1.1");
  USBSerial_println_s("Format: ADC Value | Voltage | LED State");
  USBSerial_println_s("------------------------------------");
}

void loop() {
  // Read the analog value (0-1023)
  sensorValue = analogRead(SENSOR_PIN);
  
  // Calculate voltage (5V reference)
  float voltage = sensorValue * (5.0 / 1023.0);
  
  // Print ADC value
  USBSerial_print_s("ADC: ");
  USBSerial_print_u(sensorValue);
  
  // Print voltage (using integer arithmetic to avoid float overhead)
  USBSerial_print_s(" | Voltage: ");
  USBSerial_print_u((uint16_t)(voltage * 100)); // Print as centivoltage
  USBSerial_print_s(" cV");
  
  // Control LED based on threshold
  if (sensorValue > THRESHOLD) {
    digitalWrite(LED_BUILTIN, HIGH);
    USBSerial_println_s(" | LED: ON");
  } else {
    digitalWrite(LED_BUILTIN, LOW);
    USBSerial_println_s(" | LED: OFF");
  }
  
  delay(500);  // Update every 500ms
}

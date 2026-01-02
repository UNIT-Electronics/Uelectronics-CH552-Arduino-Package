// ===================================================================================
// i2c functions for ch551, ch552 and ch554                                   * v1.1 *
// ===================================================================================
//
// simple i2c bitbanging for 400khz slave devices. for system clock < 12mhz the 
// i2c clock frequency is slower. ack bit of the slave is ignored. clock stretching 
// by the slave is not allowed.
//
// pin_sda and pin_scl must be defined in config.h:
// pin_sda - pin connected to serial data of the i2c bus
// pin_scl - pin connected to serial clock of the i2c bus
// external pull-up resistors (4k7 - 10k) are mandatory!
//
// further information:     https://github.com/wagiminator/attiny13-tinyoleddemo
// 2022 by stefan wagner:   https://github.com/wagiminator

#include "i2c.h"
#include "gpio.h"
#include "../config.h"

// i2c delay (for 400khz devices -> scl low: min 1300us, scl high: min 600us)
// the exact number of clock cycles required for jumps and thus also loops cannot 
// be precisely predicted. however, this can be accepted for this type of 
// application (synchronous data transmission).
#if f_cpu >= 24000000                                       // ~500khz i2c clock
  #define i2c_delay_h() __asm__("sjmp .+2");safe_mod++      // delay 6-7 clock cycles
  #define i2c_delay_l() __asm__("sjmp .+2");safe_mod++      // delay 6-7 clock cycles
#elif f_cpu >= 16000000                                     // ~500khz i2c clock
  #define i2c_delay_h() __asm__("sjmp .+2")                 // delay 4-5 clock cycles
  #define i2c_delay_l()                                     // no delay
#elif f_cpu >= 12000000                                     // ~360khz i2c clock
  #define i2c_delay_h() __asm__("orl _safe_mod, #0x00")     // delay 3 clock cycles
  #define i2c_delay_l()                                     // no delay
#elif f_cpu >= 6000000                                      // ~200khz i2c clock
  #define i2c_delay_h() __asm__("nop")                      // delay 1 clock cycle
  #define i2c_delay_l()                                     // no delay
#else                                                       // ~100khz i2c clock
  #define i2c_delay_h()                                     // no delay
  #define i2c_delay_l()                                     // no delay
#endif

// check pin defines
#ifndef pin_sda
  #error pin_sda is undefinded
#endif
#ifndef pin_scl
  #error pin_scl is undefined
#endif

// i2c macros
#define i2c_sda_high()  pin_high(pin_sda) // release sda -> pulled high by resistor
#define i2c_sda_low()   pin_low(pin_sda)  // sda low     -> pulled low  by mcu
#define i2c_scl_high()  pin_high(pin_scl) // release scl -> pulled high by resistor
#define i2c_scl_low()   pin_low(pin_scl)  // scl low     -> pulled low  by mcu
#define i2c_sda_read()  pin_read(pin_sda) // read sda pin
#define i2c_clockout()  i2c_delay_l();i2c_scl_high();i2c_delay_h();i2c_delay_h();i2c_scl_low()

// i2c init function
void i2c_init(void) {
  pin_output_od(pin_sda);                   // set sda pin to open-drain output
  pin_output_od(pin_scl);                   // set scl pin to open-drain output
}

// i2c transmit one data byte to the slave, ignore ack bit, no clock stretching allowed
void i2c_write(uint8_t data) {
  uint8_t i;
  for(i=8; i; i--, data<<=1) {              // transmit 8 bits, msb first
    (data & 0x80) ? (i2c_sda_high()) : (i2c_sda_low());  // sda high if bit is 1
    i2c_clockout();                         // clock out -> slave reads the bit
  }
  i2c_sda_high();                           // release sda for ack bit of slave
  i2c_delay_h();                            // delay
  i2c_delay_h();                            // delay
  i2c_clockout();                           // 9th clock pulse is for the ignored ack bit
}

// i2c start transmission
void i2c_start(uint8_t addr) {
  i2c_sda_low();                            // start condition: sda goes low first
  i2c_delay_h();                            // delay
  i2c_scl_low();                            // start condition: scl goes low second
  i2c_write(addr);                          // send slave address
}

// i2c restart transmission
void i2c_restart(uint8_t addr) {
  i2c_sda_high();                           // prepare sda for high to low transition
  i2c_delay_h();                            // delay
  i2c_scl_high();                           // restart condition: clock high
  i2c_start(addr);                          // start again
}

// i2c stop transmission
void i2c_stop(void) {
  i2c_sda_low();                            // prepare sda for low to high transition
  i2c_delay_h();                            // delay
  i2c_scl_high();                           // stop condition: scl goes high first
  i2c_delay_h();                            // delay
  i2c_sda_high();                           // stop condition: sda goes high second
}

// i2c receive one data byte from the slave (ack=0 for last byte, ack>0 if more bytes to follow)
uint8_t i2c_read(uint8_t ack) {
  uint8_t i;
  uint8_t data = 0;                         // variable for the received byte
  i2c_sda_high();                           // release sda -> will be toggled by slave
  for(i=8; i; i--) {                        // receive 8 bits
    data <<= 1;                             // bits shifted in right (msb first)
    i2c_delay_h();                          // delay
    i2c_delay_l();                          // delay
    i2c_scl_high();                         // clock high
    if(i2c_sda_read()) data |= 1;           // read bit
    i2c_scl_low();                          // clock low -> slave prepares next bit
  }
  if(ack) i2c_sda_low();                    // pull sda low to acknowledge (ack)
  i2c_delay_h();                            // delay
  i2c_delay_h();                            // delay
  i2c_clockout();                           // clock out -> slave reads ack bit
  return data;                              // return the received byte
}

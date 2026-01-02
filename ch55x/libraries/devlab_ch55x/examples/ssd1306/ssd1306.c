

// ===================================================================================
// libraries, definitions and macros
// ===================================================================================

// libraries
#include "src/system.h"                   // system functions
#include "src/gpio.h"                     // for gpio
#include "src/delay.h"                    // for delays
#include "src/oled.h"                     // for oled
#include "config.h"


// ===================================================================================
// buzzer function
// ===================================================================================

// create a short beep on the buzzer
void beep(void) {
  uint8_t i;
  for(i=255; i; i--) {
    pin_low(pin_buzzer);
    dly_us(125);
    pin_high(pin_buzzer);
    dly_us(125);
  }
}

// ===================================================================================
// main function
// ===================================================================================

void main(void) {
  // setup
  clk_config();                           // configure system clock
  dly_ms(5);                              // wait for clock to stabilize
  oled_init();                            // init oled

 // print start message
  oled_print("*  unitelectronics  *");
  oled_print("---------------------\n");
  oled_print("ready\n");

  beep();


  // loop
  while(1) {

  }
}

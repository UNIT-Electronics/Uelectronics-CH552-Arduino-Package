
#include "src/system.h" 
#include "src/gpio.h"  
#include "src/delay.h"  

#define pin_led p34

void main(void)
{
  clk_config();
  dly_ms(5);

  pin_output(pin_led);
  while (1)
  {
    // toggle led state every 500ms
    pin_toggle(pin_led);
    dly_ms(500);
  }
}
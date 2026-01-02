#include "src/system.h" 
#include "src/gpio.h"   
#include "src/delay.h"  

#define pin_adc p11

void main(void)
{
  clk_config();
  dly_ms(5);

  adc_input(pin_adc);

  adc_enable();

  while (1)
  {
    int data = adc_read(); // read adc value 0 - 255 (8-bit)
  }
}
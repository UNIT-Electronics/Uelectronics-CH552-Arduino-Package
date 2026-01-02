

#include <stdio.h>
#include "config.h"                   // user configurations
#include "src/system.h"                   // system functions
#include "src/gpio.h"                     // for gpio
#include "src/delay.h"                    // for delays


#define i2c_delay_h()

volatile uint8_t buffer = 0;
volatile uint8_t current_state = 0;
volatile uint8_t data = 0;
volatile uint8_t config = 0;
volatile uint8_t command =0;
volatile uint8_t count = 0;
volatile uint8_t flag = 0;
volatile unsigned int timeout_counter = 0;


#define pin_int p32
volatile uint16_t counter;


void ext0_interrupt(void) __interrupt(int_no_int0)
{
    pin_toggle(pin_led);  
}

int main(void)
{
    clk_config();  
    dly_ms(5);
    pin_output_od(pin_int);  
    pin_output(pin_led);
    
	ea  = 1;     /* enable global interrupt */
    ex0 = 1;    // enable int0
    it0 = 1;    // int0 is edge triggered

    while(1)
    {
        // do nothing
    }
    
}
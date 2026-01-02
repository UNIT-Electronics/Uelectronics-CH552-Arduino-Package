
#include <stdio.h>
#include "config.h"                   // user configurations
#include "src/system.h"                   // system functions
#include "src/gpio.h"                     // for gpio
#include "src/delay.h"                    // for delays
#include "src/pwm.h"


volatile uint16_t counter;



// once timer triggers this interrupt, we'll set the tl and th values, so 
// it will start to increment from there
// the formula used to calculate tl and th values in regards to the 16 bit `counter`:
// timer0/1 will _always_ increment tl/th every 12th clock cycle:
// max_val_of_counter+1 * ((0xffff-0xthtl) * (1/(freq_sys/12)) = seconds for the counter to overflow
// our `counter` is 16 bit, so the max value is 0xffff:
// (0xffff+1)*(0xffff-0xffe0)*(1/(24000000/12)) = 1.015 seconds

//     // we will update this `counter` on every interrupt

void timer0_interrupt(void) __interrupt(int_no_tmr0)	/* timer0 interrupt service routine (isr) */
{
    pin_toggle(pin_buzzer);
    th0 = 0xff;      	/* 50ms timer value */
    tl0 = 0x00;      
}

int main(void)
{
    clk_config();  
    dly_ms(5);
    pin_output(pin_buzzer);
	ea  = 1;         	/* enable global interrupt */
	et0 = 1;         	/* enable timer0 interrupt */
	
	th0 = 0xff;		/* 50ms timer value */
	tl0 = 0x00;      
    tmod = 0x01;		/* timer0 mode1 */
	tr0 = 1;  	      	/* start timer0 */
  	while(1);
}
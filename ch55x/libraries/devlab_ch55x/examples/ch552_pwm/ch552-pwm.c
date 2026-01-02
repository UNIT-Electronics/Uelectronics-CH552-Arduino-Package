/** @file ch552-pwm.c (source)
 *
 *  @brief ch552 pwm example
 *
 *  @author @cesarbautista 
 *
 *  @bug no known bugs.
 */

/******************************************************************************
 * includes
 *****************************************************************************/

#include <stdio.h>
#include "src/system.h"                   // system functions
#include "src/gpio.h"                     // for gpio
#include "src/delay.h"                    // for delays
#include "src/pwm.h"

/******************************************************************************
 * macros and defines
 *****************************************************************************/

#define min_counter 10
#define max_counter 254
#define step_size   10

/******************************************************************************
 * typedefs
 *****************************************************************************/
// define los pines pwm
#define pin_pwm1             p30       // pwm pin 0
#define pin_pwm2             p34       // pwm pin 4

// selecciona el pin pwm
#define select_pwm(pin)      (pin_pwm1)     // selecciona el pin que desees usar aquí (pin_pwm1 o pin_pwm2)
#define pin_pwm              select_pwm(pin_pwm1) // selecciona el pin pwm deseado

// ahora puedes cambiar entre pin_pwm1 y pin_pwm2 simplemente modificando select_pwm en tu definición de pin_pwm


/******************************************************************************
 * public function prototypes
 *****************************************************************************/

void change_pwm(int hex_value)
{
    pwm_write(pin_pwm, hex_value);
}
void main(void) 
{
    pwm_set_freq(1000);                    
    pin_output(pin_pwm);       
    pwm_start(pin_pwm);      
    pwm_write(pin_pwm, 0);
    clk_config();                          
    dly_ms(5);                            

while (1) 
{
    for (int i = min_counter; i < max_counter; i+=step_size) 
    {
        change_pwm(i);
        dly_ms(10);
    }
    for (int i = max_counter; i > min_counter; i-=step_size)
    {
        change_pwm(i);
        dly_ms(10);
    }
    
}
}

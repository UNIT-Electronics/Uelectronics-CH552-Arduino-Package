/*
 created by deqing sun for use with ch55xduino
 */

#define arduino_main
// clang-format off
#include "wiring_private.h"
#include "pins_arduino_include.h"
// clang-format on

#if defined(ch559)
uint16_t analogread(__data uint8_t pin)
#else
uint8_t analogread(__data uint8_t pin)
#endif
{
  pin = analogpintochannel(pin);

  if (pin == not_analog)
    return 0;

#if defined(ch551) || defined(ch552)
  adc_cfg = badc_en | badc_clk;

  adc_ctrl = (adc_ctrl & ~(0x03)) | (0x03 & pin);

  adc_start = 1;

  while (adc_start)
    ;

  return adc_data;
#elif defined(ch559)

  __data uint8_t pinmask = 1 << pin;
  p1_ie &= ~(pinmask); // close other data functions of p1 port, if only part of
                       // the sampling channel is used, set the rest to 1,
                       // otherwise it will affect the io function
  adc_setup |= badc_power_en; // adc power enable
  adc_ck_se =
      (f_cpu / 6000000l);      // set frequency division, make it similar to 6m
  adc_ctrl &= ~mask_adc_cycle; // select manual sampling
  adc_ctrl &= ~(badc_chann_mod1 | badc_chann_mod0); // manually select channel
  adc_chann = pinmask;                              // gate channel 1
  adc_ex_sw |= badc_resolution;                     // sampling bits 11bit
  // adc_ex_sw &= ~badc_resolution; //sampling bits 10bit
  delaymicroseconds(10);   // optional, wait for the channel to switch
                           // successfully
  adc_ctrl |= badc_sample; // manually generate sampling pulse
  delaymicroseconds(5);
  adc_ctrl &= ~badc_sample;
  while ((adc_stat & badc_if_act) == 0)
    ; // non-interrupt mode, waiting for the completion of the acquisition
  adc_stat |= badc_if_act;
  __data uint16_t adcvalue = adc_fifo;
  return adcvalue; // return sample value

#else
  return 0;
#endif
}

// right now, pwm output only works on the pins with
// hardware support.  these are defined in the appropriate
// pins_*.c file.  for the rest of the pins, we default
// to digital output.
void analogwrite(__data uint8_t pin, __xdata uint16_t val) {
  // we need to make sure the pwm output is enabled for those pins
  // that support it, as we turn it off when digitally reading or
  // writing with them.  also, make sure the pin is in output mode
  // for consistenty with wiring, which doesn't require a pinmode
  // call for the analog output pins.
#if defined(ch551) || defined(ch552)
  pinmode(pin, output);
  if (val == 0) {
    digitalwrite(pin, low);
  } else if (val >= 256) {
    digitalwrite(pin, high);
  } else {
    switch (digitalpintopwm(pin)) {
    case pin_pwm1:
      pin_func &= ~(bpwm1_pin_x);
      pwm_ctrl |= bpwm1_out_en;
      pwm_data1 = val;
      break;
    case pin_pwm2:
      pin_func &= ~(bpwm2_pin_x);
      pwm_ctrl |= bpwm2_out_en;
      pwm_data2 = val;
      break;
    case pin_pwm1_:
      pin_func |= (bpwm1_pin_x);
      pwm_ctrl |= bpwm1_out_en;
      pwm_data1 = val;
      break;
    case pin_pwm2_:
      pin_func |= (bpwm2_pin_x);
      pwm_ctrl |= bpwm2_out_en;
      pwm_data2 = val;
      break;
    case not_on_pwm:
    default:
      if (val < 128) {
        digitalwrite(pin, low);
      } else {
        digitalwrite(pin, high);
      }
    }
  }
#elif defined(ch559)
  pinmode(pin, output);
  if (val == 0) {
    digitalwrite(pin, low);
  } else if (val >= 256) {
    digitalwrite(pin, high);
  } else {
    uint8_t pwmpin = digitalpintopwm(pin);
    if (pwmpin != not_on_pwm) {
#if (f_cpu / (1000l * 255)) > 255
      pwm_ck_se = 255;
#else
      pwm_ck_se = (f_cpu / (1000l * 255));
#endif
      pwm_cycle = 255;
    }
    switch (pwmpin) {
    case pin_pwm1:
      pin_func &= ~(bpwm1_pin_x); // ch559 only has 1 bit for 2 pwms
      pwm_ctrl |= bpwm_out_en;
      pwm_data = val;
      break;
    case pin_pwm2:
      pin_func &= ~(bpwm1_pin_x);
      pwm_ctrl |= bpwm2_out_en;
      pwm_data2 = val;
      break;
    case pin_pwm3:
    case pin_pwm3_:
      if (pwmpin == pin_pwm3) {
        p1_dir |= bpwm3; // push pull
        p1_pu |= bpwm3;
        pin_func &= ~btmr3_pin_x;
      } else {
        p4_dir |= bpwm3_; // push pull
        p4_pu |= bpwm3_;
        pin_func |= btmr3_pin_x;
      }
      t3_ctrl |= bt3_clr_all;
      t3_ctrl &= ~bt3_clr_all;
      t3_setup |= bt3_en_ck_se;
      t3_ck_se_l = (f_cpu / (1000l * 255)) & 0xff;
      t3_ck_se_h = ((f_cpu / (1000l * 255)) >> 8) & 0xff;
      t3_setup &= ~bt3_en_ck_se;
      t3_ctrl |= bt3_out_en;
      t3_end_l = 0xff;
      t3_end_h = 0;
      t3_fifo_l = val;
      t3_fifo_h = 0;
      t3_ctrl |= bt3_cnt_en;
      break;
    case pin_pwm1_:
      pin_func |= (bpwm1_pin_x);
      pwm_ctrl |= bpwm_out_en;
      pwm_data = val;
      break;
    case pin_pwm2_:
      pin_func |= (bpwm1_pin_x);
      pwm_ctrl |= bpwm2_out_en;
      pwm_data2 = val;
      break;
    case not_on_pwm:
    default:
      if (val < 128) {
        digitalwrite(pin, low);
      } else {
        digitalwrite(pin, high);
      }
    }
  }
#else
  pin;
  val;
  return;
#endif
}

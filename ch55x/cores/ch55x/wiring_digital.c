/*
 created by deqing sun for use with ch55xduino
 */

#define arduino_main

// clang-format off
#include "wiring_private.h"
#include "pins_arduino_include.h"

#include "pins_arduino.h"   //only include once in core
// clang-format on

void pinmode(__data uint8_t pin,
             __xdata uint8_t mode) // only p1 & p3 can set mode
{
  //__data make sure the local varaibles reside in register in large model
  __data uint8_t bit = digitalpintobitmask(pin);
  __data uint8_t port = digitalpintoport(pin);

  if (port == not_a_pin)
    return;

  if (mode == input) {
#if defined(ch551) || defined(ch552) || defined(ch549)
    if (port == p1port) {
      p1_mod_oc &= ~bit;
      p1_dir_pu &= ~bit;
    } else if (port == p3port) {
      p3_mod_oc &= ~bit;
      p3_dir_pu &= ~bit;
    }
#endif
#if defined(ch549)
    else if (port == p0port) {
      p0_mod_oc &= ~bit;
      p0_dir_pu &= ~bit;
    } else if (port == p2port) {
      p2_mod_oc &= ~bit;
      p2_dir_pu &= ~bit;
    } else if (port == p4port) {
      p4_mod_oc &= ~bit;
      p4_dir_pu &= ~bit;
    }
#endif
#if defined(ch559)
    if (port == p0port) {
      port_cfg &= ~bp0_oc;
      p0_pu &= ~bit;
      p0_dir &= ~bit;
    } else if (port == p1port) {
      p1_ie |= bit;
      port_cfg &= ~bp1_oc;
      p1_pu &= ~bit;
      p1_dir &= ~bit;
    } else if (port == p2port) {
      port_cfg &= ~bp2_oc;
      p2_pu &= ~bit;
      p2_dir &= ~bit;
    } else if (port == p3port) {
      port_cfg &= ~bp3_oc;
      p3_pu &= ~bit;
      p3_dir &= ~bit;
    } else if (port == p4port) { // p4 can not do oc
      p4_pu &= ~bit;
      p4_dir &= ~bit;
    } // p5 can only do input
#endif
  } else if (mode == input_pullup) {
#if defined(ch551) || defined(ch552) || defined(ch549)
    if (port == p1port) {
      p1_mod_oc |= bit;
      p1_dir_pu |= bit;
    } else if (port == p3port) {
      p3_mod_oc |= bit;
      p3_dir_pu |= bit;
    }
#endif
#if defined(ch549)
    else if (port == p0port) {
      p0_mod_oc |= bit;
      p0_dir_pu |= bit;
    } else if (port == p2port) {
      p2_mod_oc |= bit;
      p2_dir_pu |= bit;
    } else if (port == p4port) {
      p4_mod_oc |= bit;
      p4_dir_pu |= bit;
    }
#endif
#if defined(ch559)
    if (port == p0port) {
      port_cfg &= ~bp0_oc;
      p0_pu |= bit;
      p0_dir &= ~bit;
    } else if (port == p1port) {
      p1_ie |= bit;
      port_cfg &= ~bp1_oc;
      p1_pu |= bit;
      p1_dir &= ~bit;
    } else if (port == p2port) {
      port_cfg &= ~bp2_oc;
      p2_pu |= bit;
      p2_dir &= ~bit;
    } else if (port == p3port) {
      port_cfg &= ~bp3_oc;
      p3_pu |= bit;
      p3_dir &= ~bit;
    } else if (port == p4port) { // p4 can not do oc
      p4_pu |= bit;
      p4_dir &= ~bit;
    } // p5 can only do input
#endif
  } else if (mode == output) {
#if defined(ch551) || defined(ch552) || defined(ch549)
    if (port == p1port) {
      p1_mod_oc &= ~bit;
      p1_dir_pu |= bit;
    } else if (port == p3port) {
      p3_mod_oc &= ~bit;
      p3_dir_pu |= bit;
    }
#endif
#if defined(ch549)
    else if (port == p0port) {
      p0_mod_oc &= ~bit;
      p0_dir_pu |= bit;
    } else if (port == p2port) {
      p2_mod_oc &= ~bit;
      p2_dir_pu |= bit;
    } else if (port == p4port) {
      p4_mod_oc &= ~bit;
      p4_dir_pu |= bit;
    }
#endif
#if defined(ch559)
    if (port == p0port) {
      port_cfg &= ~bp0_oc;
      p0_dir |= bit;
    } else if (port == p1port) {
      port_cfg &= ~bp1_oc;
      p1_dir |= bit;
    } else if (port == p2port) {
      port_cfg &= ~bp2_oc;
      p2_dir |= bit;
    } else if (port == p3port) {
      port_cfg &= ~bp3_oc;
      p3_dir |= bit;
    } else if (port == p4port) { // p4 can not do oc
      p4_dir |= bit;
    } // p5 can only do input
#endif
  } else if (mode == output_od) {
#if defined(ch551) || defined(ch552) || defined(ch549)
    if (port == p1port) {
      p1_mod_oc |= bit;
      p1_dir_pu &= ~bit;
    } else if (port == p3port) {
      p3_mod_oc |= bit;
      p3_dir_pu &= ~bit;
    }
#endif
#if defined(ch549)
    else if (port == p0port) {
      p0_mod_oc |= bit;
      p0_dir_pu &= ~bit;
    } else if (port == p2port) {
      p2_mod_oc |= bit;
      p2_dir_pu &= ~bit;
    } else if (port == p4port) {
      p4_mod_oc |= bit;
      p4_dir_pu &= ~bit;
    }
#endif
    // todo: oc mode for ch559
  }
}

static void turnoffpwm(__data uint8_t pwm) {
#if defined(ch551) || defined(ch552)
  switch (pwm) {
  case pin_pwm1:
    if ((pin_func & bpwm1_pin_x) == 0) {
      pwm_ctrl &= ~bpwm1_out_en;
    }
    break;
  case pin_pwm2:
    if ((pin_func & bpwm2_pin_x) == 0) {
      pwm_ctrl &= ~bpwm2_out_en;
    }
    break;
  case pin_pwm1_:
    if ((pin_func & bpwm1_pin_x) != 0) {
      pwm_ctrl &= ~bpwm1_out_en;
    }
    break;
  case pin_pwm2_:
    if ((pin_func & bpwm2_pin_x) != 0) {
      pwm_ctrl &= ~bpwm2_out_en;
    }
    break;
  }
#elif defined(ch559)
  switch (pwm) {
  case pin_pwm1:
    if ((pin_func & bpwm1_pin_x) == 0) {
      pwm_ctrl &= ~bpwm_out_en;
    }
    break;
  case pin_pwm2:
    if ((pin_func & bpwm1_pin_x) == 0) {
      pwm_ctrl &= ~bpwm2_out_en;
    }
    break;
  case pin_pwm1_:
    if ((pin_func & bpwm1_pin_x) != 0) {
      pwm_ctrl &= ~bpwm_out_en;
    }
    break;
  case pin_pwm2_:
    if ((pin_func & bpwm1_pin_x) != 0) {
      pwm_ctrl &= ~bpwm2_out_en;
    }
    break;
  case pin_pwm3:
    if ((pin_func & btmr3_pin_x) == 0) {
      if (t3_ctrl & bt3_out_en) {
        t3_ctrl &= ~(bt3_out_en | bt3_cnt_en);
      }
    }
    break;
  case pin_pwm3_:
    if ((pin_func & btmr3_pin_x) != 0) {
      if (t3_ctrl & bt3_out_en) {
        t3_ctrl &= ~(bt3_out_en | bt3_cnt_en);
      }
    }
    break;
  }
#else
  pwm;
  return;
#endif
  // todo: pwm mode for ch559
}

uint8_t digitalread(__data uint8_t pin) {
  __data uint8_t pwm = digitalpintopwm(pin);
  __data uint8_t bit = digitalpintobitmask(pin);
  __data uint8_t port = digitalpintoport(pin);

  if (port == not_a_pin)
    return low;

  // if the pin that support pwm output, we need to turn it off
  // before getting a digital reading.
  if (pwm != not_on_pwm)
    turnoffpwm(pwm);

  __data uint8_t portbuf = 0;

  switch (port) {
#if defined(ch551) || defined(ch552) || defined(ch549) || defined(ch559)
  case p1port:
    portbuf = p1;
    break;
  case p2port:
    portbuf = p2;
    break;
  case p3port:
    portbuf = p3;
    break;
#endif
#if defined(ch549)
  case p0port:
    portbuf = p0;
    break;
  case p4port:
    portbuf = p4;
    break;
  case p5port:
    portbuf = p5;
    break;
#elif defined(ch559)
  case p0port:
    portbuf = p0;
    break;
  case p4port:
    portbuf = p4_in;
    break;
  case p5port:
    portbuf = p5_in;
    break;
#endif
  default:
    break;
  }

  if (portbuf & bit)
    return high;
  return low;
}

void digitalwrite(__data uint8_t pin, __xdata uint8_t val) {
  __data uint8_t pwm = digitalpintopwm(pin);
  __data uint8_t bit = digitalpintobitmask(pin);
  __data uint8_t port = digitalpintoport(pin);

  // if the pin that support pwm output, we need to turn it off
  // before doing a digital write.
  if (pwm != not_on_pwm)
    turnoffpwm(pwm);

  // c pointers cannot be used to access the 8051's sfrs (special function
  // registers).

  __data uint8_t interrupton = ea;
  ea = 0;

  switch (port) {
#if defined(ch551) || defined(ch552) || defined(ch549) || defined(ch559)
  case p1port:
    if (val == low) {
      p1 &= ~bit;
    } else {
      p1 |= bit;
    }
    break;
  case p2port:
    if (val == low) {
      p2 &= ~bit;
    } else {
      p2 |= bit;
    }
    break;
  case p3port:
    if (val == low) {
      p3 &= ~bit;
    } else {
      p3 |= bit;
    }
    break;
#endif
#if defined(ch549)
  case p0port:
    if (val == low) {
      p0 &= ~bit;
    } else {
      p0 |= bit;
    }
    break;
  case p4port:
    if (val == low) {
      p4 &= ~bit;
    } else {
      p4 |= bit;
    }
    break;
  case p5port:
    if (val == low) {
      p5 &= ~bit;
    } else {
      p5 |= bit;
    }
    break;
#elif defined(ch559)
  case p0port:
    if (val == low) {
      p0 &= ~bit;
    } else {
      p0 |= bit;
    }
    break;
  case p4port:
    if (val == low) {
      p4_out &= ~bit;
    } else {
      p4_out |= bit;
    }
    break;
#endif

  default:
    break;
  }

  if (interrupton)
    ea = 1;
}

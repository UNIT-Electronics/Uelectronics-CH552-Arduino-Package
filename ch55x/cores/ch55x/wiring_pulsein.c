/*
 created by deqing sun for use with ch55xduino
 */

#define arduino_main

// clang-format off
#include "wiring_private.h"

uint16_t digitalreaddirectlutaddrandcarryreturn(uint8_t pin);

#if f_cpu == 24000000
  #define f_cpu_mhz 24
#elif f_cpu == 16000000
  #define f_cpu_mhz 16
#elif f_cpu == 12000000
  #define f_cpu_mhz 12
#elif f_cpu == 32000000
  #define f_cpu_mhz 32
#elif f_cpu == 56000000
  #define f_cpu_mhz 56
#endif

#define str_indir(x) #x
#define str(x) str_indir(x)

uint32_t pulsein(uint8_t pin, __xdata uint8_t state, __xdata uint16_t timeout) {
  pin;
  state;
  timeout;

  // clang-format off
  __asm__(
    //".even                                        \n"
    ";get address of the pin read instruction     \n"
    ";dpl is already ready                        \n"
    "    lcall _digitalreaddirectlutaddrandcarryreturn \n"
    "; backup dptr                                \n"

    ";put loop limit (inc to overflow)            \n"
    ";roughly accurate.                           \n"
    ";value = 65536-(ms*f_cpu*(1e-6)*34)          \n"
    ";that is 16bit x 16 bit = 32bit              \n"
    ";https://developer.arm.com/documentation/ka002453/latest/ \n"
    "    inc _xbus_aux                            \n"  //select dptr1
    "    mov dptr,#_pulsein_parm_3                \n"
    "    movx a,@dptr                             \n" //low byte of timeout
    "    mov b,#(("str(f_cpu_mhz)"*34)&0xff)      \n"
    "    mul ab                                   \n"
    "    mov r1,a                                 \n" //lowest byte of result, we use r1, r2 ,r3 ,r4 for now
    "    mov r2,b                                 \n" //store 2nd byte temporarily
    "    movx a,@dptr                             \n" //low byte of timeout
    "    mov b,#(("str(f_cpu_mhz)"*34)>>8)        \n"
    "    mul ab                                   \n"
    "    add a,r2                                 \n" 
    "    mov r2,a                                 \n" //store 2nd byte temporarily
    "    mov a, #0                                \n" 
    "    addc a,b                                 \n"
    "    mov r3,a                                 \n" //store 3rd byte temporarily
    "    inc dptr                                 \n" 
    "    movx a,@dptr                             \n" //high byte of timeout
    "    mov b,#(("str(f_cpu_mhz)"*34)&0xff)      \n"
    "    mul ab                                   \n"
    "    add a,r2                                 \n"
    "    mov r2,a                                 \n" //store 2nd byte
    "    mov a,r3                                 \n" //restore 3rd byte
    "    addc a,b                                 \n"
    "    mov r3,a                                 \n" //store 3rd byte temporarily
    "    mov a,#0                                 \n"
    "    addc a,#0                                \n"
    "    mov r4,a                                 \n" //store 4th byte temporarily, there might be carry from 3rd byte
    "    movx a,@dptr                             \n" //high byte of timeout
    "    mov b,#(("str(f_cpu_mhz)"*34)>>8)        \n"
    "    mul ab                                   \n"
    "    add a,r3                                 \n"
    "    mov r3,a                                 \n" //store 3rd byte
    "    mov a,r4                                 \n" //restore 4th byte
    "    addc a,b                                 \n"
    "    mov r4,a                                 \n" //store 4th byte
    "  ;65536-result                              \n"
    "    clr a                                    \n"
    "    subb a,r1                                \n"
    "    mov r0,a                                 \n" 
    "    clr a                                    \n"
    "    subb a,r2                                \n"
    "    mov r1,a                                 \n"
    "    clr a                                    \n"
    "    subb a,r3                                \n"
    "    mov r2,a                                 \n"
    "    clr a                                    \n"
    "    subb a,r4                                \n"
    "    mov r3,a                                 \n"
    "    dec _xbus_aux                            \n"  //select dptr0 , set it back 

    ";branching if we meausure high pulse of low  \n"
    "    inc _xbus_aux                            \n"  //select dptr1
    "    mov	dptr,#_pulsein_parm_2               \n"  //do not modify the dptr0 for read pin
    "    movx	a,@dptr                             \n"
    "    dec _xbus_aux                            \n"  //select dptr0 , set it back 
    "    jz lowpulsedetection$                    \n"

    ";highpulse, wait for low, previous pulse end\n"
    "highpulsewaitprevfinish$:                    \n"
    "    inc r0                                   \n"
    "    cjne r0,#0,highpulsewaitprevfinish_overflow_countover$\n"
    "    inc r1                                   \n"
    "    cjne r1,#0,highpulsewaitprevfinish_overflow_countover$\n"
    "    inc r2                                   \n"
    "    cjne r2,#0,highpulsewaitprevfinish_overflow_countover$\n"
    "    inc r3                                   \n"
    "    cjne r3,#0,highpulsewaitprevfinish_overflow_countover$\n"
    ";32bit overflow if get here                  \n"
    "    ljmp failreturn0$                        \n"
    "highpulsewaitprevfinish_overflow_countover$: \n"
    "    lcall __sdcc_call_dptr                   \n"
    "    jc highpulsewaitprevfinish$              \n"
    ";                                            \n"
    ";highpulse, wait for rise                    \n"
    "highpulsewaitrise$:                          \n"
    "    inc r0                                   \n"
    "    cjne r0,#0,highpulsewaitrise_overflow_countover$\n"
    "    inc r1                                   \n"
    "    cjne r1,#0,highpulsewaitrise_overflow_countover$\n"
    "    inc r2                                   \n"
    "    cjne r2,#0,highpulsewaitrise_overflow_countover$\n"
    "    inc r3                                   \n"
    "    cjne r3,#0,highpulsewaitrise_overflow_countover$\n"
    ";32bit overflow if get here                  \n"
    "    ljmp failreturn0$                        \n"
    "highpulsewaitrise_overflow_countover$:       \n"
    "    lcall __sdcc_call_dptr                   \n"
    "    jnc highpulsewaitrise$                   \n"
    ";highpulse, got rising edge, record micros   \n"
    "    inc _xbus_aux                            \n"  //select dptr1
    "    push ar0                                 \n"
    "    push ar1                                 \n"
    "    push ar2                                 \n"
    "    push ar3                                 \n"
    "    lcall	_micros                           \n"
    "    pop ar3                                  \n"
    "    pop ar2                                  \n"
    "    pop ar1                                  \n"
    "    pop ar0                                  \n"
    "    mov	r4,dpl                              \n"  //do not modify the dptr0 for read pin
    "    mov	r5,dph                              \n"
    "    mov  r6,b                                \n"
    "    mov  r7,a                                \n"
    "    dec _xbus_aux                            \n"  //select dptr0 , set it back 
    ";                                            \n"
    ";highpulse, wait for fall                    \n"
    "highpulsewaitfall$:                          \n"
    "    inc r0                                   \n"
    "    cjne r0,#0,highpulsewaitfall_overflow_countover$\n"
    "    inc r1                                   \n"
    "    cjne r1,#0,highpulsewaitfall_overflow_countover$\n"
    "    inc r2                                   \n"
    "    cjne r2,#0,highpulsewaitfall_overflow_countover$\n"
    "    inc r3                                   \n"
    "    cjne r3,#0,highpulsewaitfall_overflow_countover$\n"
    ";32bit overflow if get here                  \n"
    "    ljmp failreturn0$                        \n"
    "highpulsewaitfall_overflow_countover$:       \n"
    "    lcall __sdcc_call_dptr                   \n"
    "    jc highpulsewaitfall$                    \n"
    ";highpulse, got falling edge, record micros  \n"
    ";dptr no longer needed, r0~3 no longer needed\n"
    ";call micros again to get the difference     \n"
    "    ljmp calculatemicrosdiff$                \n"

    "lowpulsedetection$:                          \n"

    ";lowpulse, wait for high, previous pulse end\n"
    "lowpulsewaitprevfinish$:                    \n"
    "    inc r0                                   \n"
    "    cjne r0,#0,lowpulsewaitprevfinish_overflow_countover$\n"
    "    inc r1                                   \n"
    "    cjne r1,#0,lowpulsewaitprevfinish_overflow_countover$\n"
    "    inc r2                                   \n"
    "    cjne r2,#0,lowpulsewaitprevfinish_overflow_countover$\n"
    "    inc r3                                   \n"
    "    cjne r3,#0,lowpulsewaitprevfinish_overflow_countover$\n"
    ";32bit overflow if get here                  \n"
    "    ljmp failreturn0$                        \n"
    "lowpulsewaitprevfinish_overflow_countover$: \n"
    "    lcall __sdcc_call_dptr                   \n"
    "    jnc lowpulsewaitprevfinish$              \n"
    ";                                            \n"
    ";lowpulse, wait for fall                    \n"
    "lowpulsewaitfall$:                          \n"
    "    inc r0                                   \n"
    "    cjne r0,#0,lowpulsewaitfall_overflow_countover$\n"
    "    inc r1                                   \n"
    "    cjne r1,#0,lowpulsewaitfall_overflow_countover$\n"
    "    inc r2                                   \n"
    "    cjne r2,#0,lowpulsewaitfall_overflow_countover$\n"
    "    inc r3                                   \n"
    "    cjne r3,#0,lowpulsewaitfall_overflow_countover$\n"
    ";32bit overflow if get here                  \n"
    "    ljmp failreturn0$                        \n"
    "lowpulsewaitfall_overflow_countover$:       \n"
    "    lcall __sdcc_call_dptr                   \n"
    "    jnc lowpulsewaitfall$                   \n"
    ";lowpulse, got falling edge, record micros   \n"
    "    inc _xbus_aux                            \n"  //select dptr1
    "    push ar0                                 \n"
    "    push ar1                                 \n"
    "    push ar2                                 \n"
    "    push ar3                                 \n"
    "    lcall	_micros                           \n"
    "    pop ar3                                  \n"
    "    pop ar2                                  \n"
    "    pop ar1                                  \n"
    "    pop ar0                                  \n"
    "    mov	r4,dpl                              \n"  //do not modify the dptr0 for read pin
    "    mov	r5,dph                              \n"
    "    mov  r6,b                                \n"
    "    mov  r7,a                                \n"
    "    dec _xbus_aux                            \n"  //select dptr0 , set it back 
    ";                                            \n"
    ";lowpulse, wait for rise                    \n"
    "lowpulsewaitrise$:                          \n"
    "    inc r0                                   \n"
    "    cjne r0,#0,lowpulsewaitrise_overflow_countover$\n"
    "    inc r1                                   \n"
    "    cjne r1,#0,lowpulsewaitrise_overflow_countover$\n"
    "    inc r2                                   \n"
    "    cjne r2,#0,lowpulsewaitrise_overflow_countover$\n"
    "    inc r3                                   \n"
    "    cjne r3,#0,lowpulsewaitrise_overflow_countover$\n"
    ";32bit overflow if get here                  \n"
    "    ljmp failreturn0$                        \n"
    "lowpulsewaitrise_overflow_countover$:       \n"
    "    lcall __sdcc_call_dptr                   \n"
    "    jc lowpulsewaitrise$                    \n"
    ";lowpulse, got rising edge, record micros  \n"
    ";dptr no longer needed, r0~3 no longer needed\n"
    ";call micros again to get the difference     \n"
    "    ljmp calculatemicrosdiff$                \n"

    "calculatemicrosdiff$:                        \n"
    "    push ar4                                 \n"
    "    push ar5                                 \n"
    "    push ar6                                 \n"
    "    push ar7                                 \n"
    "    lcall	_micros                           \n"
    "    pop ar7                                  \n"
    "    pop ar6                                  \n"
    "    pop ar5                                  \n"
    "    pop ar4                                  \n"
    "    mov	r0,a                                \n"
    "    clr c                                    \n"
    "    mov	a,dpl                               \n"
    "    subb	a,r4                                \n"
    "    mov	dpl,a                               \n"
    "    mov	a,dph                               \n"
    "    subb	a,r5                                \n"
    "    mov	dph,a                               \n"
    "    mov	a,b                                 \n"
    "    subb	a,r6                                \n"
    "    mov	b,a                                 \n"
    "    mov	a,r0                                \n"
    "    subb	a,r7                                \n"
    "    ret                                      \n"
 
    "failreturn0$:                                \n"
    "    clr a                                    \n"
    "    mov dpl,a                                \n"
    "    mov dph,a                                \n"
    "    mov b,a                                  \n"
    "    ret                                      \n"
  );
  // clang-format on  
}

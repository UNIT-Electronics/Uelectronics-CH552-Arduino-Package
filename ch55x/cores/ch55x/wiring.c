/*
 created by deqing sun for use with ch55xduino
 */

#include "wiring_private.h"

#ifndef user_usb_ram
void usbdevicecfg();
void usbdeviceintcfg();
void usbdeviceendpointcfg();
#endif

extern __idata volatile uint32_t timer0_overflow_count;
extern __idata volatile uint8_t timer0_overflow_count_5th_byte;

#if f_cpu == 56000000
#define t0_cycle 224
#else
#define t0_cycle 250
#endif

// using register bank 1
void timer0interrupt(void) __interrupt(int_no_tmr0) __using(1) {
  /*timer0_overflow_count++;
   */ //when putting timer0_millis and timer0_overflow_count in bank 1, c code is no longer correct
  __asm__(";increase timer0_overflow_count on r0~r4(5bytes)\n"
          "    inc r0                                      \n"
          "    cjne r0,#0,inctimer0_overflow_countover$    \n"
          "    inc r1                                      \n"
          "    cjne r1,#0,inctimer0_overflow_countover$    \n"
          "    inc r2                                      \n"
          "    cjne r2,#0,inctimer0_overflow_countover$    \n"
          "    inc r3                                      \n"
          "    cjne r3,#0,inctimer0_overflow_countover$    \n"
          "    inc r4                                      \n"
          "inctimer0_overflow_countover$:                  \n");
}

uint32_t micros() {
  /*uint32_t m;
   uint8_t t;
   uint8_t interrupton = ea;
   ea = 0;

   m = timer0_overflow_count;
   t = tl0;

   if ((tf0) && (t < 255)){
   m++;
   }

   if (interrupton) ea = 1;*/

  __asm__(";uint8_t interrupton = ea; //to c            \n"
          ";clr and mov won't affect c                  \n"
          "    mov c,_ea                                \n"
          ";ea = 0;                                     \n"
          "    clr _ea                                  \n"
          ";copy _timer0_overflow_count to local r0~r3,m\n"
          "    mov r0, (_timer0_overflow_count)         \n"
          "    mov r1, (_timer0_overflow_count)+1       \n"
          "    mov r2, (_timer0_overflow_count)+2       \n"
          "    mov r3, (_timer0_overflow_count)+3       \n"
          ";copy tl0 to local r4, t                     \n"
          "    mov r4, _tl0                             \n"
          ";copy tcon (tf0) to local r5                 \n"
          "    mov b, _tcon                             \n"
          ";if (interrupton) ea = 1;                    \n"
          "    mov _ea,c                                \n"

          ";if ((tf0 in b) && (r4 != 255)){             \n"
          "    jnb b.5,inctimer0_overf_cntcopyover$     \n"
          "    mov a,#1     \n"
          "    add a,r4     \n"
          "    jz inctimer0_overf_cntcopyover$\n"

          ";m++                                         \n"
          "    inc r0                                   \n"
          "    cjne r0,#0,inctimer0_overf_cntcopyover$  \n"
          "    inc r1                                   \n"
          "    cjne r1,#0,inctimer0_overf_cntcopyover$  \n"
          "    inc r2                                   \n"
          "    cjne r2,#0,inctimer0_overf_cntcopyover$  \n"
          "    inc r3                                   \n"
          "inctimer0_overf_cntcopyover$:                \n");

  // since tl0 (r4) always ranging from (256-t0_cycle) to 255, we can reduce r4
  // by (256-t0_cycle)

#if t0_cycle == 250
  __asm__("    clr c                                    \n"
          "    mov a, r4                                \n"
          "    subb a,#6                                \n"
          "    mov r4, a                                \n");
#elif t0_cycle == 224
  __asm__("    clr c                                    \n"
          "    mov a, r4                                \n"
          "    subb a,#32                               \n"
          "    mov r4, a                                \n");
#endif

#if f_cpu == 16000000
  // 1m = 250t 1t=0.75us (m*250+t)*3/2/2
  // m in r0~r3
  // t in r4
  // t=((3*t)>>1);
  // m=(m*375)=m<<8+m*119;
  // return (m+t)>>1

  __asm__(";1m = 250t 1t=0.5us (m*250+t)*3/2/2  t is 0~249\n"
          ";we need to return (m<<8+m*119+((3*t)>>1)))>>1 \n"

          ";m=m*119;                                    \n"
          "    mov a, r0                                \n"
          "    mov r6, a                                \n"
          "    mov a, r1                                \n"
          "    mov r7, a                                \n"
          "    push ar3                                 \n"
          "    push ar2                                 \n"

          "    mov b, #119                              \n"
          "    mov a, r0                                \n"
          "    mul ab                                   \n"
          "    mov r0, a                                \n"
          "    mov r5, b                                \n"

          "    mov b, #119                              \n"
          "    mov a, r1                                \n"
          "    mul ab                                   \n"
          "    add a, r5                                \n"
          "    mov r1, a                                \n"
          "    clr a                                    \n"
          "    addc a, b                                \n"
          "    mov r5, a                                \n"

          "    mov b, #119                              \n"
          "    mov a, r2                                \n"
          "    mul ab                                   \n"
          "    add a, r5                                \n"
          "    mov r2, a                                \n"
          "    clr a                                    \n"
          "    addc a, b                                \n"
          "    mov r5, a                                \n"

          "    mov b, #119                              \n"
          "    mov a, r3                                \n"
          "    mul ab                                   \n"
          "    add a, r5                                \n"
          "    mov r3, a                                \n"
          "    clr a                                    \n"
          "    addc a, b                                \n"
          "    mov r5, a                                \n"

          ";m=m+m<<8;                                   \n"

          "    mov a, r6                                \n"
          "    add a, r1                                \n"
          "    mov r1, a                                \n"
          "    mov a, r7                                \n"
          "    addc a, r2                               \n"
          "    mov r2, a                                \n"
          "    pop a                                    \n"
          "    addc a, r3                               \n"
          "    mov r3, a                                \n"
          "    pop a                                    \n"
          "    addc a, r5                               \n"
          "    mov r5, a                                \n"

          ";t=((3*t)>>1));                              \n"

          "    mov b, #3                                \n"
          "    mov a, r4                                \n"
          "    mul ab                                   \n"
          "    mov r4, a                                \n"
          "    mov a, b                                 \n"
          "    clr c                                    \n"
          "    rrc a                                    \n"
          "    mov r6, a                                \n"
          "    mov a, r4                                \n"
          "    rrc a                                    \n"
          "    mov r4, a                                \n"

          ";get m+t                                     \n"
          "    mov r7, #0                               \n"
          "    mov a, r4                                \n"
          "    add a, r0                                \n"
          "    mov r0, a                                \n"
          "    mov a, r6                                \n"
          "    addc a, r1                               \n"
          "    mov r1, a                                \n"
          "    mov a, r7                                \n"
          "    addc a, r2                               \n"
          "    mov r2, a                                \n"
          "    mov a, r7                                \n"
          "    addc a, r3                               \n"
          "    mov r3, a                                \n"
          "    mov a, r7                                \n"
          "    addc a, r5                               \n"
          "    mov r5, a                                \n"

          ";return m+t/2                                \n"
          "    clr c                                    \n"
          "    mov a, r5                                \n"
          "    rrc a                                    \n"
          "    mov r5, a                                \n"
          "    mov a, r3                                \n"
          "    rrc a                                    \n"
          "    mov r3, a                                \n"
          "    mov a, r2                                \n"
          "    rrc a                                    \n"
          "    mov b, a                                 \n"
          "    mov a, r1                                \n"
          "    rrc a                                    \n"
          "    mov dph, a                               \n"
          "    mov a, r0                                \n"
          "    rrc a                                    \n"
          "    mov dpl, a                               \n"
          "    mov a, r3                                \n"

  );
  // ’dpl’ (lsb),’dph’,’b’ & ’acc’
#elif f_cpu == 24000000
  // 24m clk

  /*1m = 250t 1t=0.5us (m*250+t)/2

  t=(t>>1);
  m=m*125;

  return ( m+t );*/

  // assembly has better support for multiplication

  __asm__(";1m = 250t 1t=0.5us (m*250+t)/2  t is 0~249  \n"
          ";we need to return m*125+t/2                 \n"
          ";t=(t>>1);                                   \n"
          "    mov a,r4                                 \n"
          "    clr c                                    \n"
          "    rrc a                                    \n"
          "    mov r4,a                                 \n"

          ";m=m*125;                                    \n"
          "    mov b, #125                              \n"
          "    mov a, r0                                \n"
          "    mul ab                                   \n"
          "    mov r0, a                                \n"
          "    mov r5, b                                \n"

          "    mov b, #125                              \n"
          "    mov a, r1                                \n"
          "    mul ab                                   \n"
          "    add a, r5                                \n"
          "    mov r1, a                                \n"
          "    clr a                                    \n"
          "    addc a, b                                \n"
          "    mov r5, a                                \n"

          "    mov b, #125                              \n"
          "    mov a, r2                                \n"
          "    mul ab                                   \n"
          "    add a, r5                                \n"
          "    mov r2, a                                \n"
          "    clr a                                    \n"
          "    addc a, b                                \n"
          "    mov r5, a                                \n"

          "    mov b, #125                              \n"
          "    mov a, r3                                \n"
          "    mul ab                                   \n"
          "    add a, r5                                \n"
          "    mov r3, a                                \n"

          ";return m+t                                  \n"
          "    mov r5, #0                               \n"
          "    mov a, r4                                \n"
          "    add a, r0                                \n"
          "    mov dpl, a                               \n"
          "    mov a, r1                                \n"
          "    addc a, r5                               \n"
          "    mov dph, a                               \n"
          "    mov a, r2                                \n"
          "    addc a, r5                               \n"
          "    mov b, a                                 \n"
          "    mov a, r3                                \n"
          "    addc a, r5                               \n"

  );
#elif f_cpu == 32000000
  // 1m = 250t 1t=0.375us (m*250+t)*3/2/2/2
  // m in r0~r3
  // t in r4
  // t=((3*t)>>1);
  // m=(m*375)=m<<8+m*119;
  // return (m+t)>>2

  __asm__(";1m = 250t 1t=0.5us (m*250+t)*3/2/2  t is 0~249\n"
          ";we need to return (m<<8+m*119+((3*t)>>1)))>>1 \n"

          ";m=m*119;                                    \n"
          "    mov a, r0                                \n"
          "    mov r6, a                                \n"
          "    mov a, r1                                \n"
          "    mov r7, a                                \n"
          "    push ar3                                 \n"
          "    push ar2                                 \n"

          "    mov b, #119                              \n"
          "    mov a, r0                                \n"
          "    mul ab                                   \n"
          "    mov r0, a                                \n"
          "    mov r5, b                                \n"

          "    mov b, #119                              \n"
          "    mov a, r1                                \n"
          "    mul ab                                   \n"
          "    add a, r5                                \n"
          "    mov r1, a                                \n"
          "    clr a                                    \n"
          "    addc a, b                                \n"
          "    mov r5, a                                \n"

          "    mov b, #119                              \n"
          "    mov a, r2                                \n"
          "    mul ab                                   \n"
          "    add a, r5                                \n"
          "    mov r2, a                                \n"
          "    clr a                                    \n"
          "    addc a, b                                \n"
          "    mov r5, a                                \n"

          "    mov b, #119                              \n"
          "    mov a, r3                                \n"
          "    mul ab                                   \n"
          "    add a, r5                                \n"
          "    mov r3, a                                \n"
          "    clr a                                    \n"
          "    addc a, b                                \n"
          "    mov r5, a                                \n"

          ";m=m+m<<8;                                   \n"

          "    mov a, r6                                \n"
          "    add a, r1                                \n"
          "    mov r1, a                                \n"
          "    mov a, r7                                \n"
          "    addc a, r2                               \n"
          "    mov r2, a                                \n"
          "    pop a                                    \n"
          "    addc a, r3                               \n"
          "    mov r3, a                                \n"
          "    pop a                                    \n"
          "    addc a, r5                               \n"
          "    mov r5, a                                \n"

          ";t=((3*t)>>1));                              \n"

          "    mov b, #3                                \n"
          "    mov a, r4                                \n"
          "    mul ab                                   \n"
          "    mov r4, a                                \n"
          "    mov a, b                                 \n"
          "    clr c                                    \n"
          "    rrc a                                    \n"
          "    mov r6, a                                \n"
          "    mov a, r4                                \n"
          "    rrc a                                    \n"
          "    mov r4, a                                \n"

          ";get m+t                                     \n"
          "    mov r7, #0                               \n"
          "    mov a, r4                                \n"
          "    add a, r0                                \n"
          "    mov r0, a                                \n"
          "    mov a, r6                                \n"
          "    addc a, r1                               \n"
          "    mov r1, a                                \n"
          "    mov a, r7                                \n"
          "    addc a, r2                               \n"
          "    mov r2, a                                \n"
          "    mov a, r7                                \n"
          "    addc a, r3                               \n"
          "    mov r3, a                                \n"
          "    mov a, r7                                \n"
          "    addc a, r5                               \n"
          "    mov r5, a                                \n"

          ";return m+t/2                                \n"
          "    clr c                                    \n"
          "    mov a, r5                                \n"
          "    rrc a                                    \n"
          "    mov r5, a                                \n"
          "    mov a, r3                                \n"
          "    rrc a                                    \n"
          "    mov r3, a                                \n"
          "    mov a, r2                                \n"
          "    rrc a                                    \n"
          "    mov r2, a                                \n"
          "    mov a, r1                                \n"
          "    rrc a                                    \n"
          "    mov r1, a                                \n"
          "    mov a, r0                                \n"
          "    rrc a                                    \n"
          "    mov r0, a                                \n"

          ";return m+t/2/2                              \n"
          "    clr c                                    \n"
          "    mov a, r5                                \n"
          "    rrc a                                    \n"
          "    mov r5, a                                \n"
          "    mov a, r3                                \n"
          "    rrc a                                    \n"
          "    mov r3, a                                \n"
          "    mov a, r2                                \n"
          "    rrc a                                    \n"
          "    mov b, a                                 \n"
          "    mov a, r1                                \n"
          "    rrc a                                    \n"
          "    mov dph, a                               \n"
          "    mov a, r0                                \n"
          "    rrc a                                    \n"
          "    mov dpl, a                               \n"
          "    mov a, r3                                \n"

  );
  // ’dpl’ (lsb),’dph’,’b’ & ’acc’
#elif f_cpu == 56000000
  // 56m clk

  /*1m = 224t 1t=0.21412us=3/14us (m*224+t)*3/14
   m*48+t*3/14
   //m in r0~r3
   //t in r4*/

  // assembly has better support for multiplication

  __asm__(";t=t*3/14; t ranging 0~223                   \n"
          "    mov a,r4                                 \n"
          "    mov b,#14                                \n"
          "    div ab                                   \n"
          "    mov r5,b                                 \n"
          "    mov b,#3                                 \n"
          "    mul ab                                   \n"
          "    mov r4,a                                 \n"
          ";now r4=int(r4/14)*3                         \n"
          ";r4=r4+reminder*3/14                         \n"
          "    mov a,r5                                 \n"
          "    mov b,#3                                 \n"
          "    mul ab                                   \n"
          "    mov b,#14                                \n"
          "    div ab                                   \n"
          "    add a,r4                                 \n"
          "    mov r4,a                                 \n"

          ";m=m*48;                                     \n"
          "    mov b, #48                               \n"
          "    mov a, r0                                \n"
          "    mul ab                                   \n"
          "    mov r0, a                                \n"
          "    mov r5, b                                \n"

          "    mov b, #48                               \n"
          "    mov a, r1                                \n"
          "    mul ab                                   \n"
          "    add a, r5                                \n"
          "    mov r1, a                                \n"
          "    clr a                                    \n"
          "    addc a, b                                \n"
          "    mov r5, a                                \n"

          "    mov b, #48                               \n"
          "    mov a, r2                                \n"
          "    mul ab                                   \n"
          "    add a, r5                                \n"
          "    mov r2, a                                \n"
          "    clr a                                    \n"
          "    addc a, b                                \n"
          "    mov r5, a                                \n"

          "    mov b, #48                               \n"
          "    mov a, r3                                \n"
          "    mul ab                                   \n"
          "    add a, r5                                \n"
          "    mov r3, a                                \n"
          // 5th byte not needed
          //"    clr a                                    \n"
          //"    addc a, b                                \n"
          //"    mov r5, a                                \n"

          ";m=m+t                                       \n"
          "    mov r6, #0                               \n"
          "    mov a, r4                                \n"
          "    add a, r0                                \n"
          "    mov r0, a                                \n"
          "    mov a, r1                                \n"
          "    addc a, r6                               \n"
          "    mov r1, a                                \n"
          "    mov a, r2                                \n"
          "    addc a, r6                               \n"
          "    mov r2, a                                \n"
          "    mov a, r3                                \n"
          "    addc a, r6                               \n"
          "    mov r3, a                                \n"
          // 5th byte not needed
          //"    mov a, r5                                \n"
          //"    addc a, r6                               \n"
          //"    mov r5, a                                \n"

          ";return                                      \n"

          "    mov dpl, r0                              \n"
          "    mov dph, r1                              \n"
          "    mov b, r2                                \n"
          "    mov a, r3                                \n"

  );
#elif f_cpu == 12000000
  // 12m clk

  /*1m = 250t 1t=1us (m*250+t)

   t=(t);
   m=m*250;

   return ( m+t );*/

  // assembly has better support for multiplication

  __asm__(";1m = 250t 1t=1us (m*250+t)    t is 0~249    \n"
          ";we need to return m*250+t                   \n"

          ";m=m*250;                                    \n"
          "    mov b, #250                              \n"
          "    mov a, r0                                \n"
          "    mul ab                                   \n"
          "    mov r0, a                                \n"
          "    mov r5, b                                \n"

          "    mov b, #250                              \n"
          "    mov a, r1                                \n"
          "    mul ab                                   \n"
          "    add a, r5                                \n"
          "    mov r1, a                                \n"
          "    clr a                                    \n"
          "    addc a, b                                \n"
          "    mov r5, a                                \n"

          "    mov b, #250                              \n"
          "    mov a, r2                                \n"
          "    mul ab                                   \n"
          "    add a, r5                                \n"
          "    mov r2, a                                \n"
          "    clr a                                    \n"
          "    addc a, b                                \n"
          "    mov r5, a                                \n"

          "    mov b, #250                              \n"
          "    mov a, r3                                \n"
          "    mul ab                                   \n"
          "    add a, r5                                \n"
          "    mov r3, a                                \n"

          ";return m+t                                  \n"
          "    mov r5, #0                               \n"
          "    mov a, r4                                \n"
          "    add a, r0                                \n"
          "    mov dpl, a                               \n"
          "    mov a, r1                                \n"
          "    addc a, r5                               \n"
          "    mov dph, a                               \n"
          "    mov a, r2                                \n"
          "    addc a, r5                               \n"
          "    mov b, a                                 \n"
          "    mov a, r3                                \n"
          "    addc a, r5                               \n"

  );
#else
#error "clock not supported yet"

#endif
  // return values: ’dpl’ 1b, ’dpl’ lsb & ’dph’ 2b,
  // ’dpl’, ’dph’ and ’b’ 3b, ’dpl’,’dph’,’b’ & ’acc’ 4b
}

uint32_t millis() {

  // disable interrupts while we read timer0_millis or we might get an
  // inconsistent value (e.g. in the middle of a write to timer0_millis)

  // assembly has better support for 5 byte data type
  // and efficient in shifting

  __asm__(";uint8_t interrupton = ea; //to c            \n"
          ";clr and mov won't affect c                  \n"
          "    mov c,_ea                                \n"
          ";ea = 0;                                     \n"
          "    clr _ea                                  \n"
          ";copy _timer0_overflow_count to local r0~r4  \n"
          "    mov r0, (_timer0_overflow_count)         \n"
          "    mov r1, (_timer0_overflow_count)+1       \n"
          "    mov r2, (_timer0_overflow_count)+2       \n"
          "    mov r3, (_timer0_overflow_count)+3       \n"
          "    mov r4, (_timer0_overflow_count)+4       \n"
          ";if (interrupton) ea = 1;                    \n"
          "    mov _ea,c                                \n");

#if f_cpu == 16000000
  __asm__(";return (timer0_overflow_count*48)>>8        \n"

          "    mov b, #48                               \n"
          "    mov a, r0                                \n"
          "    mul ab                                   \n"
          "    mov r0, b                                \n"
          ";lowest 8 bit not used (a), r0 free to use   \n"
          "    mov b, #48                               \n"
          "    mov a, r1                                \n"
          "    mul ab                                   \n"
          "    add a, r0                                \n"
          "    mov r5, psw   ;keep c                    \n"
          "    mov dpl, a                               \n"
          "    mov r0, b                                \n"

          "    mov b, #48                               \n"
          "    mov a, r2                                \n"
          "    mul ab                                   \n"
          "    mov psw, r5   ;restore c                 \n"
          "    addc a, r0                               \n"
          "    mov r5, psw   ;keep c                    \n"
          "    mov dph, a                               \n"
          "    mov r0, b                                \n"

          "    mov b, #48                               \n"
          "    mov a, r3                                \n"
          "    mul ab                                   \n"
          "    mov psw, r5   ;restore c                 \n"
          "    addc a, r0                               \n"
          "    mov r5, psw   ;keep c                    \n"
          "    mov r1, a                                \n"
          "    mov r0, b                                \n"

          "    mov b, #48                               \n"
          "    mov a, r4                                \n"
          "    mul ab                                   \n"
          "    mov psw, r5   ;restore c                 \n"
          "    addc a, r0                               \n"

          ";calculation finished, a already in place    \n"
          "    mov b, r1                                \n");

#elif f_cpu == 24000000
  __asm__(";return timer0_overflow_count>>3             \n"
          ";or: return (timer0_overflow_count<<5)>>8    \n"
          ";or: return (timer0_overflow_count*32)>>8    \n"
          "    mov b, #32                               \n"
          "    mov a, r0                                \n"
          "    mul ab                                   \n"
          "    mov r0, b                                \n"
          ";lowest 8 bit not used (a), r0 free to use   \n"
          "    mov b, #32                               \n"
          "    mov a, r1                                \n"
          "    mul ab                                   \n"
          "    add a, r0                                \n"
          ";carry won't be set, if i calculated right   \n"
          "    mov dpl, a                               \n"
          "    mov r0, b                                \n"

          "    mov b, #32                               \n"
          "    mov a, r2                                \n"
          "    mul ab                                   \n"
          "    add a, r0                                \n"
          ";carry won't be set, if i calculated right   \n"
          "    mov dph, a                               \n"
          "    mov r0, b                                \n"

          "    mov b, #32                               \n"
          "    mov a, r3                                \n"
          "    mul ab                                   \n"
          "    add a, r0                                \n"
          ";carry won't be set, if i calculated right   \n"
          "    mov r1, a                                \n"
          "    mov r0, b                                \n"

          "    mov b, #32                               \n"
          "    mov a, r4                                \n"
          "    mul ab                                   \n"
          "    add a, r0                                \n"
          ";carry won't be set, if i calculated right   \n"

          ";calculation finished, a already in place    \n"
          "    mov b, r1                                \n");

#elif f_cpu == 32000000
  __asm__(";return (timer0_overflow_count*24)>>8        \n"

          "    mov b, #24                               \n"
          "    mov a, r0                                \n"
          "    mul ab                                   \n"
          "    mov r0, b                                \n"
          ";lowest 8 bit not used (a), r0 free to use   \n"
          "    mov b, #24                               \n"
          "    mov a, r1                                \n"
          "    mul ab                                   \n"
          "    add a, r0                                \n"
          "    mov r5, psw   ;keep c                    \n"
          "    mov dpl, a                               \n"
          "    mov r0, b                                \n"

          "    mov b, #24                               \n"
          "    mov a, r2                                \n"
          "    mul ab                                   \n"
          "    mov psw, r5   ;restore c                 \n"
          "    addc a, r0                               \n"
          "    mov r5, psw   ;keep c                    \n"
          "    mov dph, a                               \n"
          "    mov r0, b                                \n"

          "    mov b, #24                               \n"
          "    mov a, r3                                \n"
          "    mul ab                                   \n"
          "    mov psw, r5   ;restore c                 \n"
          "    addc a, r0                               \n"
          "    mov r5, psw   ;keep c                    \n"
          "    mov r1, a                                \n"
          "    mov r0, b                                \n"

          "    mov b, #24                               \n"
          "    mov a, r4                                \n"
          "    mul ab                                   \n"
          "    mov psw, r5   ;restore c                 \n"
          "    addc a, r0                               \n"

          ";calculation finished, a already in place    \n"
          "    mov b, r1                                \n");

#elif f_cpu == 56000000
  __asm__(
      ";return timer0_overflow_count*6/125          \n"
      // mutiply by 6
      "    mov b, #6                                \n"
      "    mov a, r0                                \n"
      "    mul ab                                   \n"
      "    mov r0, a                                \n"
      "    mov r5, b                                \n"

      "    mov b, #6                                \n"
      "    mov a, r1                                \n"
      "    mul ab                                   \n"
      "    add a, r5                                \n"
      "    mov r1, a                                \n"
      "    clr a                                    \n"
      "    addc a, b                                \n"
      "    mov r5, a                                \n"

      "    mov b, #6                                \n"
      "    mov a, r2                                \n"
      "    mul ab                                   \n"
      "    add a, r5                                \n"
      "    mov r2, a                                \n"
      "    clr a                                    \n"
      "    addc a, b                                \n"
      "    mov r5, a                                \n"

      "    mov b, #6                                \n"
      "    mov a, r3                                \n"
      "    mul ab                                   \n"
      "    add a, r5                                \n"
      "    mov r3, a                                \n"
      "    clr a                                    \n"
      "    addc a, b                                \n"
      "    mov r5, a                                \n"

      "    mov b, #6                                \n"
      "    mov a, r4                                \n"
      "    mul ab                                   \n"
      "    add a, r5                                \n"
      "    mov r4, a                                \n"
      "    clr a                                    \n"
      "    addc a, b                                \n"
      "    mov r5, a                                \n"

      // ref:mcs51 三字节无符号除法程序（asm）modify for 6 bytes
      "    mov r7,#125                              \n"
      "    mov a,r5                                 \n"
      "    mov b,r7                                 \n"
      "    div ab                                   \n"
      "    mov r5,b                                 \n"
      "    mov r6,a                                 \n"
      "    mov b,#40                                \n"
      ";after keeping high result, iterate all b bit with left shift \n"
      "1$: ;div125_iter_bits                        \n"
      "    clr c                                    \n"
      "    mov a,r0                                 \n"
      "    rlc a                                    \n"
      "    mov r0,a                                 \n"
      "    mov a,r1                                 \n"
      "    rlc a                                    \n"
      "    mov r1,a                                 \n"
      "    mov a,r2                                 \n"
      "    rlc a                                    \n"
      "    mov r2,a                                 \n"
      "    mov a,r3                                 \n"
      "    rlc a                                    \n"
      "    mov r3,a                                 \n"
      "    mov a,r4                                 \n"
      "    rlc a                                    \n"
      "    mov r4,a                                 \n"
      "    mov a,r5                                 \n"
      "    rlc a                                    \n"
      "    mov r5,a                                 \n"
      "    mov f0,c                                 \n"
      "    clr c                                    \n"
      "    subb a,r7                                \n"
      ";get 1 for result with overflow on shift or non-negitive subb result \n"
      "    jb f0,2$                                 \n"
      "    jc 3$    ;get 0 for this bit             \n"
      "2$: ;shift overflow                          \n"
      "    mov r5,a                                 \n"
      "    inc r0                                   \n"
      "3$: ;iternate next bit                       \n"
      "    djnz b,1$                                \n"
      "    mov a,r6                                 \n"
      "    clr ov                                   \n"
      "    jz 4$                                    \n"
      "    setb ov                                  \n"
      "4$: ;finish calculation, set high byte and reminder \n"
      "    xch a,r5                                 \n"
      "    mov r7,a                                 \n"

      ";return                                      \n"
      "    mov dpl, r0                              \n"
      "    mov dph, r1                              \n"
      "    mov b, r2                                \n"
      "    mov a, r3                                \n"

  );
#elif f_cpu == 12000000
  __asm__(";return timer0_overflow_count>>2             \n"
          ";or: return (timer0_overflow_count<<6)>>8    \n"
          ";or: return (timer0_overflow_count*64)>>8    \n"
          "    mov b, #64                               \n"
          "    mov a, r0                                \n"
          "    mul ab                                   \n"
          "    mov r0, b                                \n"
          ";lowest 8 bit not used (a), r0 free to use   \n"
          "    mov b, #64                               \n"
          "    mov a, r1                                \n"
          "    mul ab                                   \n"
          "    add a, r0                                \n"
          ";carry won't be set, if i calculated right   \n"
          "    mov dpl, a                               \n"
          "    mov r0, b                                \n"

          "    mov b, #64                               \n"
          "    mov a, r2                                \n"
          "    mul ab                                   \n"
          "    add a, r0                                \n"
          ";carry won't be set, if i calculated right   \n"
          "    mov dph, a                               \n"
          "    mov r0, b                                \n"

          "    mov b, #64                               \n"
          "    mov a, r3                                \n"
          "    mul ab                                   \n"
          "    add a, r0                                \n"
          ";carry won't be set, if i calculated right   \n"
          "    mov r1, a                                \n"
          "    mov r0, b                                \n"

          "    mov b, #64                               \n"
          "    mov a, r4                                \n"
          "    mul ab                                   \n"
          "    add a, r0                                \n"
          ";carry won't be set, if i calculated right   \n"

          ";calculation finished, a already in place    \n"
          "    mov b, r1                                \n");
#else
#error "clock not supported yet"

#endif
}

void delay(__data uint32_t ms) {
  __data uint32_t start = micros();

  while (ms > 0) {
    // yield();
    while (ms > 0 && (micros() - start) >= 1000) {
      ms--;
      start += 1000;
    }
  }
}

void delaymicroseconds(__data uint16_t us) {
  // call with const, "mov dptr, #const" and "lcall", 3 + (6 or 7) cycles,
  // depending on even or odd address call with var, "mov dpl, r", "mov dph, r",
  // and "lcall", 2 + 2 + (6 or 7) cycles, depending on odd or even address the
  // compiler by default uses a caller saves convention for register saving
  // across function calls
  us; // avoid unreferenced function argument warning
#if f_cpu >= 56000000ul
  __asm__(
      ".even                                    \n"
      "    mov  r6, dpl                         \n" // low 8-bit
      "    mov  r7, dph                         \n" // high 8-bit
      "    clr  c                               \n"
      "    mov  a,#0x00                         \n"
      "    subb a, r6                           \n"
      "    clr  a                               \n"
      "    subb a, r7                           \n"
      "    jc skip_0us$                         \n"
      "    ret                                  \n" // return if 0 us  about
                                                    // 0.48us total
      "    nop                                  \n"
      "skip_0us$:                               \n"
      "    clr  c                               \n" // do some loop init, not
                                                    // useful for 1us but better
                                                    // here
      "    mov  a, #0x01                        \n"
      "    subb a, r6                           \n"
      "    mov  r6, a                           \n"
      "    mov  a, #0x00                        \n"
      "    subb a, r7                           \n"
      "    mov  r7, a                           \n"

      "    nop                                  \n" // keep even
      "    nop \n nop \n nop \n nop \n nop \n     "
      "    nop \n nop \n nop \n nop \n nop \n     "
      "    cjne r6,#0x00,loop56m_us$            \n"
      "    cjne r7,#0x00,loop56m_us$            \n"
      "    nop \n                                 "
      "    ret                                  \n" // return if 1us  about 1 us
                                                    // total

      "loop56m_us$:                             \n" // about nus
      "    nop                                  \n" // 6 cycle
      "    nop \n nop \n nop \n nop \n nop \n     "
      "loop56m_us_2$:                           \n"

      "    mov r5, #7                           \n" // 2 cycle
      "loop_rep_nop$:                           \n"
      "    djnz r5, loop_rep_nop$               \n" // 6*6+2 = 38 cycle
      "    nop \n nop \n nop \n                   " // 3 cycle
      "    inc  r6                              \n" // 1 cycle
      "    cjne r6, #0,loop56m_us$              \n" // 6 cycle
      "    inc  r7                              \n" // there will be extra 1
                                                    // cycles for every 256us
      "    cjne r7, #0,loop56m_us_2$            \n");
#elif f_cpu >= 32000000ul
  __asm__(
      ".even                                    \n"
      "    mov  r6, dpl                         \n" // low 8-bit
      "    mov  r7, dph                         \n" // high 8-bit
      "    clr  c                               \n"
      "    mov  a,#0x00                         \n"
      "    subb a, r6                           \n"
      "    clr  a                               \n"
      "    subb a, r7                           \n"
      "    jc skip_0us$                         \n"
      "    ret                                  \n" // return if 0 us,about
                                                    // 0.85us total
      "    nop                                  \n"
      "skip_0us$:                               \n"
      "    clr  c                               \n"
      "    mov  a, #0x01                        \n"
      "    subb a, r6                           \n"
      "    mov  r6, a                           \n"
      "    mov  a, #0x00                        \n"
      "    subb a, r7                           \n"
      "    mov  r7, a                           \n"
      "    jc skip_1us$                         \n"
      "    ret                                  \n" // return if 1 us,
                                                    // about 1.25 us total
      "    nop                                  \n" // adjust alignment

      "loop32m_us$:                             \n" // about nus
      "    nop \n nop \n nop \n nop \n nop \n    "  // 6+19 cycle
      "    nop \n "
      "loop32m_us_2$:                          \n" // need more test
      "    nop \n nop \n nop \n nop \n nop \n    "
      "skip_1us$:                              \n" // 1st loop shorter than
                                                   // others
      "    nop \n nop \n nop \n nop \n nop \n    "
      "    nop \n nop \n nop \n nop \n nop \n    "
      "    nop \n nop \n nop \n nop \n           "
      "    inc  r6                              \n" // 1 cycle
      "    cjne r6, #0,loop32m_us$              \n" // 6 cycle
      "    inc  r7                              \n" // there will be extra 1
                                                    // cycles for every 256us
      "    cjne r7, #0,loop32m_us_2$            \n"
      "    nop                                  \n");
#elif f_cpu >= 24000000ul
  __asm__(
      ".even                                    \n"
      "    mov  r6, dpl                         \n" // low 8-bit
      "    mov  r7, dph                         \n" // high 8-bit
      "    clr  c                               \n"
      "    mov  a,#0x01                         \n"
      "    subb a, r6                           \n"
      "    clr  a                               \n"
      "    subb a, r7                           \n"
      "    jc skip_0us$                         \n"
      "    ret                                  \n" // return if 0 1 us
                                                    // about 1.2us total
      "    nop                                  \n"
      "skip_0us$:                               \n"
      "    clr  c                               \n" // do some loop init, not
                                                    // useful for 2us but better
                                                    // here
      "    mov  a, #0x02                        \n"
      "    subb a, r6                           \n"
      "    mov  r6, a                           \n"
      "    mov  a, #0x00                        \n"
      "    subb a, r7                           \n"
      "    mov  r7, a                           \n"

      "    nop                                  \n" // keep even
      "    cjne r6,#0x00,loop24m_us$            \n"
      "    cjne r7,#0x00,loop24m_us$            \n"
      "    nop                                  \n"
      "    ret                                  \n" // return if 2us  about 2 us
                                                    // total

      "loop24m_us$:                             \n" // about nus

      "    nop \n nop \n nop \n nop \n nop \n    " // 6+11 cycle
      "    nop \n "
      "loop24m_us_2$:                          \n" // need more test

      "    nop \n nop \n nop \n nop \n nop \n    "
      "    nop \n nop \n nop \n nop \n nop \n    "
      "    nop \n                                "

      "    inc  r6                              \n" // 1 cycle
      "    cjne r6, #0,loop24m_us$              \n" // 6 cycle
      "    inc  r7                              \n" // there will be extra 1
                                                    // cycles for every 256us
      "    cjne r7, #0,loop24m_us_2$            \n"
      "    nop                                  \n");
#elif f_cpu >= 16000000ul
  __asm__(
      ".even                                    \n"
      "    mov  r6, dpl                         \n" // low 8-bit
      "    mov  r7, dph                         \n" // high 8-bit
      "    clr  c                               \n"
      "    mov  a,#0x01                         \n"
      "    subb a, r6                           \n"
      "    clr  a                               \n"
      "    subb a, r7                           \n"
      "    jc skip_0us$                         \n"
      "    ret                                  \n" // return if 0 1 us
                                                    // about 1.7us total
      "    nop                                  \n"
      "skip_0us$:                               \n"
      "    cjne r7,#0x00,skip_2us$              \n"
      "    cjne r6,#0x02,skip_2us$              \n"
      "    ret                                  \n" // return if 2us about 2.3us
                                                    // total
      "    nop                                  \n" // keep even
      "skip_2us$:                               \n"

      "    clr  c                               \n" // do some loop init, not
                                                    // useful for 3us but better
                                                    // here
      "    mov  a, #0x03                        \n"
      "    subb a, r6                           \n"
      "    mov  r6, a                           \n"
      "    mov  a, #0x00                        \n"
      "    subb a, r7                           \n"
      "    mov  r7, a                           \n"

      "    cjne r6,#0x00,loop16m_us$            \n"
      "    cjne r7,#0x00,loop16m_us$            \n"
      "    ret                                  \n" // return if 3us  about 3.4
                                                    // us total

      "loop16m_us$:                             \n" // about n.5us

      "    nop \n nop \n nop \n nop \n nop \n    " // 6+3 cycle
      "    nop \n                                "
      "loop16m_us_2$:                          \n"
      "    nop \n nop \n nop \n                  "

      "    inc  r6                              \n" // 1 cycle
      "    cjne r6, #0,loop16m_us$              \n" // 6 cycle
      "    inc  r7                              \n" // there will be extra 1
                                                    // cycles for every 256us
      "    cjne r7, #0,loop16m_us_2$            \n");
#elif f_cpu >= 12000000ul
  __asm__(
      ".even                                    \n"
      "    mov  r6, dpl                         \n" // low 8-bit
      "    mov  r7, dph                         \n" // high 8-bit
      "    clr  c                               \n"
      "    mov  a,#0x02                         \n"
      "    subb a, r6                           \n"
      "    clr  a                               \n"
      "    subb a, r7                           \n"
      "    jc skip_0us$                         \n"
      "    ret                                  \n" // return if 0 1 2 us
                                                    // about 2.25us total
      "    nop                                  \n"
      "skip_0us$:                               \n"
      "    clr  c                               \n" // do some loop init, not
                                                    // useful for 3us but better
                                                    // here
      "    mov  a, #0x03                        \n"
      "    subb a, r6                           \n"
      "    mov  r6, a                           \n"
      "    mov  a, #0x00                        \n"
      "    subb a, r7                           \n"
      "    mov  r7, a                           \n"

      "    nop                                  \n" // keep even
      "    cjne r6,#0x00,loop12m_us$            \n"
      "    cjne r7,#0x00,loop12m_us$            \n"
      "    nop                                  \n"
      "    ret                                  \n" // return if 3us  about 2 us
                                                    // total

      "loop12m_us$:                             \n" // about nus

      "    nop \n nop \n nop \n nop \n nop \n    " //
      "    nop \n "
      "loop12m_us_2$:                          \n"  // need more test
      "    inc  r6                              \n" // 1 cycle
      "    cjne r6, #0,loop12m_us$              \n" // 6 cycle
      "    inc  r7                              \n" // there will be extra 1
                                                    // cycles for every 256us
      "    cjne r7, #0,loop12m_us_2$            \n"
      "    nop                                  \n");
#else
#error "clock not supported yet"

#endif

  // return takes 5~6 cycles, depending on even or odd address
}

void init() {

#if f_ext_osc > 0
  // switch to external osc
  safe_mod = 0x55;
  safe_mod = 0xaa;
  clock_cfg |= bosc_en_xt;
  delaymicroseconds(10000);
  safe_mod = 0x55;
  safe_mod = 0xaa;
  clock_cfg &= ~bosc_en_int;
  safe_mod = 0x00;
#endif

  // set internal clock
  safe_mod = 0x55;
  safe_mod = 0xaa;

#if defined(ch551) || defined(ch552)
#if f_cpu == 32000000
  clock_cfg = clock_cfg & ~mask_sys_ck_sel | 0x07; // 32mhz
#elif f_cpu == 24000000
  clock_cfg = clock_cfg & ~mask_sys_ck_sel | 0x06; // 24mhz
#elif f_cpu == 16000000
  clock_cfg = clock_cfg & ~mask_sys_ck_sel | 0x05; // 16mhz
#elif f_cpu == 12000000
  clock_cfg = clock_cfg & ~mask_sys_ck_sel | 0x04; // 12mhz
#elif f_cpu == 6000000
  clock_cfg = clock_cfg & ~mask_sys_ck_sel | 0x03; // 6mhz
#elif f_cpu == 3000000
  clock_cfg = clock_cfg & ~mask_sys_ck_sel | 0x02; // 3mhz
#elif f_cpu == 750000
  clock_cfg = clock_cfg & ~mask_sys_ck_sel | 0x01; // 750khz
#elif f_cpu == 187500
  clock_cfg = clock_cfg & ~mask_sys_ck_sel | 0x00; // 187.5khz
#else
#warning f_cpu invalid or not set
#endif

#elif defined(ch549)
#if f_cpu == 48000000
  clock_cfg = clock_cfg & ~mask_sys_ck_sel | 0x07; // 48mhz
#elif f_cpu == 32000000
  clock_cfg = clock_cfg & ~mask_sys_ck_sel | 0x06; // 32mhz
#elif f_cpu == 24000000
  clock_cfg = clock_cfg & ~mask_sys_ck_sel | 0x05; // 24mhz
#elif f_cpu == 16000000
  clock_cfg = clock_cfg & ~mask_sys_ck_sel | 0x04; // 16mhz
#elif f_cpu == 12000000
  clock_cfg = clock_cfg & ~mask_sys_ck_sel | 0x03; // 12mhz
#elif f_cpu == 3000000
  clock_cfg = clock_cfg & ~mask_sys_ck_sel | 0x02; // 3mhz
#elif f_cpu == 750000
  clock_cfg = clock_cfg & ~mask_sys_ck_sel | 0x01; // 750khz
#elif f_cpu == 187500
  clock_cfg = clock_cfg & ~mask_sys_ck_sel | 0x00; // 187.5khz
#else
#warning f_cpu invalid or not set
#endif

#elif defined(ch559)
#if f_cpu == 24000000
  clock_cfg =
      clock_cfg & ~mask_sys_ck_div | 12; // 24mhz, 12m*(24 default pll)/12=24m
#elif f_cpu == 16000000
  clock_cfg = clock_cfg & ~mask_sys_ck_div | 18; // 16mhz
#elif f_cpu == 56000000
  pll_cfg = (7 << 5) | (28);                    // fusb4x = 12m*(28 pll)/7=48m
  clock_cfg = clock_cfg & ~mask_sys_ck_div | 6; // 56mhz, 12m*(28 pll)/6=56m
#else
#warning f_cpu invalid or not set
#endif

#endif

  safe_mod = 0x00;

  delaymicroseconds(5000); // needed to stablize internal rc

#ifndef user_usb_ram
  // init usb
  usbdevicecfg();
  usbdeviceendpointcfg(); //????
  usbdeviceintcfg();      //?????
  uep0_t_len = 0;
  uep1_t_len = 0; //????????????
  uep2_t_len = 0; //????????????
#endif

  // init pwm
  pwm_ck_se = 93; // div by 94 for 1k freq on 24m clk
  pwm_ctrl = 0;

  // init t0 for millis
  tmod = (tmod & ~0x0f) | (bt0_m1); // mode 2 for autoreload
  t2mod = t2mod & ~bt0_clk;         // bt0_clk=0;clk div by 12
  th0 = 255 - t0_cycle + 1;
  tf0 = 0;
  et0 = 1;
  tr0 = 1;

  ea = 1; // millis and delay needs interrupt
}

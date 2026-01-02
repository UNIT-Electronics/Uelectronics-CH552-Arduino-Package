// clang-format off
#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
// clang-format on

// in sdcc, caller will save the registers (r0-r7)
// dpl, dph, b and acc are for parameter/return value passing
// at this moment, sendchardebug is not included in any header
// use pragma callee_saves when you declare sendchardebug
#pragma callee_saves sendchardebug
void sendchardebug(__data char c) // 8mbps under 24m clk
{
  c; // avoid unreferenced function argument warning
  // uint8_t interrupton = ea;
  // ea = 0;
  __asm__("  mov c,_ea         \n"
          "  clr a             \n"
          "  rlc a             \n"
          "  mov b,a           \n"
          "  clr _ea           \n");

  // using p1.4
  __asm__( // any branch will cause unpredictable timing due to code alignment
      "  mov a,dpl         \n" // the parameter of func

      "  clr c             \n"
      "  mov _p1_4,c       \n"
      "  rrc a             \n"
      "  mov _p1_4,c       \n"
      "  rrc a             \n"
      "  mov _p1_4,c       \n"
      "  rrc a             \n"
      "  mov _p1_4,c       \n"
      "  rrc a             \n"
      "  mov _p1_4,c       \n"
      "  rrc a             \n"
      "  mov _p1_4,c       \n"
      "  rrc a             \n"
      "  mov _p1_4,c       \n"
      "  rrc a             \n"
      "  mov _p1_4,c       \n"
      "  rrc a             \n"
      "  mov _p1_4,c       \n"
      "  setb c            \n"
      "  mov _p1_4,c       \n");
  // if (interrupton) ea = 1;

  __asm__("  mov a,b           \n"
          "  jz skipseteadebug$\n"
          "  setb _ea          \n"
          "skipseteadebug$:    \n");
}

/*
void    mdelayus( uint16_t n )
{
#ifdef    f_cpu
#if        f_cpu <= 6000000
    n >>= 2;
#endif
#if        f_cpu <= 3000000
    n >>= 2;
#endif
#if        f_cpu <= 750000
    n >>= 4;
#endif
#endif
    while ( n ) {  // total = 12~13 fsys cycles, 1us @fsys=12mhz
        ++ safe_mod;  // 2 fsys cycles, for higher fsys, add operation here
#ifdef    f_cpu
#if        f_cpu >= 14000000
        ++ safe_mod;
#endif
#if        f_cpu >= 16000000
        ++ safe_mod;
#endif
#if        f_cpu >= 18000000
        ++ safe_mod;
#endif
#if        f_cpu >= 20000000
        ++ safe_mod;
#endif
#if        f_cpu >= 22000000
        ++ safe_mod;
#endif
#if        f_cpu >= 24000000
        ++ safe_mod;
#endif
#if        f_cpu >= 26000000
        ++ safe_mod;
#endif
#if        f_cpu >= 28000000
        ++ safe_mod;
#endif
#if        f_cpu >= 30000000
        ++ safe_mod;
#endif
#if        f_cpu >= 32000000
        ++ safe_mod;
#endif
#endif
        -- n;
    }
}

void    mdelayms( uint16_t n ){
    while ( n ) {
#ifdef    delay_ms_hw
        while ( ( tkey_ctrl & btkc_if ) == 0 );
        while ( tkey_ctrl & btkc_if );
#else
        mdelayus( 1000 );
#endif
        -- n;
    }
}*/

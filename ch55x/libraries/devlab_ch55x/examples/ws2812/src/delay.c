// ===================================================================================
// delay functions for ch551, ch552 and ch554
// ===================================================================================

#include "delay.h"
#include "ch554.h"

// ===================================================================================
// delay in units of us
// ===================================================================================
void dly_us(uint16_t n) {           // delay in us
  #ifdef f_cpu
    #if f_cpu <= 6000000
      n >>= 2;
    #endif
    #if f_cpu <= 3000000
      n >>= 2;
    #endif
    #if f_cpu <= 750000
      n >>= 4;
    #endif
  #endif

  while(n) {                        // total = 12~13 fsys cycles, 1us @fsys=12mhz
    safe_mod++;                     // 2 fsys cycles, for higher fsys, add operation here
    #ifdef f_cpu
      #if f_cpu >= 14000000
        safe_mod++;
      #endif
      #if f_cpu >= 16000000
        safe_mod++;
      #endif
      #if f_cpu >= 18000000
        safe_mod++;
      #endif
      #if f_cpu >= 20000000
        safe_mod++;
      #endif
      #if f_cpu >= 22000000
        safe_mod++;
      #endif
      #if f_cpu >= 24000000
        safe_mod++;
      #endif
      #if f_cpu >= 26000000
        safe_mod++;
      #endif
      #if f_cpu >= 28000000
        safe_mod++;
      #endif
      #if f_cpu >= 30000000
        safe_mod++;
      #endif
      #if f_cpu >= 32000000
		    safe_mod++;
      #endif
    #endif
		n--;
  }
}

// ===================================================================================
// delay in units of ms
// ===================================================================================
void dly_ms(uint16_t n) {           // delay in ms
  while(n) {
    dly_us(1000);
    n--;
  }
}

// ===================================================================================
// delay 20+4*(n-1) clock cycles
// ===================================================================================
#pragma callee_saves _delay_more_cycles
void _delay_more_cycles (uint8_t n) __naked {
  n;              // stop unreferenced arg warning
  __asm
    .even         ; make predictable cycles for jumps
    push ar7      ; 2 cycles
    mov  r7, dpl  ; 2 cycles
    djnz r7, .+0  ; 2/4 cycles
    pop  ar7      ; 2 cycles
    ret           ; 4|5 cycles
  __endasm;
}

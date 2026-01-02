// ===================================================================================
// libraries, definitions and macros
// ===================================================================================
// code rewrite by master
// -------                                 ch552
//                                       +--\/--+
//           din  ---------------- p32  1|°     |16  v33 -------------
//                ---------------- p14  2|      |15  vcc -------------
//                ---------------- p15  3|      |14  gnd -------------
//                ---------------- p16  4|      |13  p37 udm --- usb d-
//                ---------------- p17  5|      |12  p36 udp --- usb d+
//                ---------------- rst  6|      |11  p34 -------------
//                ---------------- p31  7|      |10  p33 -------------
//                ---------------- p30  8|      |9   p11 -------------
//                                       +------+

// libraries


#include "src/system.h"   // system functions
#include "src/delay.h"    // delay functions
#include "src/neo.h"      // neopixel functions
#include <stdlib.h>       // for random number generation

// ===================================================================================
// neopixel functions
// ===================================================================================

// pin definitions
#define pin_neo             p33       // pin connected to neopixel
// 



#define delay 100
#define neopixel 16 // number neopixel conect
#define level 100 // ilumination level 0 to 255

void randomcolorsequence(void) {

  for(int j=0;j<neopixel;j++){
      uint8_t red = rand() % level;
      uint8_t green = rand() % level;
      uint8_t blue = rand() % level;
      uint8_t num = rand() % neopixel;

      for(int i=0; i<num; i++){
        neo_writecolor(0, 0, 0);
      }
      neo_writecolor(red, green, blue);
      dly_ms(delay);
      neo_writecolor(0, 0, 0);
    }
    
    for(int l=0; l<9; l++){
        neo_writecolor(0, 0, 0);
      }

}

void colorsequence(void) {
 
  for(int j=0;j<=neopixel;j++){
        uint8_t red = rand() % level;
        uint8_t green = rand() % level;
        uint8_t blue = rand() % level;
      for(int i=0; i<j; i++){
        neo_writecolor(red, green, blue);
      }
      dly_ms(delay);
      for(int l=0; l<j; l++){
        neo_writecolor(0, 0, 0);
      }
  }
}

// ===================================================================================
// main function
// ===================================================================================
void main(void) {
  neo_init();                       // init neopixels
  clk_config();                     // configure system clock
  dly_ms(delay);                       // wait for clock to settle

  // loop
  while (1) {
    randomcolorsequence();
    dly_ms(100);
    colorsequence();
    dly_ms(100);
  }
}

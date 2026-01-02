/*
 created by deqing sun for use with ch55xduino
 */

// clang-format off
#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
// clang-format on

void eeprom_write_byte(__data uint8_t addr, __xdata uint8_t val) {

#if defined(ch551) || defined(ch552)

  if (addr >= 128) {
    return;
  }

  safe_mod = 0x55;
  safe_mod = 0xaa;        // enter safe mode
  global_cfg |= bdata_we; // enable dataflash write
  safe_mod = 0;           // exit safe mode
  rom_addr_h = data_flash_addr >> 8;
  rom_addr_l = addr << 1;
  rom_data_l = val;
  if (rom_status & brom_addr_ok) { // valid access address
    rom_ctrl = rom_cmd_write;      // write
  }
  safe_mod = 0x55;
  safe_mod = 0xaa;         // enter safe mode
  global_cfg &= ~bdata_we; // disable dataflash write
  safe_mod = 0;            // exit safe mode

#else
  addr; // tbd
  val;
  return;
#endif
}

uint8_t eeprom_read_byte(__data uint8_t addr) {
#if defined(ch551) || defined(ch552)

  rom_addr_h = data_flash_addr >> 8;
  rom_addr_l = addr << 1; // addr must be even
  rom_ctrl = rom_cmd_read;
  return rom_data_l;
#else
  addr; // tbd
  return 0;
#endif
}

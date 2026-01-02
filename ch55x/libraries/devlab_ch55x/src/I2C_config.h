// ===================================================================================
// I2C Pin Configuration for CH552
// ===================================================================================
// User configurations for I2C pins
// ===================================================================================

#pragma once

// Default I2C pin definitions for CH552
// User can override these in their sketch before including i2c.h
#ifndef PIN_SDA
  #define PIN_SDA  16  // P1.6 - I2C SDA pin
#endif

#ifndef PIN_SCL
  #define PIN_SCL  15  // P1.7 - I2C SCL pin
#endif

# Devlab CH55x Library

Complete Arduino library for CH55x microcontrollers with comprehensive examples and utilities.

## Features

### I2C Communication (Software Bit-Bang)
- Full I2C master implementation with bit-banging
- ACK/NACK detection for reliable device scanning
- Support for 400kHz I2C devices
- Automatic frequency adjustment based on CPU clock
- Default pins: P1.6 (SDA), P1.7 (SCL)

### Servo Control
- PWM-based servo motor control
- Support for multiple servos
- Microsecond and degree control

### PWM Control
- Hardware PWM on CH552 pins
- PWM1 output on P1.5 (or P3.0 with pin remapping)
- PWM2 output on P1.4 (or P3.1 with pin remapping)
- Configurable duty cycle and frequency

## Library Usage

### Simple Setup (Recommended)
```cpp
#include <devlab_ch55x.h>  // Includes all library components

void setup() {
  I2C_init();  // Initialize I2C with default pins
}
```

### Advanced Setup - Individual Components
If you only need specific functionality, you can include individual headers:
```cpp
#include <i2c.h>     // Only I2C
#include <oled.h>    // Only OLED
#include <pwm.h>     // Only PWM
#include <Servo.h>   // Only Servo
```

### PWM Usage
```cpp
#include <devlab_ch55x.h>

void setup() {
  SetPWMClk(4);           // Set PWM clock divider
  PWM1OutEnable();        // Enable PWM1 output on P1.5
  PWM1OutPolarHighAct();  // Active high
  SetPWM1Dat(128);        // 50% duty cycle (0-255)
}
```

### Custom Pin Configuration
```cpp
// Define custom pins BEFORE including the library
#define PIN_SDA 16  // Your custom SDA pin
#define PIN_SCL 15  // Your custom SCL pin
#include <devlab_ch55x.h>
```

### Scanning for I2C Devices
```cpp
#include <devlab_ch55x.h>

void setup() {
  I2C_init();
}

void loop() {
  for(uint8_t addr = 1; addr < 127; addr++) {
    if (I2C_probe(addr)) {
      // Device found at address 'addr'
    }
  }
  delay(5000);
}
```

### Writing to I2C Device
```cpp
I2C_start(0x3C << 1);  // Start communication (write mode)
I2C_write(0x00);        // Send command byte
I2C_write(0xAF);        // Send data byte
I2C_stop();             // End communication
```

### Reading from I2C Device
```cpp
I2C_start((0x3C << 1) | 1);  // Start communication (read mode)
uint8_t data = I2C_read(0);   // Read byte (send NACK)
I2C_stop();                   // End communication
```

## API Reference

### I2C Functions
- `void I2C_init(void)` - Initialize I2C pins
- `void I2C_start(uint8_t addr)` - Start I2C transmission
- `void I2C_stop(void)` - Stop I2C transmission
- `void I2C_write(uint8_t data)` - Write byte to I2C bus
- `uint8_t I2C_read(uint8_t ack)` - Read byte from I2C bus
- `uint8_t I2C_probe(uint8_t addr)` - Probe for device (returns 1 if ACK)

### PWM Functions
- `SetPWMClk(divider)` - Set PWM clock divider
- `SetPWM1Dat(duty)` - Set PWM1 duty cycle (0-255)
- `SetPWM2Dat(duty)` - Set PWM2 duty cycle (0-255)
- `PWM1OutEnable()` - Enable PWM1 output
- `PWM2OutEnable()` - Enable PWM2 output
- `PWM1PinAlter()` - Map PWM1 to P3.0
- `PWM2PinAlter()` - Map PWM2 to P3.1

## Examples

See the `examples` folder for complete working examples:
- `i2c_scanner` - Scan I2C bus for devices
- `oled_display` - SSD1306 OLED display control
- `blink` - Basic LED blink with serial output
- `adc` - Analog input reading
- `pwm` - PWM LED fading
- `input` - Digital input with button

## Hardware Requirements

### I2C
- External pull-up resistors (4.7kΩ - 10kΩ) on SDA and SCL lines are mandatory
- Default pins: P1.6 (SDA), P1.7 (SCL)

## Version History

- **v1.1.0** - Added I2C library with ACK detection and probe function
- **v1.0.0** - Initial release with Servo support

## License

Mixed licensing:
- Original I2C code: CC BY-SA 3.0
- Modifications: GPL v3.0
- Examples: Public Domain / MIT

## Credits

- Based on Stefan Wagner's I2C implementation
- Modified by Unit Electronics for Devlab CH55x

## Links

- GitHub: https://github.com/UNIT-Electronics
- Documentation: https://uelectronics.com/

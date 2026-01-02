# Devlab_CH55x - CH55x Arduino Core

Welcome to Devlab_CH55x! This package provides a standardized and optimized Arduino core for the CH55x microcontroller. Based on the excellent [CH55xduino](https://github.com/DeqingSun/ch55xduino/tree/ch55xduino) project by [DeqingSun](https://github.com/DeqingSun), this version features code standardization with all filenames and code in lowercase for consistency across platforms.

## Supported Boards
- CH552

## Installing with Arduino Boards Manager
---
To install, follow these steps:

1. Open the Arduino IDE and navigate to File -> Preferences.
2. In the dialog that appears, enter the following URL into the "Additional Boards Manager URLs" field: 

   > https://raw.githubusercontent.com/UNIT-Electronics/Uelectronics-CH552-Arduino-Package/refs/heads/main/package_duino_mcs51_index.json
   
   ![Adding URL](./images/board_json.png)

3. Click OK to close the dialog.
4. Proceed to Tools -> Boards -> Boards Manager in the IDE.
5. Type "CH" in the search box and select "Install".
   
   ![Boards Manager](./images/manager.png)

## Code Standardization
This release (v0.0.5) includes significant improvements:
- **Lowercase File Naming**: All source files (*.c, *.ino, Makefile) are now in lowercase for cross-platform compatibility
- **Consistent Code Style**: Code content standardized while preserving comments and preprocessor directives
- **Unified Library Structure**: All examples consolidated under `devlab_ch55x` library
- **Examples Included**: ADC, Blink, PWM, Timer, USB Serial, WS2812 NeoPixel, OLED displays, and more

## License
This package is distributed under the GNU Lesser General Public License, Version 2.1, February 1999.

## Contributing
If you wish to contribute or have bug fixes, please open an issue or pull request 

## Licensing and Credits
* The [Arduino IDE and ArduinoCore-API](https://arduino.cc) are developed and maintained by the Arduino team and are licensed under GPL.
* The [ch55xduino](https://github.com/DeqingSun/ch55xduino/tree/ch55xduino) project serves as the foundation for this repository, with updates provided by its maintainer.

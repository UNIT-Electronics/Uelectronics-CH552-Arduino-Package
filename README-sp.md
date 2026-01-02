# Devlab_CH55x - Núcleo de Arduino para CH55x

¡Bienvenido a Devlab_CH55x! Este paquete proporciona un núcleo de Arduino estandarizado y optimizado para el microcontrolador CH55x. Basado en el excelente proyecto [CH55xduino](https://github.com/DeqingSun/ch55xduino/tree/ch55xduino) de [DeqingSun](https://github.com/DeqingSun), esta versión incluye estandarización de código con todos los nombres de archivo y código en minúsculas para consistencia entre plataformas.

## Placas Compatibles
- CH552

## Instalación con el Administrador de Placas de Arduino
---
Para instalar, sigue estos pasos:

1. Abre el IDE de Arduino y navega a Archivo -> Preferencias.
2. En el cuadro de diálogo que aparece, ingresa la siguiente URL en el campo "URLs adicionales del Administrador de placas":

   > https://raw.githubusercontent.com/UNIT-Electronics/Uelectronics-CH552-Arduino-Package/refs/heads/main/package_duino_mcs51_index.json

   ![Añadir URL](./images/board_json.png)

3. Haz clic en OK para cerrar el cuadro de diálogo.
4. Ve a Herramientas -> Placas -> Administrador de placas en el IDE.
5. Escribe "CH" en el cuadro de búsqueda y selecciona "Instalar".
   
   ![Administrador de Placas](./images/manager.png)

## Estandarización de Código
Esta versión (v0.0.5) incluye mejoras significativas:
- **Nombres de Archivo en Minúsculas**: Todos los archivos fuente (*.c, *.ino, makefile) ahora están en minúsculas para compatibilidad multiplataforma
- **Estilo de Código Consistente**: Contenido de código estandarizado preservando comentarios y directivas de preprocesador
- **Estructura de Biblioteca Unificada**: Todos los ejemplos consolidados bajo la biblioteca `devlab_ch55x`
- **Ejemplos Incluidos**: ADC, Blink, PWM, Timer, USB Serial, WS2812 NeoPixel, pantallas OLED, y más

## Licencia
Este paquete se distribuye bajo la Licencia Pública General Menor de GNU, Versión 2.1, febrero de 1999.

## Contribuciones
Si deseas contribuir o tienes correcciones de errores, por favor abre un problema o una solicitud de extracción.

## Licencias y Créditos
* El [IDE de Arduino y ArduinoCore-API](https://arduino.cc) son desarrollados y mantenidos por el equipo de Arduino y están licenciados bajo GPL.
* El proyecto [ch55xduino](https://github.com/DeqingSun/ch55xduino/tree/ch55xduino) sirve como base para este repositorio, con actualizaciones proporcionadas por su mantenedor.
* Versión estandarizada y optimizada por [Unit Electronics](https://uelectronics.com/).

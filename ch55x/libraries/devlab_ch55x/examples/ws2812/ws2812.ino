// ================================================== =====================================
// contenedor ide de arduino para ch55xduino
// ================================================== =====================================
//
// instrucciones de compilación para el ide de arduino:
//----------------------------------------------------
// - asegúrate de haber instalado ch55xduino: https://github.com/deqingsun/ch55xduino
// - copie los archivos .ino y .c, así como la carpeta /src, juntos en una sola carpeta
// y asígnale el nombre del archivo .ino. abra el archivo .ino en el ide de arduino. ir a
// "herramientas -> placa -> placas ch55x -> placa ch552". en "herramientas", seleccione la
// siguientes opciones:
// - fuente de reloj: 16 mhz (interno)
// - método de carga: usb
// - configuración usb: cÓdigo de usuario /w 266b usb ram
// - presiona el botón boot en la placa y mantenlo presionado mientras la conectas vía usb
// con tu pc.
// - haga clic en "cargar" inmediatamente después.



unsigned char _sdcc_external_startup (void) __nonbanked {
  return 0;
}

# Release v0.0.6 — 2026-06-21

Paquete Arduino para microcontroladores **CH55x** de Unit Electronics.
Esta versión actualiza la cadena de herramientas (tools) y mejora el flujo
de empaquetado.

---

## Cambios principales

### Herramientas (tools)
- Renombrada la dependencia de tools `MCS51Tools` → **`devlabtools`** en
  `package_duino_mcs51_index.json` (todas las versiones `0.0.1` – `0.0.5`
  y el bloque principal `tools`).
- Renombrada la carpeta local de tools para Windows 32 a la versión actual:
  `tools/ch55xduino-tools_mingw32-2023.10.10` → `tools/ch55xduino-tools_mingw32-2026.06.21`.
- Regenerado el archivo comprimido de tools para `i686-mingw32`.

### Core / plataforma
- `ch55x/platform.txt`: corregida la variable `compiler.path.wrapper`
  → `compiler.wrapper.path` en la regla `recipe.preproc.includes` para
  evitar fallos en la fase de preprocesado de includes.

### Scripts
- `compress.py` actualizado a **v2.0**:
  - Comprime simultáneamente el paquete `ch55x` y la carpeta
    `tools/ch55xduino-tools_mingw32-*` más reciente.
  - Genera un único `file_info.txt` con tamaño, tipo y SHA-256 de cada
    archivo.
  - Detecta automáticamente la versión por nombre de carpeta.

### Repositorio
- Nuevo `.gitignore` para descartar archivos `.tar*` dentro de `tools/`
  (evita versionar binarios pesados de tools).

---

## Archivos del release

| Archivo | Tamaño | SHA-256 |
|---|---|---|
| `devlab_ch55x_version_2026-06-21.tar.bz2` | 99 218 bytes | `c6f6eaa35ecf342479f06708368b156406cf24e2ba7730991538bff1951968ed` |
| `ch55xduino-tools_mingw32-2026.06.21.tar.bz2` | 696 737 bytes | `bde033034222917c6dd5029f1bb2c55f353cc02710c4f2786d5e20b7eee2f7ed` |

> Los valores anteriores se generan automáticamente con `python3 compress.py`
> y se guardan también en `file_info.txt`.

---

## Cómo actualizar `package_duino_mcs51_index.json`

Para registrar esta versión como `0.0.6` en el Board Manager, añadir un
bloque dentro de `platforms`:

```json
{
    "name": "Devlab_CH55x",
    "architecture": "mcs51",
    "version": "0.0.6",
    "category": "Unit Electronics",
    "help": {
        "online": "https://github.com/UNIT-Electronics/Uelectronics-CH552-Arduino-Package"
    },
    "boards": [
        {"name": "CH552 Board"}
    ],
    "toolsDependencies": [
        {
            "name": "devlabtools",
            "version": "2026.06.21",
            "packager": "Uelectronics"
        },
        {
            "name": "sdcc",
            "version": "build.13407_4",
            "packager": "Uelectronics"
        }
    ],
    "url": "https://github.com/UNIT-Electronics/Uelectronics-CH552-Arduino-Package/releases/download/v0.0.6/devlab_ch55x_version_2026-06-21.tar.bz2",
    "archiveFileName": "devlab_ch55x_version_2026-06-21.tar.bz2",
    "checksum": "SHA-256:c6f6eaa35ecf342479f06708368b156406cf24e2ba7730991538bff1951968ed",
    "size": "99218"
}
```

Y un nuevo `system` en el bloque `tools` → `devlabtools` (o un nuevo bloque
con versión `2026.06.21`):

```json
{
    "host": "i686-mingw32",
    "url": "https://github.com/UNIT-Electronics/Uelectronics-CH552-Arduino-Package/releases/download/v0.0.6/ch55xduino-tools_mingw32-2026.06.21.tar.bz2",
    "archiveFileName": "ch55xduino-tools_mingw32-2026.06.21.tar.bz2",
    "checksum": "SHA-256:bde033034222917c6dd5029f1bb2c55f353cc02710c4f2786d5e20b7eee2f7ed",
    "size": "696737"
}
```

---

## Instalación rápida

1. Arduino IDE → **Preferences** → *Additional Boards Manager URLs*:
   ```
   https://github.com/UNIT-Electronics/Uelectronics-CH552-Arduino-Package/raw/main/package_duino_mcs51_index.json
   ```
2. **Boards Manager** → buscar *Devlab_CH55x* → instalar `0.0.6`.

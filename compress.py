#!/usr/bin/env python3
# compress.py

"""
Descripción: Genera archivos comprimidos tar.bz2 del paquete ch55x y de la
carpeta de tools (mingw32), y guarda el tamaño y el SHA-256 de cada uno en
un archivo de texto compartido.
Autor: Cesar
Fecha de creación: 01/04/2024
Versión: 2.0
Dependencias: tarfile, os, hashlib, datetime, glob
"""

import tarfile
import os
import hashlib
import datetime
import glob

REPO_ROOT = os.path.dirname(os.path.abspath(__file__))
TODAY = datetime.datetime.now().strftime("%Y-%m-%d")
INFO_FILE = os.path.join(REPO_ROOT, "file_info.txt")


def compress_folder(folder_path, output_filename, arcname=None):
    """Comprime folder_path en output_filename. arcname controla el nombre
    del directorio raíz dentro del tar (por defecto, el basename de folder_path)."""
    if arcname is None:
        arcname = os.path.basename(folder_path)
    with tarfile.open(output_filename, "w:bz2") as tar:
        tar.add(folder_path, arcname=arcname)


def calculate_sha256(file_path):
    sha256_hash = hashlib.sha256()
    with open(file_path, "rb") as f:
        for byte_block in iter(lambda: f.read(4096), b""):
            sha256_hash.update(byte_block)
    return sha256_hash.hexdigest()


def build_entry(label, archive_path):
    size = os.path.getsize(archive_path)
    sha = calculate_sha256(archive_path)
    return (
        f"=== {label} ===\n"
        f"Nombre del archivo comprimido: {os.path.basename(archive_path)}\n"
        f"Ruta: {os.path.relpath(archive_path, REPO_ROOT)}\n"
        f"Tamaño: {size} bytes\n"
        f"Tipo: {archive_path.split('.')[-1]}\n"
        f"SHA-256: {sha}\n\n"
    )


def find_tools_folder():
    """Busca la carpeta de tools mingw32 más reciente dentro de ./tools/."""
    pattern = os.path.join(REPO_ROOT, "tools", "ch55xduino-tools_mingw32-*")
    matches = [p for p in glob.glob(pattern) if os.path.isdir(p)]
    if not matches:
        return None
    matches.sort()
    return matches[-1]


def main():
    entries = []

    # 1) Paquete ch55x (core Arduino)
    ch55x_folder = os.path.join(REPO_ROOT, "ch55x")
    if os.path.isdir(ch55x_folder):
        ch55x_archive = os.path.join(
            REPO_ROOT, f"devlab_ch55x_version_{TODAY}.tar.bz2"
        )
        print(f"[ch55x] Comprimiendo {ch55x_folder} -> {ch55x_archive}")
        compress_folder(ch55x_folder, ch55x_archive)
        entries.append(build_entry("Paquete ch55x", ch55x_archive))
    else:
        print(f"[ch55x] Carpeta no encontrada: {ch55x_folder}")

    # 2) Tools mingw32 (Windows 32)
    #
    # Estructura requerida (igual que los archivos originales de DeqingSun):
    #   tools/
    #     win/...
    #     wrapper/...
    #
    # La carpeta local es <repo>/tools/ch55xduino-tools_mingw32-<ver>/tools/...
    # así que empaquetamos el subdirectorio "tools" usando arcname="tools".
    tools_folder = find_tools_folder()
    if tools_folder:
        version_tag = os.path.basename(tools_folder).split("-")[-1]
        inner_tools = os.path.join(tools_folder, "tools")
        if not os.path.isdir(inner_tools):
            print(f"[tools] Subcarpeta esperada no encontrada: {inner_tools}")
        else:
            tools_archive = os.path.join(
                REPO_ROOT,
                "tools",
                f"ch55xduino-tools_mingw32-{version_tag}.tar.bz2",
            )
            print(f"[tools] Comprimiendo {inner_tools} -> {tools_archive}")
            compress_folder(inner_tools, tools_archive, arcname="tools")
            entries.append(build_entry("Tools mingw32 (Windows 32)", tools_archive))
    else:
        print("[tools] No se encontró carpeta ch55xduino-tools_mingw32-* en ./tools/")

    # 3) Guardar info combinada
    if entries:
        header = (
            f"Información de archivos comprimidos\n"
            f"Generado: {datetime.datetime.now().isoformat(timespec='seconds')}\n\n"
        )
        with open(INFO_FILE, "w") as f:
            f.write(header)
            f.writelines(entries)
        print(f"\nInformación guardada en {INFO_FILE}")
    else:
        print("\nNo se generó ningún archivo comprimido.")


if __name__ == "__main__":
    main()

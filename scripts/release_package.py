#!/usr/bin/env python3
"""Build a Boards Manager archive and append its release metadata."""

from __future__ import annotations

import argparse
import copy
import hashlib
import json
import re
import tarfile
from pathlib import Path


VERSION_RE = re.compile(r"^(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)$")


def version_key(version: str) -> tuple[int, int, int]:
    return tuple(int(part) for part in version.split("."))  # type: ignore[return-value]


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as source:
        for chunk in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def reproducible_tarinfo(info: tarfile.TarInfo) -> tarfile.TarInfo:
    info.uid = 0
    info.gid = 0
    info.uname = "root"
    info.gname = "root"
    info.mtime = 0
    return info


def create_archive(source_dir: Path, archive: Path) -> None:
    archive.parent.mkdir(parents=True, exist_ok=True)
    with tarfile.open(archive, "w:bz2") as output:
        output.add(
            source_dir,
            arcname=source_dir.name,
            recursive=True,
            filter=reproducible_tarinfo,
        )


def update_index(
    index_path: Path,
    version: str,
    tag: str,
    archive_name: str,
    archive_size: int,
    archive_sha256: str,
    repository: str,
) -> None:
    index = json.loads(index_path.read_text(encoding="utf-8"))
    package = next(
        package
        for package in index["packages"]
        if package["name"] == "Uelectronics"
    )
    platforms = package["platforms"]

    if any(platform["version"] == version for platform in platforms):
        raise SystemExit(f"Platform version {version} already exists in {index_path}")

    latest = max(platforms, key=lambda platform: version_key(platform["version"]))
    entry = copy.deepcopy(latest)
    entry["version"] = version
    entry["url"] = (
        f"https://github.com/{repository}/releases/download/{tag}/{archive_name}"
    )
    entry["archiveFileName"] = archive_name
    entry["checksum"] = f"SHA-256:{archive_sha256}"
    entry["size"] = str(archive_size)
    platforms.append(entry)

    index_path.write_text(
        json.dumps(index, indent=4, ensure_ascii=False) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--version", required=True)
    parser.add_argument("--tag", required=True)
    parser.add_argument("--repository", required=True)
    parser.add_argument("--source", type=Path, default=Path("ch55x"))
    parser.add_argument(
        "--index", type=Path, default=Path("package_duino_mcs51_index.json")
    )
    parser.add_argument("--output-dir", type=Path, default=Path("dist"))
    args = parser.parse_args()

    if not VERSION_RE.fullmatch(args.version):
        raise SystemExit("Version must use numeric semantic versioning, for example 0.0.6")
    if args.tag != f"v{args.version}":
        raise SystemExit("Tag must match the version as v<version>")
    if not args.source.is_dir():
        raise SystemExit(f"Source directory does not exist: {args.source}")
    if not args.index.is_file():
        raise SystemExit(f"Package index does not exist: {args.index}")

    archive_name = f"devlab_ch55x_version_{args.version}.tar.bz2"
    archive_path = args.output_dir / archive_name
    create_archive(args.source, archive_path)
    update_index(
        args.index,
        args.version,
        args.tag,
        archive_name,
        archive_path.stat().st_size,
        sha256(archive_path),
        args.repository,
    )
    print(f"archive={archive_path}")
    print(f"sha256={sha256(archive_path)}")
    print(f"size={archive_path.stat().st_size}")


if __name__ == "__main__":
    main()

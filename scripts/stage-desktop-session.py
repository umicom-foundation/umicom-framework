#!/usr/bin/env python3
"""Stage an opt-in Linux launcher without changing the running desktop.
Sammy Hegab, Umicom Foundation. MIT.

This is packaging tooling, not another runtime or process launcher. Runtime
checks and the handoff into Desk remain in the Framework's C23 session module.
No files are installed into /usr, ~/.config/autostart or a display manager.
"""
from __future__ import annotations
import argparse
import hashlib
import json
import os
from pathlib import Path
import stat
import sys

MAX_BINARY = 512 * 1024 * 1024
SCHEMA = "umicom.desktop-session-stage"

def absolute(path: str | Path) -> Path:
    p = Path(path)
    if not p.is_absolute() or any(ord(c) < 32 or ord(c) == 127 for c in str(p)):
        raise ValueError("Use an absolute path without control characters.")
    # Percent field substitution inside Exec arguments is intentionally excluded.
    if "%" in str(p):
        raise ValueError("A staged launcher path must not contain percent field codes.")
    return p

def read_regular(path: Path, limit: int) -> tuple[bytes, os.stat_result]:
    fd = os.open(path, os.O_RDONLY | getattr(os, "O_NOFOLLOW", 0) | getattr(os, "O_NONBLOCK", 0))
    try:
        info = os.fstat(fd)
        if not stat.S_ISREG(info.st_mode) or info.st_size > limit:
            raise ValueError("Input must be a bounded regular file.")
        with os.fdopen(fd, "rb", closefd=False) as stream:
            data = stream.read(limit + 1)
        if len(data) > limit:
            raise ValueError("Input exceeded its byte limit.")
        return data, info
    finally:
        os.close(fd)

def binary(path: Path) -> dict[str, str | int]:
    data, info = read_regular(path, MAX_BINARY)
    if (info.st_mode & (stat.S_IWGRP | stat.S_IWOTH | stat.S_ISUID | stat.S_ISGID)) or not info.st_mode & 0o111:
        raise ValueError("Executables must be non-set-id, executable, and not writable by group/others.")
    if hasattr(os, "getuid") and info.st_uid not in (0, os.getuid()):
        raise ValueError("Executables must belong to the current user or root.")
    if len(data) < 64 or data[:7] != b"\x7fELF\x02\x01\x01" or int.from_bytes(data[16:18], "little") not in (2, 3):
        raise ValueError("Select a 64-bit little-endian Linux ELF binary, not a Windows executable or script.")
    machine = int.from_bytes(data[18:20], "little")
    if machine not in (62, 183, 243):
        raise ValueError("Only x86-64, AArch64 and RISC-V 64 ELF files are recognised.")
    return {"path": str(path), "sha256": hashlib.sha256(data).hexdigest(), "bytes": len(data), "machine": machine}

def value(text: str) -> str:
    if any(ord(c) < 32 or ord(c) == 127 for c in text):
        raise ValueError("Desktop entry values may not contain control characters.")
    return text.replace("\\", "\\\\")

def argument(text: str) -> str:
    if "%" in text:
        raise ValueError("Percent field codes are not accepted.")
    quoted = '"' + "".join("\\" + c if c in '"`$\\' else c for c in text) + '"'
    # General key-value unescaping happens before Exec quoting is interpreted.
    return value(quoted)

def exclusive(path: Path, data: bytes, mode: int = 0o644) -> None:
    fd = os.open(path, os.O_WRONLY | os.O_CREAT | os.O_EXCL | getattr(os, "O_NOFOLLOW", 0), mode)
    try:
        with os.fdopen(fd, "wb", closefd=False) as stream:
            stream.write(data); stream.flush(); os.fsync(stream.fileno())
    finally:
        os.close(fd)

def stage(args: argparse.Namespace) -> None:
    desk, launcher, output, icon = map(absolute, (args.desk, args.launcher, args.output, args.icon))
    if "=" in str(launcher):
        raise ValueError("The launcher executable path cannot contain '=' in an Exec key.")
    records = {"desk": binary(desk), "launcher": binary(launcher)}
    if records["desk"]["machine"] != records["launcher"]["machine"]:
        raise ValueError("Desk and its launcher must target the same architecture.")
    icon_bytes, _ = read_regular(icon, 1024 * 1024)
    if not icon_bytes.startswith(b"\x89PNG\r\n\x1a\n"):
        raise ValueError("Select the Umicom PNG icon for this staged launcher.")
    # All inputs are validated before the only directory-creation operation.
    # A partial output is deliberately retained on failure, never removed.
    output.mkdir(mode=0o700, parents=False, exist_ok=False)
    icon_target = output / "umicom-icon.png"
    desktop = ("[Desktop Entry]\nType=Application\nVersion=1.0\nName=Umicom Desk session\n"
        "Comment=Open Umicom Desk in the current graphical session\n"
        f"Exec={argument(str(launcher))} --run --desk {argument(str(desk))}\n"
        f"TryExec={value(str(launcher))}\nIcon={value(str(icon_target))}\n"
        "Terminal=false\nDBusActivatable=false\nCategories=System;\nStartupNotify=false\n")
    exclusive(icon_target, icon_bytes)
    exclusive(output / "umicom-desk-session.desktop", desktop.encode("utf-8"))
    manifest = {"schema": SCHEMA, "root": str(output), "inputs": records,
        "files": {"umicom-icon.png": hashlib.sha256(icon_bytes).hexdigest(),
                  "umicom-desk-session.desktop": hashlib.sha256(desktop.encode()).hexdigest()},
        "scope": "References an existing Linux Desk installation; no compositor, login manager, autostart or boot image installed."}
    exclusive(output / "manifest.json", (json.dumps(manifest, indent=2, ensure_ascii=True) + "\n").encode())
    print(f"Staged only: {output}\nNo application was started and no desktop setting was changed.")

def unique_object(pairs: list[tuple[str, object]]) -> dict[str, object]:
    result: dict[str, object] = {}
    for key, value in pairs:
        if key in result:
            raise ValueError(f"Duplicate manifest key: {key}")
        result[key] = value
    return result

def verify(args: argparse.Namespace) -> None:
    output = absolute(args.output)
    data, _ = read_regular(output / "manifest.json", 16384)
    manifest = json.loads(data, object_pairs_hook=unique_object)
    if not isinstance(manifest, dict):
        raise ValueError("The manifest must contain an object.")
    if manifest.get("schema") != SCHEMA or manifest.get("root") != str(output):
        raise ValueError("Unknown manifest or moved stage. Create a new stage at its final review location.")
    if not isinstance(manifest.get("inputs"), dict) or not isinstance(manifest.get("files"), dict):
        raise ValueError("Input and file inventories must be objects.")
    if set(manifest.get("inputs", {})) != {"desk", "launcher"} or set(manifest.get("files", {})) != {"umicom-icon.png", "umicom-desk-session.desktop"}:
        raise ValueError("The manifest does not describe exactly the expected input and output files.")
    for record in manifest["inputs"].values():
        if not isinstance(record, dict) or not isinstance(record.get("path"), str):
            raise ValueError("Each input must be a record with an absolute path.")
        if binary(absolute(record["path"])) != record:
            raise ValueError("A referenced executable changed. Review the new build and create a new stage.")
    if manifest["inputs"]["desk"]["machine"] != manifest["inputs"]["launcher"]["machine"]:
        raise ValueError("Desk and its launcher target different architectures.")
    for name, expected in manifest["files"].items():
        content, _ = read_regular(output / name, 1024*1024)
        if hashlib.sha256(content).hexdigest() != expected:
            raise ValueError(f"Staged file changed: {name}")
    print("Stage and referenced binaries agree with the recorded hashes. This is not a signature or a graphical-session test.")

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="operation", required=True)
    make = sub.add_parser("stage")
    for option in ("desk", "launcher", "output"):
        make.add_argument("--"+option, required=True)
    default_icon = Path(__file__).resolve().parents[1] / "resources/brand/umicom-icon.png"
    make.add_argument("--icon", default=str(default_icon))
    inspect = sub.add_parser("verify"); inspect.add_argument("--output", required=True)
    args = parser.parse_args()
    try:
        if sys.platform != "linux":
            raise ValueError("Run session staging on Linux, using Linux binaries. It does not stage a Windows desktop session.")
        (stage if args.operation == "stage" else verify)(args)
    except (OSError, ValueError, KeyError, TypeError, json.JSONDecodeError) as exc:
        print(f"Not completed: {exc}", file=sys.stderr)
        return 1
    return 0
if __name__ == "__main__":
    raise SystemExit(main())

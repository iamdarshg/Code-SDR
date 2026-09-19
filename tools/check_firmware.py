#!/usr/bin/env python3
"""
Compile check for the RP2040 firmware.

Why this exists: the sketch was never compiled anywhere, so two whole-file
errors survived review - a use of an undeclared `drop_pct`, and a `selftest()`
placed above the `#define`s it reads (a #define is resolved lexically, so a
function above it cannot see it). Both are invisible to a reading; a compiler
finds them in seconds. The RP2040 is the board's only controller, so firmware
that does not build is a hard blocker.

Usage:
    python tools/check_firmware.py

Requires arduino-cli and the earlephilhower RP2040 core (the sketch uses
LittleFS and SPI.setSCK(), which are Arduino-Pico APIs, not the mbed core):

    arduino-cli config add board_manager.additional_urls \\
      https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
    arduino-cli core update-index
    arduino-cli core install rp2040:rp2040
"""

import os
import shutil
import subprocess
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SKETCH = os.path.join("firmware", "rp2040_dashboard")
FQBN = "rp2040:rp2040:rpipico"

CANDIDATES = [
    r"C:\Program Files\Arduino CLI\arduino-cli.exe",
    r"C:\Program Files (x86)\Arduino CLI\arduino-cli.exe",
]


def find_cli():
    exe = shutil.which("arduino-cli")
    if exe:
        return exe
    for c in CANDIDATES:
        if os.path.exists(c):
            return c
    return None


def main():
    cli = find_cli()
    if cli is None:
        print("arduino-cli not found - skipping firmware compile check")
        print("(install arduino-cli and the RP2040 core to enable it)")
        return 0

    # Is the core actually installed? Without this the compile fails with a
    # confusing 'platform not installed' error.
    r = subprocess.run([cli, "core", "list"], capture_output=True, text=True)
    if "rp2040:rp2040" not in (r.stdout + r.stderr):
        print("RP2040 core not installed - skipping firmware compile check")
        print("  install with: arduino-cli core install rp2040:rp2040")
        print("  (see the docstring of this script for the board URL)")
        return 0

    print(f"compiling {SKETCH} for {FQBN} ...")
    r = subprocess.run([cli, "compile", "--fqbn", FQBN, SKETCH],
                       cwd=REPO, capture_output=True, text=True)

    out = r.stdout + r.stderr
    errors = [l for l in out.splitlines() if "error:" in l]
    warns = [l for l in out.splitlines() if "warning:" in l]

    if r.returncode != 0 or errors:
        for l in errors[:15]:
            print("  " + l.strip())
        print(f"\nFAIL: {len(errors)} compile error(s)")
        return 1

    # Report the size line so a change that blows the part is visible.
    for l in out.splitlines():
        if "Sketch uses" in l or "Global variables use" in l:
            print("  " + l.strip())
    print(f"PASS: firmware compiles ({len(warns)} warning(s))")
    return 0


if __name__ == "__main__":
    sys.exit(main())

"""Remove degenerate one-point wires from a Freerouting Specctra session."""

from __future__ import annotations

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SES_PATH = ROOT / "build" / "Code-SDR-V2-routed.ses"
PATH_RE = re.compile(
    r"\(path\s+\S+\s+\d+\s*((?:-?\d+\s+-?\d+\s*)+)\)",
    re.DOTALL,
)


def main() -> None:
    source = SES_PATH.read_text(encoding="utf-8")
    lines = source.splitlines(keepends=True)
    output: list[str] = []
    removed = 0
    index = 0

    while index < len(lines):
        if lines[index].strip() != "(wire":
            output.append(lines[index])
            index += 1
            continue

        start = index
        depth = 0
        while index < len(lines):
            depth += lines[index].count("(") - lines[index].count(")")
            index += 1
            if depth == 0:
                break
        block = "".join(lines[start:index])
        path_match = PATH_RE.search(block)
        if path_match is None:
            if "(path " in block:
                removed += 1
                continue
            raise RuntimeError("Wire block has no Specctra path")
        coordinates = [int(value) for value in path_match.group(1).split()]
        points = list(zip(coordinates[::2], coordinates[1::2]))
        if len(set(points)) < 2:
            removed += 1
            continue
        output.append(block)

    cleaned = "".join(output)
    if cleaned.count("(") != cleaned.count(")"):
        raise RuntimeError("Sanitized session has unbalanced parentheses")
    SES_PATH.write_text(cleaned, encoding="utf-8", newline="\n")
    print(f"Removed {removed} degenerate one-point wires from {SES_PATH.name}")


if __name__ == "__main__":
    main()

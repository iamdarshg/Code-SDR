"""Generate pin-accurate local black-box symbols used by both hierarchy and PCB."""

from __future__ import annotations

from pathlib import Path

from design_model import ROOT, components


LIB = ROOT / "CodeSDR.kicad_sym"


def q(text: str) -> str:
    return text.replace("\\", "\\\\").replace('"', '\\"')


def child_symbol(item) -> str:
    pins = list(item.pins)
    count = len(pins)
    left_count = (count + 1) // 2
    pitch = 2.54
    half_height = max(12.7, left_count * pitch / 2 + 2.54)
    half_width = 17.78
    lines = [
        f'  (symbol "{q(item.symbol_name)}"',
        "    (exclude_from_sim no)",
        "    (in_bom yes)",
        "    (on_board yes)",
        f'    (property "Reference" "{q(item.ref.rstrip("0123456789") or "U")}" (at 0 {half_height + 2.54:.3f} 0)',
        "      (effects (font (size 1.27 1.27)))",
        "    )",
        f'    (property "Value" "{q(item.value)}" (at 0 {-half_height - 2.54:.3f} 0)',
        "      (effects (font (size 1.27 1.27)))",
        "    )",
        f'    (property "Footprint" "{q(item.footprint)}" (at 0 0 0)',
        "      (effects (font (size 1.27 1.27)) hide)",
        "    )",
        f'    (property "Datasheet" "{q(item.datasheet)}" (at 0 0 0)',
        "      (effects (font (size 1.27 1.27)) hide)",
        "    )",
        f'    (property "Description" "{q(item.description)}" (at 0 0 0)',
        "      (effects (font (size 1.27 1.27)) hide)",
        "    )",
        f'    (symbol "{q(item.symbol_name)}_0_1"',
        f"      (rectangle (start {-half_width:.3f} {half_height:.3f}) (end {half_width:.3f} {-half_height:.3f})",
        "        (stroke (width 0.254) (type default))",
        "        (fill (type background))",
        "      )",
        "    )",
        f'    (symbol "{q(item.symbol_name)}_1_1"',
    ]
    for index, pin in enumerate(pins):
        if index < left_count:
            x = -half_width - 5.08
            y = half_height - 2.54 - index * pitch
            orientation = 0
        else:
            x = half_width + 5.08
            y = half_height - 2.54 - (index - left_count) * pitch
            orientation = 180
        name = item.pin_names.get(pin, f"PAD_{pin}")
        lines.extend([
            f"      (pin passive line (at {x:.3f} {y:.3f} {orientation}) (length 5.08)",
            f'        (name "{q(name)}" (effects (font (size 1.0 1.0))))',
            f'        (number "{q(pin)}" (effects (font (size 1.0 1.0))))',
            "      )",
        ])
    lines.extend(["    )", "  )"])
    return "\n".join(lines)


def main() -> None:
    # A distinct symbol per value/pinout; passives and ordinary connectors use
    # the system libraries and do not need to be duplicated.
    custom = {}
    for item in components:
        if item.symbol and ":" in item.symbol:
            continue
        custom[item.symbol_name] = item
    text = [
        "(kicad_symbol_lib",
        "  (version 20231120)",
        '  (generator "Code-SDR-V2")',
    ]
    text.extend(child_symbol(item) for item in sorted(custom.values(), key=lambda x: x.symbol_name))
    text.append(")")
    LIB.parent.mkdir(parents=True, exist_ok=True)
    LIB.write_text("\n".join(text) + "\n", encoding="utf-8")
    print(f"Generated {LIB} with {len(custom)} symbols")


if __name__ == "__main__":
    main()


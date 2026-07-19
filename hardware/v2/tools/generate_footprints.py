"""Generate the small set of datasheet-specific Code-SDR V2 footprints."""

from __future__ import annotations

from pathlib import Path

from design_model import ROOT, fpga_pins


OUT = ROOT / "CodeSDR.pretty"


def pad(number: str, x: float, y: float, sx: float, sy: float, shape: str = "roundrect") -> str:
    extra = " (roundrect_rratio 0.2)" if shape == "roundrect" else ""
    return (
        f'  (pad "{number}" smd {shape} (at {x:.3f} {y:.3f}) (size {sx:.3f} {sy:.3f})'
        f' (layers "F.Cu" "F.Paste" "F.Mask"){extra})'
    )


def footprint(name: str, body_x: float, body_y: float, pads: list[str], description: str) -> None:
    cx, cy = body_x / 2, body_y / 2
    courtyard_x, courtyard_y = cx + 0.25, cy + 0.25
    content = [
        f'(footprint "{name}"',
        '  (version 20240108)',
        '  (generator "CodeSDR")',
        '  (layer "F.Cu")',
        f'  (descr "{description}")',
        '  (attr smd)',
        '  (fp_text reference "REF**" (at 0 -2.5 0) (layer "F.SilkS")',
        '    (effects (font (size 0.8 0.8) (thickness 0.12))))',
        '  (fp_text value "VALUE" (at 0 2.5 0) (layer "F.Fab") hide',
        '    (effects (font (size 0.8 0.8) (thickness 0.12))))',
        f'  (fp_rect (start {-cx:.3f} {-cy:.3f}) (end {cx:.3f} {cy:.3f})',
        '    (stroke (width 0.1) (type default)) (fill none) (layer "F.Fab"))',
        f'  (fp_rect (start {-courtyard_x:.3f} {-courtyard_y:.3f})'
        f' (end {courtyard_x:.3f} {courtyard_y:.3f})',
        '    (stroke (width 0.05) (type default)) (fill none) (layer "F.CrtYd"))',
        # Tiny RF packages do not have enough free surface for a useful silk
        # body outline. Keep the polarity mark on fabrication output so silk
        # cannot encroach on solder mask openings.
        f'  (fp_circle (center {-cx + 0.18:.3f} {-cy + 0.18:.3f}) (end {-cx + 0.28:.3f} {-cy + 0.18:.3f})',
        '    (stroke (width 0.08) (type default)) (fill solid) (layer "F.Fab"))',
        *pads,
        ')',
        '',
    ]
    (OUT / f"{name}.kicad_mod").write_text("\n".join(content), encoding="utf-8")


def main() -> None:
    OUT.mkdir(parents=True, exist_ok=True)

    # Lattice FPGA-DS-02053: JMG80 is 6.5 mm ctfBGA, 10 x 10 grid,
    # 0.65 mm pitch, 0.25 mm nominal balls.  The omitted centre sites are
    # encoded by the official JMG80 ball list used by the schematic.
    bga = []
    rows = "ABCDEFGHJK"
    for ball in fpga_pins:
        row, column = ball[0], int(ball[1:])
        bga.append(pad(ball, (column - 5.5) * 0.65, (rows.index(row) - 4.5) * 0.65, 0.30, 0.30, "circle"))
    footprint(
        "LIF-MD6000-6JMG80I", 6.5, 6.5, bga,
        "Lattice JMG80 80-ball ctfBGA, 6.5x6.5 mm, 0.65 mm pitch, NSMD lands",
    )

    grf = [
        pad("1", -0.725, -0.50, 0.50, 0.28), pad("2", -0.725, 0, 0.50, 0.28),
        pad("3", -0.725, 0.50, 0.50, 0.28), pad("4", 0.725, 0.50, 0.50, 0.28),
        pad("5", 0.725, 0, 0.50, 0.28), pad("6", 0.725, -0.50, 0.50, 0.28),
        pad("7", 0, 0, 0.62, 1.05),
    ]
    footprint(
        "GRF_DFN6_1.5x1.5mm", 1.5, 1.5, grf,
        "Guerrilla RF DFN6-15-25-B common footprint with exposed ground pad",
    )

    ltc = []
    for index in range(6):
        y = -1.25 + index * 0.5
        ltc.append(pad(str(index + 1), -1.45, y, 0.65, 0.25))
        ltc.append(pad(str(12 - index), 1.45, y, 0.65, 0.25))
    ltc.append(pad("13", 0, 0, 1.70, 0.75))
    footprint(
        "DFN-12-1EP_3x2mm_P0.5mm", 3.0, 2.0, ltc,
        "Analog Devices UDB12 variation A, 3x2 mm QFN, 0.50 mm pitch",
    )

    footprint(
        "SOD962-2_DSN0603", 0.6, 0.3,
        [pad("1", -0.30, 0, 0.30, 0.28), pad("2", 0.30, 0, 0.30, 0.28)],
        "Nexperia SOD962/DSN0603-2 ultra-low-capacitance ESD diode",
    )

    footprint(
        "Infineon_TSNP6_1.1x0.7mm", 1.1, 0.7,
        [
            pad("1", -0.40, -0.35, 0.25, 0.40), pad("2", 0, -0.35, 0.25, 0.40),
            pad("3", 0.40, -0.35, 0.25, 0.40), pad("4", 0.40, 0.35, 0.25, 0.40),
            pad("5", 0, 0.35, 0.25, 0.40), pad("6", -0.40, 0.35, 0.25, 0.40),
        ],
        "Infineon PG-TSNP-6-2, 1.1x0.7 mm, datasheet Figure 5 NSMD pattern",
    )

    footprint(
        "MiniCircuits_FV1206", 3.2, 1.6,
        [
            pad("1", -1.35, -0.45, 0.70, 0.55), pad("2", -1.35, 0.45, 0.70, 0.55),
            pad("3", 1.35, -0.45, 0.70, 0.55), pad("4", 1.35, 0.45, 0.70, 0.55),
        ],
        "Mini-Circuits FV1206 four-terminal RF filter land pattern",
    )

    footprint(
        "Johanson_0805_6P_Balun", 2.0, 1.25,
        [
            pad("1", -0.88, -0.50, 0.48, 0.36), pad("2", -0.88, 0, 0.48, 0.36),
            pad("3", -0.88, 0.50, 0.48, 0.36), pad("4", 0.88, 0.50, 0.48, 0.36),
            pad("5", 0.88, 0, 0.48, 0.36), pad("6", 0.88, -0.50, 0.48, 0.36),
        ],
        "Johanson 2450BL15B0100001E six-terminal 2.0x1.25 mm balun",
    )

    footprint(
        "QPQ1909_1.4x1.2mm", 1.4, 1.2,
        [
            pad("1", -0.495, 0, 0.254, 0.330),
            pad("2", 0, 0.286, 0.254, 0.330),
            pad("3", 0.495, 0.286, 0.254, 0.330),
            pad("4", 0.495, -0.286, 0.254, 0.330),
            pad("5", 0, -0.286, 0.254, 0.330),
        ],
        "Qorvo QPQ1909 five-pad 1.4x1.2 mm laminate, 0.254x0.330 mm recommended lands",
    )

    # Qorvo QPC6144 Rev. C, Figure 11.2: six 0.5 mm-pitch terminals
    # on every side and a 2.70 x 2.70 mm exposed ground land.  Keep paste
    # off the solid EP land and use four stencil apertures for about 61 %
    # coverage to reduce float/voiding while retaining the required RF ground.
    qpc6144 = []
    for index in range(6):
        offset = -1.25 + index * 0.50
        qpc6144.append(pad(str(index + 1), -1.775, offset, 0.300, 0.250))
        qpc6144.append(pad(str(7 + index), offset, 1.775, 0.250, 0.300))
        qpc6144.append(pad(str(13 + index), 1.775, -offset, 0.300, 0.250))
        qpc6144.append(pad(str(24 - index), offset, -1.775, 0.250, 0.300))
    qpc6144.append(
        '  (pad "25" smd rect (at 0 0) (size 2.700 2.700)'
        ' (layers "F.Cu" "F.Mask"))'
    )
    for paste_x in (-0.65, 0.65):
        for paste_y in (-0.65, 0.65):
            qpc6144.append(
                f'  (pad "" smd rect (at {paste_x:.3f} {paste_y:.3f})'
                ' (size 1.000 1.000) (layers "F.Paste"))'
            )
    footprint(
        "QPC6144_LGA24_4x4mm", 4.0, 4.0, qpc6144,
        "Qorvo QPC6144 24-lead 4x4 mm LGA plus exposed ground, Rev. C recommended lands",
    )

    print(f"Generated {len(list(OUT.glob('*.kicad_mod')))} CodeSDR footprints in {OUT}")


if __name__ == "__main__":
    main()

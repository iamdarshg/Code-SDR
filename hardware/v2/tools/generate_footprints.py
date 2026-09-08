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


def footprint(
    name: str, body_x: float, body_y: float, pads: list[str], description: str,
    *, courtyard: tuple[float, float] | None = None,
    pin1_mark: tuple[float, float] | None = None,
) -> None:
    cx, cy = body_x / 2, body_y / 2
    courtyard_x, courtyard_y = cx + 0.25, cy + 0.25
    if courtyard is not None:
        courtyard_x, courtyard_y = courtyard[0] / 2, courtyard[1] / 2
    mark_x, mark_y = pin1_mark if pin1_mark is not None else (-cx + 0.18, -cy + 0.18)
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
        f'  (fp_circle (center {mark_x:.3f} {mark_y:.3f}) (end {mark_x + 0.10:.3f} {mark_y:.3f})',
        '    (stroke (width 0.08) (type default)) (fill solid) (layer "F.Fab"))',
        *pads,
        ')',
        '',
    ]
    (OUT / f"{name}.kicad_mod").write_text("\n".join(content), encoding="utf-8")


def generate_qpc6144() -> None:
    """Generate QPC6144 alone so its audited correction can be applied in isolation."""
    # Qorvo QPC6144 Rev. C (2026-06-01), section 11.2, page 10:
    # six 0.5 mm-pitch terminals on each side; 2.70 mm square ground
    # land with a 0.225 mm upper-left chamfer (2.475 mm straight edges).
    # Add the specified 0.05 mm mask expansion locally, independent of
    # the board default. Preserve the existing signal-pad rounded corners.
    qpc6144 = []
    for index in range(6):
        offset = -1.25 + index * 0.50
        for number, x, y, sx, sy in (
            (str(index + 1), -1.775, offset, 0.300, 0.250),
            (str(7 + index), offset, 1.775, 0.250, 0.300),
            (str(13 + index), 1.775, -offset, 0.300, 0.250),
            (str(24 - index), offset, -1.775, 0.250, 0.300),
        ):
            qpc6144.append(pad(number, x, y, sx, sy)[:-1] + ' (solder_mask_margin 0.05))')
    qpc6144.append(
        '  (pad "25" smd custom (at 0 0) (size 0.100 0.100)'
        ' (layers "F.Cu" "F.Mask") (solder_mask_margin 0.05)'
        ' (options (clearance outline) (anchor rect))'
        ' (primitives (gr_poly (pts'
        ' (xy -1.125 -1.350) (xy 1.350 -1.350) (xy 1.350 1.350)'
        ' (xy -1.350 1.350) (xy -1.350 -1.125)) (width 0) (fill yes))))'
    )
    # Four retained 1 mm square stencil apertures give about 55% coverage
    # of the chamfered EP. This windowing is our assembly choice; the
    # datasheet specifies the copper/mask lands, not this stencil pattern.
    for paste_x in (-0.65, 0.65):
        for paste_y in (-0.65, 0.65):
            qpc6144.append(
                f'  (pad "" smd rect (at {paste_x:.3f} {paste_y:.3f})'
                ' (size 1.000 1.000) (layers "F.Paste"))'
            )
    footprint(
        "QPC6144_LGA24_4x4mm", 4.0, 4.0, qpc6144,
        "Qorvo QPC6144 Rev. C section 11.2; 24-lead 4x4 mm LGA; chamfered 2.7 mm ground land; 0.05 mm mask expansion",
    )


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

    # LTC5548 datasheet 5548f, pp. 2 and 19, UDB12 variation A:
    # three pins on EACH of four sides, not a two-row DFN. Top view:
    #          12 11 10
    #       1           9
    #       2    13     8
    #       3           7
    #           4  5  6
    # The tiny exposed ground is deliberately offset -0.175 mm in Y:
    # its lower edge is 0.05 mm above the package centreline (detail B).
    ltc = [pad("1", -1.325, -0.5, 0.85, 0.25)]
    for index in range(3):
        offset = -0.5 + index * 0.5
        if index:
            ltc.append(pad(str(index + 1), -1.375, offset, 0.75, 0.25))
        ltc.append(pad(str(9 - index), 1.375, offset, 0.75, 0.25))
        ltc.append(pad(str(4 + index), offset, 0.925, 0.25, 0.65))
        ltc.append(pad(str(12 - index), offset, -0.925, 0.25, 0.65))
    ltc.append(pad("13", 0, -0.175, 0.77, 0.25, "rect"))
    footprint(
        "DFN-12-1EP_3x2mm_P0.5mm", 3.0, 2.0, ltc,
        "Analog Devices LTC5548 UDB12 variation A; 5548f pp.2,19; four-sided QFN; rectangular pin-1 land",
        courtyard=(4.0, 3.0),
    )

    footprint(
        "SOD962-2_DSN0603", 0.6, 0.3,
        [pad("1", -0.30, 0, 0.30, 0.28), pad("2", 0.30, 0, 0.30, 0.28)],
        "Nexperia SOD962/DSN0603-2 ultra-low-capacitance ESD diode",
    )

    # BGS12WN6 Rev. 2.9, pp. 9/11, Figures 2/5. Rotate the TOP view
    # counterclockwise by 90 degrees to retain this horizontal body:
    #     3 2 1
    #     4 5 6
    # Figure 5 specifies square copper and circular stencil apertures.
    # The 0.025 mm NSMD expansion is our fabrication choice, not a
    # manufacturer-dimensioned mask opening. Stencil thickness: 0.100 mm.
    tsnp = []
    for number, x, y in (
        ("1", 0.4, -0.2), ("2", 0, -0.2), ("3", -0.4, -0.2),
        ("4", -0.4, 0.2), ("5", 0, 0.2), ("6", 0.4, 0.2),
    ):
        tsnp.append(
            f'  (pad "{number}" smd rect (at {x:.3f} {y:.3f})'
            ' (size 0.250 0.250) (layers "F.Cu" "F.Mask")'
            ' (solder_mask_margin 0.025))'
        )
        tsnp.append(
            f'  (pad "" smd circle (at {x:.3f} {y:.3f})'
            ' (size 0.250 0.250) (layers "F.Paste"))'
        )
    footprint(
        "Infineon_TSNP6_1.1x0.7mm", 1.1, 0.7, tsnp,
        "Infineon PG-TSNP-6-2; BGS12WN6 Rev 2.9 Figs 2/5; top view 3-2-1 / 4-5-6; 0.25 mm square NSMD lands",
        pin1_mark=(0.35, 0),
    )

    footprint(
        "MiniCircuits_FV1206", 3.2, 1.6,
        [
            pad("1", -1.625, 0, 1.04, 1.80),
            pad("2", 0, 0.9275, 0.61, 1.245),
            pad("3", 1.625, 0, 1.04, 1.80),
            pad("4", 0, -0.9275, 0.61, 1.245),
            '  (pad "2" thru_hole circle (at 0 1.105) (size 0.610 0.610)'
            ' (drill 0.300) (layers "*.Cu" "*.Mask"))',
            '  (pad "4" thru_hole circle (at 0 -1.105) (size 0.610 0.610)'
            ' (drill 0.300) (layers "*.Cu" "*.Mask"))',
        ],
        "Mini-Circuits FV1206 98-FV Rev V; RF ends 1/3, ground sides 2/4; recommended ground holes",
        courtyard=(4.8, 3.6), pin1_mark=(-1.10, 0),
    )

    footprint(
        "Johanson_0805_6P_Balun", 2.0, 1.25,
        [
            pad("1", 0.65, -0.90, 0.35, 1.00), pad("2", 0, -0.90, 0.35, 1.00),
            pad("3", -0.65, -0.90, 0.35, 1.00), pad("4", -0.65, 0.90, 0.35, 1.00),
            pad("5", 0, 0.90, 0.35, 1.00), pad("6", 0.65, 0.90, 0.35, 1.00),
        ],
        "Johanson 2450BL15B0100001E/2450BL15B0200001E; datasheet pp.2-3; 0.65 mm pitch, top view 3-2-1 / 4-5-6",
        courtyard=(2.6, 3.3), pin1_mark=(0.65, 0),
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

    generate_qpc6144()

    print(f"Generated {len(list(OUT.glob('*.kicad_mod')))} CodeSDR footprints in {OUT}")


if __name__ == "__main__":
    main()

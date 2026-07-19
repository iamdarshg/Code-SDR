"""Generate the placed four-layer Code-SDR V2 PCB from design_model.py."""

from __future__ import annotations

import json
import math
from collections import defaultdict
from pathlib import Path

import pcbnew

from design_model import ROOT, SHEETS, all_nets, by_sheet, components


SYSTEM_FOOTPRINTS = Path(r"C:\Program Files\KiCad\9.0\share\kicad\footprints")
BOARD_PATH = ROOT / "Code-SDR-V2.kicad_pcb"
DSN_PATH = ROOT / "build" / "Code-SDR-V2.dsn"
PROJECT_PATH = ROOT / "Code-SDR-V2.kicad_pro"
SCHEMATIC_PATHS = ROOT / "build" / "schematic_paths.json"

W, H = 160.0, 100.0

# Cost-first 160 x 100 mm board. Each rectangle is a physical functional block;
# the ESC remains an entirely separate board and is not present in this design.
REGIONS = {
    "07_rf_high": (4.0, 18.0, 132.0, 10.0),
    "06_rf_low": (4.0, 31.0, 53.0, 24.0),
    "08_if_chain": (61.0, 31.0, 75.0, 31.0),
    "05_reference_lo": (4.0, 58.0, 53.0, 19.0),
    "01_power": (4.0, 77.5, 23.0, 15.5),
    "02_controller": (30.0, 77.5, 36.0, 15.5),
    "03_fpga_adc": (69.0, 65.0, 40.0, 28.0),
    "04_ethernet": (109.5, 65.0, 16.0, 28.0),
}

MANUAL_PLACEMENT = {
    # Keep the protected 5 V entry beside the power converters.  The rotated
    # terminal block stays inside the bottom-left outline and avoids carrying
    # unfused input power across the RF/digital board.
    "J1": (8.5, 94.0, 180.0),
    "F1": (14.5, 96.0, 0.0),
    "D1": (21.5, 92.5, 0.0),
    "FB1": (18.0, 87.8, 0.0),
    "J10": (48.0, 100.0, 0.0),
    "SW1": (54.0, 90.0, 0.0),
    "SW2": (35.0, 90.0, 0.0),
    # RJ45 jack axis points toward the right-hand board edge. Its mating face
    # ends at x=159.15 mm while all signal pins and Ethernet electronics remain
    # safely inboard. The extra 20 mm is cheaper than a second-sided assembly.
    "J30": (133.0, 78.0, 0.0),
    "J50": (0.0, 44.0, 0.0),
    # 270 degrees puts each SMA body outside the top outline and leaves only
    # the manufacturer-defined edge-clamping lands crossing into the board.
    "J60": (20.0, 0.0, 270.0),
    "J61": (50.0, 0.0, 270.0),
    "J62": (80.0, 0.0, 270.0),
    "J63": (110.0, 0.0, 270.0),
}
FOOTPRINT_CACHE: dict[str, pcbnew.FOOTPRINT] = {}
PATH_MAP = json.loads(SCHEMATIC_PATHS.read_text(encoding="utf-8"))["components"]


# RF placement is derived from the same logical coordinates as the schematic,
# but compressed into short, monotonic signal chains. The >7 GHz C/D inputs
# have the shortest runs into the post-LNA switch; slower A/B inputs tolerate
# the longer convergence runs.
RF_HIGH_PLACEMENT: dict[str, tuple[float, float, float]] = {}
for port_x, refs in [
    (20.0, ("D60", "C215", "C219", "U60", "R100", "L83", "C223", "C224")),
    (50.0, ("D61", "C216", "C2110", "U61", "R101", "R105", "C225", None)),
    (80.0, ("D62", "C217", "C2111", "U62", "R102", "R106", "C226", "C2210")),
    (110.0, ("D63", "C218", "C2112", "U66", "R103", "R107", "C227", "C2211")),
]:
    esd, series, shunt, lna, enable, output_part, bypass, vdd_match = refs
    RF_HIGH_PLACEMENT.update({
        esd: (port_x, 5.2, 90.0),
        series: (port_x, 7.0, 90.0),
        shunt: (port_x - 2.0, 7.7, 0.0),
        lna: (port_x, 9.6, 90.0),
        enable: (port_x - 2.2, 10.5, 90.0),
        output_part: (port_x, 12.3, 90.0),
        bypass: (port_x + 2.2, 10.5, 0.0),
    })
    if vdd_match:
        RF_HIGH_PLACEMENT[vdd_match] = (port_x + 2.2, 12.4, 0.0)
RF_HIGH_PLACEMENT.update({
    # The second D-port LNA forms a short monotonic 10 GHz path from U66 to
    # the closest QPC6144 common-port side.
    "C291": (107.0, 13.2, 0.0),
    "C292": (106.5, 15.0, 0.0),
    "U67": (103.0, 13.2, 0.0),
    "R112": (101.0, 15.0, 0.0),
    "R111": (100.0, 13.2, 0.0),
    "C293": (103.0, 10.5, 0.0),
    "C294": (100.5, 10.5, 0.0),
    "U64": (95.0, 16.0, 0.0),
    "C260": (92.0, 20.5, 0.0),
    "C261": (95.0, 21.0, 0.0),
    "R144": (98.0, 20.5, 0.0),
    "R145": (100.5, 20.5, 0.0),
    "C232": (100.0, 16.0, 0.0),
    "C233": (104.0, 12.5, 90.0),
    "U63": (105.0, 16.0, 0.0),
    "C93": (105.0, 20.5, 0.0),
    "C94": (108.5, 20.5, 0.0),
    "T60": (113.0, 16.0, 0.0),
})


ADF_REFS = {
    "U41", "C72", "C73", "R58", "C74", "R59", "C75", "R93", "C205",
    "R94", "R95", "C206", "C207", "C76", "C77", "C78",
}


def placement_override(item) -> tuple[float, float, float] | None:
    if item.ref in MANUAL_PLACEMENT:
        return MANUAL_PLACEMENT[item.ref]
    # The model coordinates on these four sheets already describe their
    # signal flow.  Preserve that topology on the PCB instead of packing by
    # footprint height, which can strand bypass capacitors away from their IC
    # and lengthen high-speed buses.
    if item.sheet == "01_power":
        x = 4.0 + (item.xy[0] - 22.0) * 0.31
        y = 78.0 + (item.xy[1] - 77.0) * 0.72
        return x, y, item.rotation
    if item.sheet == "02_controller":
        x = 31.0 + (item.xy[0] - 18.0) * 0.75
        y = 78.0 + (item.xy[1] - 25.0) * 0.50
        return x, y, item.rotation
    if item.sheet == "03_fpga_adc":
        x = 70.0 + (item.xy[0] - 78.0) * 0.78
        y = 66.0 + (item.xy[1] - 21.0) * 0.75
        return x, y, item.rotation
    if item.sheet == "04_ethernet":
        x = 110.0 + (item.xy[0] - 136.0) * 0.35
        y = 66.0 + (item.xy[1] - 24.0) * 0.72
        return x, y, item.rotation
    if item.sheet == "06_rf_low":
        x = (item.xy[0] - 5.0) * 0.65
        y = 44.0 + (item.xy[1] - 63.0) * 0.55
        return x, y, item.rotation
    if item.sheet == "07_rf_high":
        return RF_HIGH_PLACEMENT[item.ref]
    if item.sheet == "08_if_chain":
        if item.ref in {"U75", "R81", "L97", "C284", "C238"}:
            local = {
                "U75": (98.0, 40.0, 0.0),
                "R81": (95.5, 42.5, 0.0),
                "L97": (100.5, 42.5, 0.0),
                "C284": (103.0, 42.5, 0.0),
                "C238": (102.0, 40.0, 0.0),
            }
            return local[item.ref]
        x = 72.0 + (item.xy[0] - 73.0) * 0.80
        y = 34.0 + (item.xy[1] - 67.0) * 0.55
        return x, y, item.rotation
    if item.sheet == "05_reference_lo":
        if item.ref in ADF_REFS:
            x = 102.0 + (item.xy[0] - 165.0) * 0.65
            y = 45.0 + (item.xy[1] - 88.0) * 0.45
        else:
            x = 70.0 + (item.xy[0] - 140.0) * 0.65
            y = 22.0 + (item.xy[1] - 88.0) * 0.45
        return x, y, item.rotation
    return None


def load_footprint(identifier: str) -> pcbnew.FOOTPRINT:
    if identifier in FOOTPRINT_CACHE:
        return FOOTPRINT_CACHE[identifier].Duplicate()
    library, name = identifier.split(":", 1)
    if library == "CodeSDR":
        path = ROOT / "CodeSDR.pretty"
    else:
        path = SYSTEM_FOOTPRINTS / f"{library}.pretty"
    result = pcbnew.FootprintLoad(str(path), name)
    if result is None:
        raise FileNotFoundError(f"Cannot load {identifier} from {path}")
    FOOTPRINT_CACHE[identifier] = result
    return result.Duplicate()


def prepare_footprint(item, net_items: dict[str, pcbnew.NETINFO_ITEM]) -> pcbnew.FOOTPRINT:
    footprint = load_footprint(item.footprint)
    footprint.SetReference(item.ref)
    footprint.SetValue(item.value)
    library, name = item.footprint.split(":", 1)
    footprint.SetFPID(pcbnew.LIB_ID(library, name))
    footprint.SetPath(pcbnew.KIID_PATH(PATH_MAP[item.ref]["path"]))
    footprint.SetSheetname(item.sheet)
    footprint.SetSheetfile(f"sheets/{item.sheet}.kicad_sch")
    footprint.SetOrientationDegrees(item.rotation)
    footprint.Value().SetVisible(False)
    # Dense RF/passive placement cannot carry legible designators on board
    # silk. References remain available in BOM, position and interactive CAD
    # data; the release also exports an assembly drawing.
    footprint.Reference().SetVisible(False)
    for pad_item in footprint.Pads():
        number = pad_item.GetNumber()
        net_name = item.pins.get(number)
        if net_name:
            pad_item.SetNet(net_items[net_name])
    return footprint


def place_region(board: pcbnew.BOARD, sheet: str, net_items: dict[str, pcbnew.NETINFO_ITEM]) -> None:
    x0, y0, width, height = REGIONS[sheet]
    packed = []
    for item in by_sheet(sheet):
        if placement_override(item) is not None:
            continue
        fp = prepare_footprint(item, net_items)
        bbox = fp.GetBoundingBox()
        packed.append((
            pcbnew.ToMM(bbox.GetHeight()) + 0.20,
            pcbnew.ToMM(bbox.GetWidth()) + 0.20,
            pcbnew.ToMM(bbox.GetX()),
            pcbnew.ToMM(bbox.GetY()),
            fp,
        ))

    # Height-sorted shelf packing avoids pad/courtyard collisions while keeping
    # the board small.  KiCad's full footprint box includes the courtyard;
    # the extra 0.20 mm provides assembly tolerance beyond it.
    packed.sort(key=lambda entry: (-entry[0], -entry[1]))
    cursor_x, cursor_y, shelf_height = x0, y0, 0.0
    for item_height, item_width, local_x, local_y, footprint in packed:
        if cursor_x + item_width > x0 + width:
            cursor_x = x0
            cursor_y += shelf_height
            shelf_height = 0.0
        if cursor_y + item_height > y0 + height:
            raise RuntimeError(f"{sheet} does not fit its {width}x{height} mm placement region")
        target_left = cursor_x + 0.10
        target_top = cursor_y + 0.10
        origin_x = target_left - local_x
        origin_y = target_top - local_y
        footprint.SetPosition(pcbnew.VECTOR2I(pcbnew.FromMM(origin_x), pcbnew.FromMM(origin_y)))
        board.Add(footprint)
        cursor_x += item_width
        shelf_height = max(shelf_height, item_height)


def place_overrides(board: pcbnew.BOARD, net_items: dict[str, pcbnew.NETINFO_ITEM]) -> None:
    override_items = [item for item in components if placement_override(item) is not None]

    def placement_priority(item) -> tuple[int, str]:
        if item.ref in MANUAL_PLACEMENT:
            return 0, item.ref
        if item.ref.startswith(("U", "T", "FL", "Y")):
            return 1, item.ref
        return 2, item.ref

    override_items.sort(key=placement_priority)
    occupied = [footprint.GetBoundingBox() for footprint in board.GetFootprints()]

    def overlaps(candidate) -> bool:
        margin = pcbnew.FromMM(0.10)
        left = candidate.GetLeft() - margin
        right = candidate.GetRight() + margin
        top = candidate.GetTop() - margin
        bottom = candidate.GetBottom() + margin
        for other in occupied:
            if (
                left < other.GetRight() and right > other.GetLeft()
                and top < other.GetBottom() and bottom > other.GetTop()
            ):
                return True
        return False

    search_offsets = [(0.0, 0.0)]
    for radius_steps in range(1, 25):
        radius = radius_steps * 0.50
        search_offsets.extend([
            (0.0, radius), (0.0, -radius), (radius, 0.0), (-radius, 0.0),
            (radius, radius), (radius, -radius), (-radius, radius), (-radius, -radius),
        ])

    for item in override_items:
        placement = placement_override(item)
        assert placement is not None
        x, y, rotation = placement
        footprint = prepare_footprint(item, net_items)
        footprint.SetOrientationDegrees(rotation)
        fixed = item.ref in MANUAL_PLACEMENT
        for dx, dy in search_offsets:
            footprint.SetPosition(
                pcbnew.VECTOR2I(pcbnew.FromMM(x + dx), pcbnew.FromMM(y + dy))
            )
            candidate = footprint.GetBoundingBox()
            if fixed or not overlaps(candidate):
                occupied.append(candidate)
                break
        else:
            raise RuntimeError(f"No collision-free RF placement found near {item.ref} at {x},{y}")
        board.Add(footprint)


def add_edge(board: pcbnew.BOARD, start: tuple[float, float], end: tuple[float, float]) -> None:
    edge = pcbnew.PCB_SHAPE(board)
    edge.SetShape(pcbnew.SHAPE_T_SEGMENT)
    edge.SetLayer(pcbnew.Edge_Cuts)
    edge.SetStart(pcbnew.VECTOR2I(pcbnew.FromMM(start[0]), pcbnew.FromMM(start[1])))
    edge.SetEnd(pcbnew.VECTOR2I(pcbnew.FromMM(end[0]), pcbnew.FromMM(end[1])))
    edge.SetWidth(pcbnew.FromMM(0.1))
    board.Add(edge)


def add_text(
    board: pcbnew.BOARD,
    text: str,
    x: float,
    y: float,
    size: float = 1.0,
    layer: int = pcbnew.F_SilkS,
) -> None:
    item = pcbnew.PCB_TEXT(board)
    item.SetText(text)
    item.SetLayer(layer)
    item.SetPosition(pcbnew.VECTOR2I(pcbnew.FromMM(x), pcbnew.FromMM(y)))
    item.SetTextSize(pcbnew.VECTOR2I(pcbnew.FromMM(size), pcbnew.FromMM(size)))
    item.SetTextThickness(pcbnew.FromMM(max(0.12, size / 7)))
    item.SetHorizJustify(pcbnew.GR_TEXT_H_ALIGN_LEFT)
    board.Add(item)


def add_ground_zone(board: pcbnew.BOARD, net: pcbnew.NETINFO_ITEM, layer: int, margin: float) -> None:
    zone = pcbnew.ZONE(board)
    zone.SetLayer(layer)
    zone.SetNet(net)
    zone.SetLocalClearance(pcbnew.FromMM(0.15))
    zone.SetMinThickness(pcbnew.FromMM(0.12))
    polygon = zone.Outline()
    polygon.NewOutline()
    for x, y in [
        (margin, margin), (W - margin, margin),
        (W - margin, H - margin), (margin, H - margin),
    ]:
        polygon.Append(pcbnew.FromMM(x), pcbnew.FromMM(y))
    board.Add(zone)


def is_critical_signal(name: str) -> bool:
    return (
        name.startswith((
            "RF_", "LO_", "LMX_LO", "ADF_LO2", "LTC_", "IF_HIGH", "IF_LOW",
            "IF_SELECTED", "IF_BAW", "GRF2013_RFIN", "GRF2013_RFOUT",
            "IF_GAIN", "PE4312", "IF_DSA", "LT_INPUT", "LT_IN", "IF2",
            "AD8351_IN", "AD8351_OUT", "ADC_VIN", "REF_100M",
        ))
        or name in {"ADC_CLK", "ADF_REF", "LMX_REF", "LMX_REF_N", "LMX_REF_PRE"}
    )


def route_critical_signals(
    board: pcbnew.BOARD,
    net_items: dict[str, pcbnew.NETINFO_ITEM],
) -> int:
    """Pre-route RF/clock/analog nets as shortest top-layer trees.

    These routes are deliberately present in the DSN before Freerouting sees
    the design, so the general-purpose router cannot introduce vias into the
    microwave paths.
    """
    pads_by_net: dict[str, list[pcbnew.PAD]] = defaultdict(list)
    for footprint in board.GetFootprints():
        for pad_item in footprint.Pads():
            name = pad_item.GetNetname()
            if name and is_critical_signal(name):
                pads_by_net[name].append(pad_item)

    count = 0
    for name, pads in sorted(pads_by_net.items()):
        points: list[pcbnew.VECTOR2I] = []
        for pad_item in pads:
            point = pad_item.GetPosition()
            if all(point != existing for existing in points):
                points.append(point)
        if len(points) < 2:
            continue
        connected = [points[0]]
        remaining = points[1:]
        while remaining:
            start, end = min(
                ((a, b) for a in connected for b in remaining),
                key=lambda pair: (
                    (pair[0].x - pair[1].x) ** 2 + (pair[0].y - pair[1].y) ** 2
                ),
            )
            track = pcbnew.PCB_TRACK(board)
            track.SetStart(start)
            track.SetEnd(end)
            track.SetLayer(pcbnew.F_Cu)
            width = 0.35 if not name.startswith((
                "IF2", "AD8351_IN", "AD8351_OUT", "ADC_VIN", "REF_100M",
            )) and name != "ADC_CLK" else 0.20
            track.SetWidth(pcbnew.FromMM(width))
            track.SetNet(net_items[name])
            board.Add(track)
            count += 1
            connected.append(end)
            remaining.remove(end)
    return count


def configure_netclasses(
    board: pcbnew.BOARD,
    net_items: dict[str, pcbnew.NETINFO_ITEM],
) -> None:
    definitions = {
        "RF_50": (0.18, 0.35, 0.60, 0.30, 0.20, 0.20),
        "IF_DIFF": (0.15, 0.20, 0.50, 0.25, 0.20, 0.20),
        "DIGITAL_HS": (0.12, 0.15, 0.45, 0.20, 0.15, 0.15),
        "POWER": (0.15, 0.60, 0.80, 0.40, 0.20, 0.20),
    }
    classes = {}
    for name, (clearance, width, via, drill, diff_width, diff_gap) in definitions.items():
        netclass = pcbnew.NETCLASS(name)
        netclass.SetClearance(pcbnew.FromMM(clearance))
        netclass.SetTrackWidth(pcbnew.FromMM(width))
        netclass.SetViaDiameter(pcbnew.FromMM(via))
        netclass.SetViaDrill(pcbnew.FromMM(drill))
        netclass.SetDiffPairWidth(pcbnew.FromMM(diff_width))
        netclass.SetDiffPairGap(pcbnew.FromMM(diff_gap))
        board.GetNetClasses()[name] = netclass
        classes[name] = netclass

    for name, net in net_items.items():
        if (
            name.startswith((
                "RF_", "LO_", "LMX_LO", "ADF_LO2", "LTC_", "IF_HIGH", "IF_LOW",
                "IF_SELECTED", "IF_BAW", "GRF2013_RFIN", "GRF2013_RFOUT",
                "IF_GAIN", "PE4312", "IF_DSA", "LT_INPUT", "LT_IN",
            ))
        ):
            net.SetNetClass(classes["RF_50"])
        elif name.startswith(("IF2", "AD8351_IN", "AD8351_OUT", "ADC_VIN")):
            net.SetNetClass(classes["IF_DIFF"])
        elif name.startswith(("RGMII", "ADC_D", "ADC_CLK", "REF_100M")):
            net.SetNetClass(classes["DIGITAL_HS"])
        elif (
            name.startswith(("+", "VIN_", "SW_"))
            or name in {
                "VIN_5V", "VIN_FUSED", "RP_CORE",
                "PHY_AVDDH_3V3", "PHY_AVDDL_1V2", "PHY_PLL_1V2",
            }
            or name.startswith("FPGA_VCC")
            or name in {"FPGA_PLL_1V2", "FPGA_AUX_2V5"}
        ):
            net.SetNetClass(classes["POWER"])


def router_class_for_net(name: str) -> str:
    """Return the explicit Specctra class used by the external router."""
    if (
        name.startswith(("RF_HA", "RF_HB", "RF_HC", "RF_HD", "RF_HIGH"))
        or name in {
            "LTC_RF_IN",
            "LTC_LO_IN",
            "LO_HIGH_P",
            "LO_HIGH_N_TERM",
            "LMX_LO_HIGH_P_RAW",
            "LMX_LO_HIGH_N_RAW",
        }
    ):
        return "RF_CRITICAL"
    if (
        name.startswith((
            "RF_", "LO_", "LMX_LO", "ADF_LO2", "LTC_", "IF_HIGH", "IF_LOW",
            "IF_SELECTED", "IF_BAW", "GRF2013_RFIN", "GRF2013_RFOUT",
            "IF_GAIN", "PE4312", "IF_DSA", "LT_INPUT", "LT_IN",
        ))
    ):
        return "RF_50"
    if name.startswith(("IF2", "AD8351_IN", "AD8351_OUT", "ADC_VIN")):
        return "IF_DIFF"
    if name.startswith(("RGMII", "ADC_D", "ADC_CLK", "REF_100M")):
        return "DIGITAL_HS"
    if name.startswith(("+", "VIN_", "SW_")) or name in {
        "VIN_5V", "VIN_FUSED", "RP_CORE",
        "PHY_AVDDH_3V3", "PHY_AVDDL_1V2", "PHY_PLL_1V2",
    } or name.startswith("FPGA_VCC") or name in {
        "FPGA_PLL_1V2", "FPGA_AUX_2V5",
    }:
        return "POWER"
    return "DEFAULT"


def _format_dsn_class(
    name: str,
    nets: list[str],
    width_um: int,
    clearance_um: int,
    layers: tuple[str, ...],
) -> str:
    """Create one Freerouting-compatible Specctra net-class scope."""
    lines = [f"    (class {name}"]
    for index in range(0, len(nets), 8):
        lines.append("      " + " ".join(nets[index:index + 8]))
    lines.extend([
        "      (circuit",
        '        (use_via "Via[0-3]_600:300_um")',
        "        (use_layer " + " ".join(layers) + ")",
        "      )",
        "      (rule",
        f"        (width {width_um})",
        f"        (clearance {clearance_um})",
        "      )",
        "    )",
    ])
    return "\n".join(lines) + "\n"


def _remove_dsn_net(source: str, net_name: str) -> str:
    """Remove one balanced Specctra net scope from an exported DSN."""
    marker = f"    (net {net_name}\n"
    start = source.find(marker)
    if start < 0:
        raise RuntimeError(f"Cannot locate {net_name} net in DSN")
    depth = 0
    for index in range(start, len(source)):
        character = source[index]
        if character == "(":
            depth += 1
        elif character == ")":
            depth -= 1
            if depth == 0:
                end = index + 1
                if end < len(source) and source[end] == "\n":
                    end += 1
                return source[:start] + source[end:]
    raise RuntimeError(f"Unbalanced {net_name} net scope in DSN")


def enforce_router_contract() -> None:
    """Prevent the autorouter from cutting the reference plane.

    KiCad 9 exports every copper layer as a signal layer and collapses Python
    API net-class assignments into one Specctra class.  Rewrite only those two
    deterministic DSN sections: In1 is a plane, critical RF is F.Cu-only, and
    all other tracks may use F.Cu/In2.Cu/B.Cu.
    """
    source = DSN_PATH.read_text(encoding="utf-8")
    in1_signal = """\
    (layer In1.Cu
      (type signal)
"""
    in1_power = """\
    (layer In1.Cu
      (type power)
"""
    if in1_signal not in source:
        raise RuntimeError("Cannot locate In1.Cu signal-layer declaration in DSN")
    source = source.replace(in1_signal, in1_power, 1)
    source = source.replace(
        """\
    (rule
      (width 200)
      (clearance 200)
      (clearance 50 (type smd_smd))
    )
""",
        """\
    (rule
      (width 150)
      (clearance 100)
      (clearance 50 (type smd_smd))
    )
""",
        1,
    )

    # Ground is connected by the solid In1 plane plus filled F/B pours.  Do
    # not make the track router spend most of its passes on hundreds of ground
    # pads or create unnecessary reference-plane stubs.
    source = _remove_dsn_net(source, "GND")

    grouped: dict[str, list[str]] = defaultdict(list)
    for net_name in all_nets():
        if net_name == "GND":
            continue
        grouped[router_class_for_net(net_name)].append(net_name)
    class_specs = [
        ("RF_CRITICAL", 350, 180, ("F.Cu",)),
        ("RF_50", 350, 180, ("F.Cu", "In2.Cu", "B.Cu")),
        ("IF_DIFF", 200, 150, ("F.Cu", "In2.Cu", "B.Cu")),
        ("DIGITAL_HS", 150, 120, ("F.Cu", "In2.Cu", "B.Cu")),
        ("POWER", 600, 150, ("F.Cu", "In2.Cu", "B.Cu")),
        ("DEFAULT", 150, 100, ("F.Cu", "In2.Cu", "B.Cu")),
    ]
    class_text = "".join(
        _format_dsn_class(class_name, sorted(grouped[class_name]), width, clearance, layers)
        for class_name, width, clearance, layers in class_specs
    )
    class_start = source.find("    (class kicad_default ")
    network_close = source.find("  )\n  (wiring", class_start)
    if class_start < 0 or network_close < 0:
        raise RuntimeError("Cannot locate KiCad default DSN class")
    source = source[:class_start] + class_text + source[network_close:]
    DSN_PATH.write_text(source, encoding="utf-8")


def inject_stackup() -> None:
    """Add the controlled, low-cost four-layer impedance stack to the board."""
    stackup = """\
\t\t(stackup
\t\t\t(layer "F.SilkS" (type "Top Silk Screen"))
\t\t\t(layer "F.Paste" (type "Top Solder Paste"))
\t\t\t(layer "F.Mask" (type "Top Solder Mask") (thickness 0.010))
\t\t\t(layer "F.Cu" (type "copper") (thickness 0.035))
\t\t\t(layer "dielectric 1" (type "prepreg") (thickness 0.180) (material "FR4") (epsilon_r 4.2) (loss_tangent 0.020))
\t\t\t(layer "In1.Cu" (type "copper") (thickness 0.035))
\t\t\t(layer "dielectric 2" (type "core") (thickness 1.090) (material "FR4") (epsilon_r 4.2) (loss_tangent 0.020))
\t\t\t(layer "In2.Cu" (type "copper") (thickness 0.035))
\t\t\t(layer "dielectric 3" (type "prepreg") (thickness 0.180) (material "FR4") (epsilon_r 4.2) (loss_tangent 0.020))
\t\t\t(layer "B.Cu" (type "copper") (thickness 0.035))
\t\t\t(layer "B.Mask" (type "Bottom Solder Mask") (thickness 0.010))
\t\t\t(layer "B.Paste" (type "Bottom Solder Paste"))
\t\t\t(layer "B.SilkS" (type "Bottom Silk Screen"))
\t\t\t(copper_finish "ENIG")
\t\t\t(dielectric_constraints yes)
\t\t)
"""
    source = BOARD_PATH.read_text(encoding="utf-8")
    marker = "\t(setup\n"
    if marker not in source:
        raise RuntimeError("Cannot locate KiCad board setup block for stackup")
    BOARD_PATH.write_text(source.replace(marker, marker + stackup, 1), encoding="utf-8")


def enforce_project_rules() -> None:
    """Keep low-cost 0.10 mm/0.20 mm-finished-hole rules deterministic."""
    project = json.loads(PROJECT_PATH.read_text(encoding="utf-8"))
    design_settings = project.setdefault("board", {}).setdefault("design_settings", {})
    rules = design_settings.setdefault("rules", {})
    rules.update({
        "allow_blind_buried_vias": False,
        "allow_microvias": False,
        "min_clearance": 0.10,
        "min_copper_edge_clearance": 0.00,
        "min_hole_clearance": 0.10,
        "min_hole_to_hole": 0.10,
        "min_through_hole_diameter": 0.15,
        "min_track_width": 0.10,
        "min_via_annular_width": 0.10,
        "min_via_diameter": 0.40,
    })
    classes = project.setdefault("net_settings", {}).setdefault("classes", [])
    default_class = next((entry for entry in classes if entry.get("name") == "Default"), None)
    if default_class is None:
        default_class = {"name": "Default", "priority": 2147483647}
        classes.append(default_class)
    default_class.update({
        "bus_width": 12,
        "clearance": 0.10,
        "diff_pair_gap": 0.15,
        "diff_pair_via_gap": 0.20,
        "diff_pair_width": 0.15,
        "line_style": 0,
        "microvia_diameter": 0.30,
        "microvia_drill": 0.10,
        "pcb_color": "rgba(0, 0, 0, 0.000)",
        "priority": 2147483647,
        "schematic_color": "rgba(0, 0, 0, 0.000)",
        "track_width": 0.15,
        "via_diameter": 0.45,
        "via_drill": 0.20,
        "wire_width": 6,
    })
    for name, clearance, width, via, drill, diff_width, diff_gap in [
        ("RF_50", 0.18, 0.35, 0.60, 0.30, 0.20, 0.20),
        ("IF_DIFF", 0.15, 0.20, 0.50, 0.25, 0.20, 0.20),
        ("DIGITAL_HS", 0.12, 0.15, 0.45, 0.20, 0.15, 0.15),
        ("POWER", 0.15, 0.60, 0.80, 0.40, 0.20, 0.20),
    ]:
        entry = next((item for item in classes if item.get("name") == name), None)
        if entry is None:
            entry = {"name": name}
            classes.append(entry)
        entry.update({
            "bus_width": 12,
            "clearance": clearance,
            "diff_pair_gap": diff_gap,
            "diff_pair_via_gap": 0.20,
            "diff_pair_width": diff_width,
            "line_style": 0,
            "microvia_diameter": 0.30,
            "microvia_drill": 0.10,
            "pcb_color": "rgba(0, 0, 0, 0.000)",
            "priority": 2147483647,
            "schematic_color": "rgba(0, 0, 0, 0.000)",
            "track_width": width,
            "via_diameter": via,
            "via_drill": drill,
            "wire_width": 6,
        })
    PROJECT_PATH.write_text(json.dumps(project, indent=2) + "\n", encoding="utf-8")


def main() -> None:
    board = pcbnew.BOARD()
    board.SetCopperLayerCount(4)
    board.SetFileName(str(BOARD_PATH))
    board.GetTitleBlock().SetTitle("Code-SDR V2 100 MHz to 10 GHz receiver/control board")
    board.GetTitleBlock().SetRevision("2.0")
    board.GetTitleBlock().SetCompany("Code-SDR")
    board.GetTitleBlock().SetComment(0, "CONTROL/SDR BOARD ONLY - ESC IS A DISTINCT EXTERNAL BOARD")

    settings = board.GetDesignSettings()
    settings.m_MinClearance = pcbnew.FromMM(0.10)
    settings.m_TrackMinWidth = pcbnew.FromMM(0.10)
    settings.m_MinThroughDrill = pcbnew.FromMM(0.15)
    settings.m_HoleClearance = pcbnew.FromMM(0.10)
    settings.m_HoleToHoleMin = pcbnew.FromMM(0.10)
    settings.m_ViasMinSize = pcbnew.FromMM(0.40)
    settings.m_ViasMinAnnularWidth = pcbnew.FromMM(0.10)
    settings.m_CopperEdgeClearance = pcbnew.FromMM(0.00)
    settings.m_MinSilkTextHeight = pcbnew.FromMM(0.50)
    settings.m_MinSilkTextThickness = pcbnew.FromMM(0.08)
    settings.m_SolderMaskMinWidth = pcbnew.FromMM(0.05)
    settings.SetCustomTrackWidth(pcbnew.FromMM(0.15))
    settings.SetCustomViaSize(pcbnew.FromMM(0.45))
    settings.SetCustomViaDrill(pcbnew.FromMM(0.20))
    settings.UseCustomTrackViaSize(True)
    default_class = pcbnew.NETCLASS("Default")
    default_class.SetClearance(pcbnew.FromMM(0.10))
    default_class.SetTrackWidth(pcbnew.FromMM(0.15))
    default_class.SetViaDiameter(pcbnew.FromMM(0.45))
    default_class.SetViaDrill(pcbnew.FromMM(0.20))
    board.GetNetClasses()["Default"] = default_class

    net_items: dict[str, pcbnew.NETINFO_ITEM] = {}
    for name in all_nets():
        net = pcbnew.NETINFO_ITEM(board, name)
        board.Add(net)
        net_items[name] = net
    configure_netclasses(board, net_items)

    for sheet, _ in SHEETS:
        place_region(board, sheet, net_items)
    place_overrides(board, net_items)

    add_edge(board, (0, 0), (W, 0))
    add_edge(board, (W, 0), (W, H))
    add_edge(board, (W, H), (0, H))
    add_edge(board, (0, H), (0, 0))

    add_text(board, "CODE-SDR V2 / 100 MHz - 10 GHz", 31, 98, 1.2, pcbnew.Cmts_User)
    add_text(board, "RECEIVER + CONTROL BOARD / NO ESC", 87, 98, 1.0, pcbnew.Cmts_User)
    for sheet, title in SHEETS:
        x, y, width, height = REGIONS[sheet]
        add_text(
            board,
            f"{sheet} / {title}".upper(),
            x + 0.4,
            y + height - 0.6,
            0.55,
            pcbnew.Cmts_User,
        )

    # In1.Cu is the uninterrupted RF/digital reference plane.  Front/back
    # pours shorten ground return paths; power and signals route on F/B/In2.
    add_ground_zone(board, net_items["GND"], pcbnew.In1_Cu, 0.25)
    add_ground_zone(board, net_items["GND"], pcbnew.F_Cu, 0.35)
    add_ground_zone(board, net_items["GND"], pcbnew.B_Cu, 0.35)

    ROOT.joinpath("build").mkdir(parents=True, exist_ok=True)
    pcbnew.SaveBoard(str(BOARD_PATH), board)
    inject_stackup()
    if not pcbnew.ExportSpecctraDSN(board, str(DSN_PATH)):
        raise RuntimeError("KiCad failed to export Specctra DSN")
    enforce_router_contract()
    enforce_project_rules()
    print(
        f"Generated {BOARD_PATH.name}: {len(components)} footprints, "
        f"{len(all_nets())} nets, four copper layers, {W:.0f}x{H:.0f} mm"
    )
    print(f"Exported autorouter input {DSN_PATH}")


if __name__ == "__main__":
    main()

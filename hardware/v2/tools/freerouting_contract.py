"""Preservation and DSN constraints for one controlled Freerouting pass."""

from __future__ import annotations

from collections import Counter
from dataclasses import dataclass
from pathlib import Path
import re
from typing import Iterable


PAIR_SPECS = (
    ("USB connector", ("USB_DP_CONN",), ("USB_DM_CONN",), 1.00),
    ("USB device", ("USB_DP",), ("USB_DM",), 1.00),
    ("Ethernet MDI A", ("MDI_A_P",), ("MDI_A_N",), 1.00),
    ("Ethernet MDI B", ("MDI_B_P",), ("MDI_B_N",), 1.00),
    ("Ethernet MDI C", ("MDI_C_P",), ("MDI_C_N",), 1.00),
    ("Ethernet MDI D", ("MDI_D_P",), ("MDI_D_N",), 1.00),
    ("LMX low-band LO", ("LMX_LO_LOW_P_RAW",), ("LMX_LO_LOW_N_RAW",), 0.75),
    ("ADF second LO", ("ADF_LO2_P_RAW",), ("ADF_LO2_N_RAW",), 0.75),
    ("Second LO", ("LO2_P",), ("LO2_N",), 0.75),
    ("Low-band LO", ("LO_LOW_P",), ("LO_LOW_N",), 0.75),
    ("Low-band IF", ("IF_LOW_P",), ("IF_LOW_N",), 0.75),
    ("High-band IF", ("IF_HIGH_P",), ("IF_HIGH_N",), 0.75),
    ("LT5560 RF", ("LT_IN_P",), ("LT_IN_N",), 0.75),
    ("Second IF", ("IF2_P",), ("IF2_N",), 0.50),
    ("IF2 filter 1", ("IF2_F1_P",), ("IF2_F1_N",), 0.50),
    ("IF2 filter 2", ("IF2_F2_P",), ("IF2_F2_N",), 0.50),
    ("IF2 filter 3", ("IF2_F3_P",), ("IF2_F3_N",), 0.50),
    ("IF2 filter 4", ("IF2_F4_P",), ("IF2_F4_N",), 0.50),
    ("IF2 filter output", ("IF2_LPF_P",), ("IF2_LPF_N",), 0.50),
    ("ADC driver input", ("AD8351_IN_P",), ("AD8351_IN_N",), 0.50),
    ("ADC driver output", ("AD8351_OUT_P",), ("AD8351_OUT_N",), 0.50),
    ("ADC analog input", ("ADC_VIN_P",), ("ADC_VIN_N",), 0.50),
)


@dataclass(frozen=True)
class PairMeasurement:
    name: str
    positive: tuple[str, ...]
    negative: tuple[str, ...]
    tolerance_mm: float
    positive_length_mm: float
    negative_length_mm: float
    positive_vias: int
    negative_vias: int
    positive_segments: int
    negative_segments: int

    @property
    def skew_mm(self) -> float:
        return abs(self.positive_length_mm - self.negative_length_mm)

    @property
    def connected(self) -> bool:
        return self.positive_segments > 0 and self.negative_segments > 0

    @property
    def editable(self) -> bool:
        return (
            not self.connected
            or self.skew_mm > self.tolerance_mm + 1e-6
            or self.positive_vias != self.negative_vias
        )


@dataclass(frozen=True)
class BoardSnapshot:
    footprints: tuple[tuple[object, ...], ...]
    zones: tuple[tuple[object, ...], ...]
    protected_items: Counter[tuple[object, ...]]
    rf_protected_items: Counter[tuple[object, ...]]
    internal_items: Counter[tuple[object, ...]]
    pair_measurements: dict[str, PairMeasurement]
    editable_nets: frozenset[str]


def _scope_end(source: str, start: int) -> int:
    depth = 0
    for index in range(start, len(source)):
        if source[index] == "(":
            depth += 1
        elif source[index] == ")":
            depth -= 1
            if depth == 0:
                return index + 1
    raise ValueError("unbalanced Specctra scope")


def _scopes(source: str, name: str) -> Iterable[tuple[int, int, str]]:
    marker = f"({name}"
    offset = 0
    while True:
        start = source.find(marker, offset)
        if start < 0:
            return
        end = _scope_end(source, start)
        yield start, end, source[start:end]
        offset = end


def _net_from_wiring_scope(scope: str) -> str | None:
    match = re.search(r"\(net\s+([^\s\)]+)\)", scope)
    return match.group(1) if match else None


def _protect_wires(source: str, editable_nets: set[str]) -> str:
    pieces: list[str] = []
    cursor = 0
    for start, end, scope in _scopes(source, "wire"):
        pieces.append(source[cursor:start])
        net_name = _net_from_wiring_scope(scope)
        if net_name and net_name not in editable_nets:
            if "(type route)" in scope:
                scope = scope.replace("(type route)", "(type protect)")
            elif "(type protect)" not in scope:
                scope = re.sub(r"(\(net\s+[^\s\)]+\))", r"\1 (type protect)", scope, count=1)
        pieces.append(scope)
        cursor = end
    pieces.append(source[cursor:])
    return "".join(pieces)


def _mark_internal_layers_power(source: str) -> str:
    for layer in ("In1.Cu", "In2.Cu"):
        source = source.replace(
            f"(layer {layer}\n      (type signal)",
            f"(layer {layer}\n      (type power)",
        )
    return source


def _limit_route_layers(source: str) -> str:
    source = re.sub(r"\(use_layer\s+[^\)]+\)", "(use_layer F.Cu B.Cu)", source)
    pattern = r"(\(circuit\s*\n\s*\(use_via\s+[^\)]+\))"
    return re.sub(pattern, r"\1\n        (use_layer F.Cu B.Cu)", source)


def _priority_class(net_name: str) -> str:
    """Return the router rule bucket for an unlocked differential net."""
    if net_name.startswith(("USB_", "MDI_")):
        return "DIFF_DIGITAL"
    if net_name.startswith(("IF2_", "AD8351_", "ADC_VIN")):
        return "DIFF_IF"
    return "DIFF_RF"


def _priority_sort_key(net_name: str) -> tuple[int, str]:
    family = 0 if net_name.startswith("USB_") else 1 if net_name.startswith("MDI_") else 2
    return (family, net_name)


def _rewrite_classes(source: str, editable_nets: set[str], route_only: bool = False) -> str:
    """Put editable differential nets in correctly-sized classes before default.

    Freerouting uses class declaration order as its routing order.  Removing
    these nets from the KiCad default class avoids duplicate class membership;
    removing GND avoids creating any new ground copper in the pass.
    """
    default_scope: tuple[int, int, str] | None = None
    for candidate in _scopes(source, "class"):
        if re.match(r"\(class\s+kicad_default(?:\s|\))", candidate[2]):
            default_scope = candidate
            break
    if default_scope is None:
        raise ValueError("DSN has no kicad_default class")

    start, end, scope = default_scope
    header_end = scope.find("\n")
    if header_end < 0:
        raise ValueError("malformed kicad_default class")
    header, body = scope[:header_end], scope[header_end:]
    if route_only:
        header = "(class kicad_default"
    else:
        for net_name in sorted((*editable_nets, "GND"), key=len, reverse=True):
            header = re.sub(rf"(?<!\S){re.escape(net_name)}(?!\S)", "", header)
    header = re.sub(r"[ \t]+", " ", header).rstrip()
    rewritten_default = header + body

    buckets: dict[str, list[str]] = {"DIFF_DIGITAL": [], "DIFF_IF": [], "DIFF_RF": []}
    for net_name in editable_nets:
        buckets[_priority_class(net_name)].append(net_name)
    rule = {
        "DIFF_DIGITAL": (200, 120),
        "DIFF_IF": (200, 150),
        "DIFF_RF": (230, 180),
    }
    classes: list[str] = []
    for class_name in ("DIFF_DIGITAL", "DIFF_IF", "DIFF_RF"):
        names = sorted(buckets[class_name], key=_priority_sort_key)
        if not names:
            continue
        width, clearance = rule[class_name]
        classes.append(
            f"(class {class_name} {' '.join(names)}\n"
            "      (circuit\n"
            "        (use_via \"Via[0-3]_450:200_um\")\n"
            "        (use_layer F.Cu B.Cu)\n"
            "      )\n"
            f"      (rule (width {width})(clearance {clearance}))\n"
            "    )"
        )
    priority_text = "\n    ".join(classes)
    if priority_text:
        priority_text += "\n    "
    return source[:start] + priority_text + rewritten_default + source[end:]


def transform_dsn(source: str, editable_nets: set[str], route_only: bool = False) -> str:
    """Protect prior non-editable wiring and bar new inner-layer signals."""
    return _limit_route_layers(
        _mark_internal_layers_power(
            _rewrite_classes(_protect_wires(source, editable_nets), editable_nets, route_only)
        )
    )


def _point(point: object) -> tuple[int, int]:
    return (int(point.x), int(point.y))


def _track_signature(board: object, item: object, pcbnew: object) -> tuple[object, ...]:
    net = item.GetNetname()
    if isinstance(item, pcbnew.PCB_VIA):
        pos = _point(item.GetPosition())
        return (
        "via", net, pos, int(item.TopLayer()), int(item.BottomLayer()),
            int(item.GetWidth(pcbnew.F_Cu)), int(item.GetDrillValue()),
        )
    start, end = sorted((_point(item.GetStart()), _point(item.GetEnd())))
    return ("segment", net, int(item.GetLayer()), start, end, int(item.GetWidth()))


def _zone_signature(zone: object) -> tuple[object, ...]:
    bbox = zone.GetBoundingBox()
    return (
        zone.GetNetname(), int(zone.GetLayer()), int(zone.GetAssignedPriority()),
        int(zone.GetMinThickness()), int(zone.GetLocalClearance()),
        int(bbox.GetX()), int(bbox.GetY()), int(bbox.GetWidth()), int(bbox.GetHeight()),
    )


def _footprint_signature(footprint: object) -> tuple[object, ...]:
    pos = footprint.GetPosition()
    orientation = footprint.GetOrientation().AsDegrees()
    return (footprint.GetReference(), int(pos.x), int(pos.y), round(orientation, 6), int(footprint.GetLayer()))


def measure_pairs(board_path: Path) -> dict[str, PairMeasurement]:
    import pcbnew

    board = pcbnew.LoadBoard(str(board_path))
    lengths: Counter[str] = Counter()
    vias: Counter[str] = Counter()
    segments: Counter[str] = Counter()
    for item in board.GetTracks():
        name = item.GetNetname()
        if isinstance(item, pcbnew.PCB_VIA):
            vias[name] += 1
        else:
            segments[name] += 1
            lengths[name] += pcbnew.ToMM(item.GetLength())
    result: dict[str, PairMeasurement] = {}
    for name, positive, negative, tolerance in PAIR_SPECS:
        result[name] = PairMeasurement(
            name, positive, negative, tolerance,
            sum(lengths[n] for n in positive), sum(lengths[n] for n in negative),
            sum(vias[n] for n in positive), sum(vias[n] for n in negative),
            sum(segments[n] for n in positive), sum(segments[n] for n in negative),
        )
    return result


def snapshot_board(board_path: Path) -> BoardSnapshot:
    import pcbnew
    from export_rf50_dsn import RF50_NETS

    board = pcbnew.LoadBoard(str(board_path))
    pairs = measure_pairs(board_path)
    editable = frozenset(
        net for pair in pairs.values() if pair.editable for net in (*pair.positive, *pair.negative)
    )
    items = [_track_signature(board, item, pcbnew) for item in board.GetTracks()]
    protected = Counter(item for item in items if item[1] not in editable)
    rf_protected = Counter(item for item in items if item[1] in RF50_NETS and item[1] not in editable)
    internal = Counter(item for item in items if item[0] == "segment" and item[2] in {pcbnew.In1_Cu, pcbnew.In2_Cu})
    return BoardSnapshot(
        tuple(sorted(_footprint_signature(fp) for fp in board.GetFootprints())),
        tuple(sorted(_zone_signature(zone) for zone in board.Zones())),
        protected,
        rf_protected,
        internal,
        pairs,
        editable,
    )


def _counter_errors(label: str, expected: Counter[tuple[object, ...]], actual: Counter[tuple[object, ...]]) -> list[str]:
    missing = expected - actual
    return [f"{label} missing: {signature!r} x{count}" for signature, count in missing.items()]


def compare_preservation(before: BoardSnapshot, candidate_path: Path) -> list[str]:
    import pcbnew
    from export_rf50_dsn import RF50_NETS

    board = pcbnew.LoadBoard(str(candidate_path))
    errors: list[str] = []
    footprints = tuple(sorted(_footprint_signature(fp) for fp in board.GetFootprints()))
    zones = tuple(sorted(_zone_signature(zone) for zone in board.Zones()))
    if footprints != before.footprints:
        errors.append("footprint position/rotation/side changed")
    if zones != before.zones:
        errors.append("zone geometry, layer or net changed")
    items = [_track_signature(board, item, pcbnew) for item in board.GetTracks()]
    actual = Counter(items)
    errors.extend(_counter_errors("protected copper", before.protected_items, actual))
    rf_actual = Counter(item for item in items if item[1] in RF50_NETS and item[1] not in before.editable_nets)
    errors.extend(_counter_errors("protected RF50 copper", before.rf_protected_items, rf_actual))
    internal_actual = Counter(item for item in items if item[0] == "segment" and item[2] in {pcbnew.In1_Cu, pcbnew.In2_Cu})
    new_internal = internal_actual - before.internal_items
    errors.extend(f"new inner-layer signal: {signature!r} x{count}" for signature, count in new_internal.items())
    return errors

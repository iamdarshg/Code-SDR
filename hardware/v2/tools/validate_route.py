"""Check routed-layer invariants and export per-class route statistics."""

from __future__ import annotations

import csv
from collections import defaultdict
from pathlib import Path

import pcbnew

from generate_pcb import router_class_for_net


ROOT = Path(__file__).resolve().parents[1]
BOARD_PATH = ROOT / "Code-SDR-V2.kicad_pcb"
OUT_CSV = ROOT / "build" / "ROUTE_STATISTICS.csv"

MATCHED_PAIRS = (
    (("USB_DP_CONN", "USB_DP"), ("USB_DM_CONN", "USB_DM"), 1.00, "USB D+/D-"),
    (("MDI_A_P",), ("MDI_A_N",), 1.00, "Ethernet MDI A"),
    (("MDI_B_P",), ("MDI_B_N",), 1.00, "Ethernet MDI B"),
    (("MDI_C_P",), ("MDI_C_N",), 1.00, "Ethernet MDI C"),
    (("MDI_D_P",), ("MDI_D_N",), 1.00, "Ethernet MDI D"),
    (("LMX_LO_LOW_P_RAW",), ("LMX_LO_LOW_N_RAW",), 0.75, "low-band LO"),
    (("ADF_LO2_P_RAW",), ("ADF_LO2_N_RAW",), 0.75, "fixed second LO"),
    (("LO2_P",), ("LO2_N",), 0.75, "second-mixer LO"),
    (("LO_LOW_P",), ("LO_LOW_N",), 0.75, "low-band mixer LO"),
    (("IF_LOW_P",), ("IF_LOW_N",), 0.75, "low-band first IF"),
    (("IF_HIGH_P",), ("IF_HIGH_N",), 0.75, "high-band first IF"),
    (("LT_IN_P",), ("LT_IN_N",), 0.75, "LT5560 RF input"),
    (("IF2_P",), ("IF2_N",), 0.50, "second IF input"),
    (("IF2_F1_P",), ("IF2_F1_N",), 0.50, "anti-alias section 1"),
    (("IF2_F2_P",), ("IF2_F2_N",), 0.50, "anti-alias section 2"),
    (("IF2_F3_P",), ("IF2_F3_N",), 0.50, "anti-alias section 3"),
    (("IF2_F4_P",), ("IF2_F4_N",), 0.50, "anti-alias section 4"),
    (("IF2_LPF_P",), ("IF2_LPF_N",), 0.50, "anti-alias output"),
    (("AD8351_IN_P",), ("AD8351_IN_N",), 0.50, "ADC-driver input"),
    (("AD8351_OUT_P",), ("AD8351_OUT_N",), 0.50, "ADC-driver output"),
    (("ADC_VIN_P",), ("ADC_VIN_N",), 0.50, "ADC analog input"),
)


def main() -> None:
    board = pcbnew.LoadBoard(str(BOARD_PATH))
    rows: dict[tuple[str, str], dict[str, float | int | str]] = defaultdict(
        lambda: {
            "track_segments": 0,
            "vias": 0,
            "length_mm": 0.0,
            "layers": set(),
        }
    )
    forbidden_plane_items: list[str] = []
    critical_vias: list[str] = []
    critical_wrong_layers: list[str] = []

    for item in board.GetTracks():
        net_name = item.GetNetname()
        route_class = router_class_for_net(net_name)
        key = (route_class, net_name)
        layer_name = board.GetLayerName(item.GetLayer())
        rows[key]["layers"].add(layer_name)  # type: ignore[union-attr]
        if isinstance(item, pcbnew.PCB_VIA):
            rows[key]["vias"] = int(rows[key]["vias"]) + 1
            if route_class == "RF_CRITICAL":
                critical_vias.append(net_name)
        else:
            rows[key]["track_segments"] = int(rows[key]["track_segments"]) + 1
            rows[key]["length_mm"] = float(rows[key]["length_mm"]) + pcbnew.ToMM(
                item.GetLength()
            )
            if item.GetLayer() == pcbnew.In1_Cu:
                forbidden_plane_items.append(net_name)
            if route_class == "RF_CRITICAL" and item.GetLayer() != pcbnew.F_Cu:
                critical_wrong_layers.append(f"{net_name}:{layer_name}")

    with OUT_CSV.open("w", newline="", encoding="utf-8") as handle:
        fieldnames = (
            "route_class",
            "net",
            "track_segments",
            "vias",
            "length_mm",
            "layers",
        )
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        for (route_class, net_name), data in sorted(rows.items()):
            writer.writerow(
                {
                    "route_class": route_class,
                    "net": net_name,
                    "track_segments": data["track_segments"],
                    "vias": data["vias"],
                    "length_mm": f"{float(data['length_mm']):.3f}",
                    "layers": ",".join(sorted(data["layers"])),  # type: ignore[arg-type]
                }
            )

    if forbidden_plane_items:
        raise AssertionError(
            "Signal routing exists on solid In1 ground plane: "
            + ", ".join(sorted(set(forbidden_plane_items)))
        )
    if critical_vias:
        raise AssertionError(
            "Critical RF route contains vias: " + ", ".join(sorted(set(critical_vias)))
        )
    if critical_wrong_layers:
        raise AssertionError(
            "Critical RF route left F.Cu: " + ", ".join(sorted(set(critical_wrong_layers)))
        )

    def aggregate(net_names: tuple[str, ...]) -> tuple[float, int]:
        length = 0.0
        vias = 0
        for net_name in net_names:
            data = rows.get((router_class_for_net(net_name), net_name))
            if data is None:
                raise AssertionError(f"Routed net is missing from statistics: {net_name}")
            length += float(data["length_mm"])
            vias += int(data["vias"])
        return length, vias

    matching_errors: list[str] = []
    for positive, negative, tolerance_mm, description in MATCHED_PAIRS:
        positive_length, positive_vias = aggregate(positive)
        negative_length, negative_vias = aggregate(negative)
        skew = abs(positive_length - negative_length)
        if skew > tolerance_mm or positive_vias != negative_vias:
            matching_errors.append(
                f"{description}: {positive_length:.3f}/{negative_length:.3f} mm, "
                f"skew {skew:.3f} mm, vias {positive_vias}/{negative_vias}"
            )

    def bus_lengths(signal_names: tuple[str, ...]) -> list[float]:
        return [aggregate((name, f"{name}_FPGA"))[0] for name in signal_names]

    adc_lengths = [aggregate((f"ADC_D{index}",))[0] for index in range(10)]
    if max(adc_lengths) - min(adc_lengths) > 5.0:
        matching_errors.append(
            "ADC data-bus skew exceeds 5.0 mm: "
            + ", ".join(f"D{index}={length:.3f}" for index, length in enumerate(adc_lengths))
        )

    rgmii_tx_names = (
        "RGMII_TXD0", "RGMII_TXD1", "RGMII_TXD2", "RGMII_TXD3", "RGMII_TX_EN"
    )
    rgmii_rx_names = (
        "RGMII_RXD0", "RGMII_RXD1", "RGMII_RXD2", "RGMII_RXD3", "RGMII_RX_DV"
    )
    for description, signal_names in (
        ("RGMII TX", rgmii_tx_names),
        ("RGMII RX", rgmii_rx_names),
    ):
        lengths = bus_lengths(signal_names)
        if max(lengths) - min(lengths) > 5.0:
            matching_errors.append(
                f"{description} data/control skew exceeds 5.0 mm: "
                + ", ".join(
                    f"{name}={length:.3f}"
                    for name, length in zip(signal_names, lengths)
                )
            )

    if matching_errors:
        raise AssertionError(
            "Length/via matching checks failed:\n  " + "\n  ".join(matching_errors)
        )

    track_segments = sum(int(data["track_segments"]) for data in rows.values())
    vias = sum(int(data["vias"]) for data in rows.values())
    print(
        f"Route invariants valid: {track_segments} segments, {vias} vias, "
        "solid In1 preserved, critical RF F.Cu-only, high-speed skew valid"
    )


if __name__ == "__main__":
    main()

"""Return-path and reference-plane analysis for Code-SDR V2 RF50 nets.

Analytical (design-rule / geometry) checks, NOT full-wave:

1. Reference-plane contract: F.Cu -> In1.Cu solid GND; B.Cu -> In2.Cu is a
   SPLIT POWER plane (+3V3_ANA / +3V3_DIG), no GND reference directly beneath.
2. Any RF50 net routed on B.Cu therefore lacks a solid contiguous ground
   reference and must be treated as a return-path risk until field-solved or
   re-routed. This matches the repo's own documented requirement in the v2
   README ("B.Cu controlled signals need an explicitly reviewed return path").
3. Via usage in RF50 nets: vias interrupt the outer-layer microstrip
   reference and require co-located ground-return vias; count stitching vias.
4. In2 split crossing: compute whether each B.Cu RF50 track crosses the
   measured split line (y ~ 45.3 mm) and flag it.

Inputs: pcbnew-load of the committed board copy (byte-identical to repo).
"""

from __future__ import annotations

import json
import math
import sys
from pathlib import Path

import pcbnew  # kicad python only

EV = Path(__file__).resolve().parents[1]
BOARD = EV / "scripts" / "v2" / "Code-SDR-V2.kicad_pcb"
OUT = EV / "outputs"

sys.path.insert(0, str(EV / "scripts" / "v2" / "tools"))
from export_rf50_dsn import RF50_NETS  # noqa: E402

SPLIT_Y_MM = 45.3  # measured In2 split line (see in2_zones.txt)


def main() -> None:
    board = pcbnew.LoadBoard(str(BOARD))
    track_list = list(board.GetTracks())

    # Collect RF50 track segments and vias, per net, with layer + endpoints.
    nets: dict[str, dict] = {n: {"tracks": [], "vias": [], "length_mm": 0.0}
                             for n in RF50_NETS}
    for t in track_list:
        n = t.GetNetname()
        if n not in RF50_NETS:
            continue
        if isinstance(t, pcbnew.PCB_VIA):
            nets[n]["vias"].append((pcbnew.ToMM(t.GetPosition().x),
                                    pcbnew.ToMM(t.GetPosition().y)))
        else:
            layer = board.GetLayerName(t.GetLayer())
            nets[n]["tracks"].append({
                "layer": layer,
                "width_mm": round(pcbnew.ToMM(t.GetWidth()), 4),
                "a": (pcbnew.ToMM(t.GetStart().x), pcbnew.ToMM(t.GetStart().y)),
                "b": (pcbnew.ToMM(t.GetEnd().x), pcbnew.ToMM(t.GetEnd().y)),
            })
            nets[n]["length_mm"] += pcbnew.ToMM(t.GetLength())

    # Ground vias anywhere on board (stitching budget).
    gnd_vias = [t for t in track_list
                if isinstance(t, pcbnew.PCB_VIA) and t.GetNetname() == "GND"]
    gnd_via_pos = [(pcbnew.ToMM(t.GetPosition().x), pcbnew.ToMM(t.GetPosition().y))
                   for t in gnd_vias]

    results = {"split_line_y_mm": SPLIT_Y_MM, "rf50_nets": {}}

    def near_gnd_via(x: float, y: float, radius_mm: float = 3.0) -> bool:
        for gx, gy in gnd_via_pos:
            if math.hypot(gx - x, gy - y) <= radius_mm:
                return True
        return False

    def crosses_split(a: tuple, b: tuple) -> bool:
        """Segment crosses y = SPLIT_Y_MM within the segment x-range."""
        ymin, ymax = min(a[1], b[1]), max(a[1], b[1])
        if ymin < SPLIT_Y_MM < ymax:
            # interpolate x at split y
            if abs(b[1] - a[1]) < 1e-9:
                return False
            t = (SPLIT_Y_MM - a[1]) / (b[1] - a[1])
            x_at = a[0] + t * (b[0] - a[0])
            return x_at >= 0.0
        return False

    for n in sorted(RF50_NETS):
        data = nets[n]
        layers = sorted({tr["layer"] for tr in data["tracks"]})
        bcu_tracks = [tr for tr in data["tracks"] if tr["layer"] == "B.Cu"]
        crosses = [tr for tr in bcu_tracks if crosses_split(tr["a"], tr["b"])]
        via_near_gnd = [v for v in data["vias"] if near_gnd_via(*v)]
        longest = max((tr for tr in data["tracks"]), key=lambda tr: tr["b"][0] - tr["a"][0] or 0,
                      default=None)
        results["rf50_nets"][n] = {
            "length_mm": round(data["length_mm"], 3),
            "layers": layers,
            "on_bcu": bool(bcu_tracks),
            "bcu_segments": len(bcu_tracks),
            "bcu_crosses_in2_split": len(crosses),
            "vias": len(data["vias"]),
            "vias_with_gnd_stitch_within_3mm": len(via_near_gnd),
        }

    summary = {
        "total_rf50_nets": len(RF50_NETS),
        "nets_on_bcu": [n for n in results["rf50_nets"]
                        if results["rf50_nets"][n]["on_bcu"]],
        "nets_with_vias": [n for n in results["rf50_nets"]
                           if results["rf50_nets"][n]["vias"] > 0],
        "nets_crossing_in2_split_on_bcu": [n for n in results["rf50_nets"]
                                           if results["rf50_nets"][n]["bcu_crosses_in2_split"] > 0],
        "board_gnd_vias_total": len(gnd_via_pos),
    }
    results["summary"] = summary

    (OUT / "return_path_analysis.json").write_text(json.dumps(results, indent=1), encoding="utf-8")

    print("=== RETURN-PATH / REFERENCE-PLANE ANALYSIS (analytical) ===")
    print(f"In2 split line y={SPLIT_Y_MM} mm (+3V3_DIG above, +3V3_ANA below).")
    print(f"Board GND vias total: {len(gnd_via_pos)}")
    print("RF50 nets on B.Cu (reference = split power plane, NOT GND):")
    for n in summary["nets_on_bcu"]:
        d = results["rf50_nets"][n]
        print(f"  {n:16s} len {d['length_mm']:7.2f} mm  bcu_seg {d['bcu_segments']} "
              f"cross_split {d['bcu_crosses_in2_split']}  vias {d['vias']}  "
              f"stitch_reachable {d['vias_with_gnd_stitch_within_3mm']}")
    print("RF50 nets with vias:")
    for n in summary["nets_with_vias"]:
        d = results["rf50_nets"][n]
        print(f"  {n:16s} vias {d['vias']}  stitch_reachable {d['vias_with_gnd_stitch_within_3mm']}")
    print(f"Nets crossing In2 split on B.Cu: {summary['nets_crossing_in2_split_on_bcu'] or 'none'}")
    print("NOTE: analytical geometry audit only. Return-path adequacy requires",
          "full-wave/field solve or VNA/TDR validation. Failing = no claim.")


if __name__ == "__main__":
    main()
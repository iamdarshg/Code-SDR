#!/usr/bin/env python3
"""Add a grid of GND stitching vias tying the F.Cu / In1.Cu / B.Cu ground
pours together.

The board already has an excellent ground system -- a solid In1.Cu GND
plane (96% coverage) plus GND flood pours on both outer layers -- but those
three GND regions are currently connected ONLY through shared component
pads (GND has zero vias). Stitching vias distributed across the board tie
the layers together directly, which shortens signal return-current paths,
suppresses ground bounce, and improves plane-to-plane coupling. For an SDR
mixing sensitive RF/analog with noisy digital/FPGA/PHY switching, this is a
standard and worthwhile ground-integrity improvement.

SAFETY: this only ADDS GND vias, and only at grid points that are (a)
inside the GND fill on In1.Cu AND at least one outer layer, and (b) clear
of every other-net pad and track by a comfortable margin. It never moves or
removes existing copper, so connectivity cannot regress.
"""
from __future__ import annotations

import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import auto_router as ar
import pcbnew

ROOT = Path(__file__).resolve().parents[1]
PCB_PATH = ROOT / "Code-SDR-V2.kicad_pcb"

VIA_DIAM_MM = 0.6
VIA_DRILL_MM = 0.3
GRID_PITCH_MM = 5.0
# how much clearance a candidate stitching via needs from any other-net
# pad/track (measured centre-to-edge). Deliberately conservative -- a
# through-via pierces every layer, so it must clear signal tracks on In2.Cu
# (which has no GND pour) as well as the outer layers. A tight margin let
# ~3.5% of vias land close enough to signal traces to register as
# tracks_crossing; this larger margin trades a few via sites for zero new
# crossing/clearance violations.
CLEAR_MM = 0.5


def mm(v):
    return pcbnew.ToMM(v)


def to_iu(v):
    return pcbnew.FromMM(v)


def main():
    board = pcbnew.LoadBoard(str(PCB_PATH))
    t_start = time.time()

    gnd_net = board.FindNet("GND")
    if gnd_net is None:
        print("no GND net?!")
        return

    gnd_zone_by_layer = {}
    for z in board.Zones():
        if z.GetNetname() != "GND":
            continue
        ls = z.GetLayerSet()
        for l in (pcbnew.F_Cu, pcbnew.In1_Cu, pcbnew.B_Cu):
            if ls.Contains(l) and z.HasFilledPolysForLayer(l):
                gnd_zone_by_layer.setdefault(l, []).append(z)
    print("GND pours found on layers:", sorted(gnd_zone_by_layer.keys()))
    if pcbnew.In1_Cu not in gnd_zone_by_layer:
        print("no In1.Cu GND plane -- aborting")
        return

    # obstacle model for clearance checks (pads + tracks of OTHER nets)
    obstacles = ar.Obstacles(board)
    via_r = VIA_DIAM_MM / 2.0
    need = via_r + CLEAR_MM

    def gnd_covered(layer, pos):
        for z in gnd_zone_by_layer.get(layer, []):
            if z.HitTestFilledArea(layer, pos):
                return True
        return False

    def clear_of_others(x, y):
        for (cx, cy, r, layers, net, halfw, halfh, angle) in obstacles.nearby_pad_circles(
                x, y, x, y, need + 2.0, "GND"):
            if (x - cx) ** 2 + (y - cy) ** 2 < (need + max(halfw, halfh)) ** 2:
                return False
        for (sx, sy, ex, ey, r, layer, is_via, net) in obstacles.nearby_track_circles(
                x, y, x, y, need + 2.0, "GND"):
            if ar.point_seg_dist(x, y, sx, sy, ex, ey) < need + r:
                return False
        return True

    bbox = board.GetBoardEdgesBoundingBox()
    lo_x, hi_x = mm(bbox.GetLeft()) + 2.0, mm(bbox.GetRight()) - 2.0
    lo_y, hi_y = mm(bbox.GetTop()) + 2.0, mm(bbox.GetBottom()) - 2.0

    added = 0
    y = lo_y
    while y <= hi_y:
        x = lo_x
        while x <= hi_x:
            pos = pcbnew.VECTOR2I(to_iu(x), to_iu(y))
            # require In1 plane + at least one outer GND pour at this point
            if gnd_covered(pcbnew.In1_Cu, pos) and (
                    gnd_covered(pcbnew.F_Cu, pos) or gnd_covered(pcbnew.B_Cu, pos)):
                if clear_of_others(x, y):
                    via = pcbnew.PCB_VIA(board)
                    via.SetPosition(pos)
                    via.SetWidth(to_iu(VIA_DIAM_MM))
                    via.SetDrill(to_iu(VIA_DRILL_MM))
                    via.SetNet(gnd_net)
                    via.SetViaType(pcbnew.VIATYPE_THROUGH)
                    board.Add(via)
                    added += 1
            x += GRID_PITCH_MM
        y += GRID_PITCH_MM

    print(f"Added {added} GND stitching vias on a {GRID_PITCH_MM}mm grid.")

    # refill so the pours bond to the new vias
    filler = pcbnew.ZONE_FILLER(board)
    filler.Fill(board.Zones())

    board.Save(str(PCB_PATH))
    print(f"Saved: {PCB_PATH}")
    print(f"Elapsed: {time.time()-t_start:.0f}s")


if __name__ == "__main__":
    main()

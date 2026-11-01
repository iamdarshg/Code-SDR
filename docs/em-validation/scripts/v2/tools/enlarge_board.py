"""Enlarge the PCB outline and relocate edge-mount connectors outward.

Run against an isolated candidate copy only:

    python enlarge_board.py --board <candidate.kicad_pcb> \
        --left 25 --top 25 --right 25 --bottom 25

Footprints whose pads cross an old edge are translated outward with that
edge so edge-launch/edge-mount mechanics stay intact.  Ground reference
zones are rebuilt to the new outline; In2 power zones are kept as-is.
"""

from __future__ import annotations

import argparse

import pcbnew


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--board", required=True)
    ap.add_argument("--left", type=float, default=0.0)
    ap.add_argument("--top", type=float, default=0.0)
    ap.add_argument("--right", type=float, default=0.0)
    ap.add_argument("--bottom", type=float, default=0.0)
    args = ap.parse_args()

    board = pcbnew.LoadBoard(args.board)

    # Old outline from Edge.Cuts bbox
    edge_box = board.GetBoardEdgesBoundingBox()
    ox0, oy0 = pcbnew.ToMM(edge_box.GetX()), pcbnew.ToMM(edge_box.GetY())
    ox1 = ox0 + pcbnew.ToMM(edge_box.GetWidth())
    oy1 = oy0 + pcbnew.ToMM(edge_box.GetHeight())
    print(f"old outline: ({ox0:.2f},{oy0:.2f})-({ox1:.2f},{oy1:.2f})")

    nx0, ny0 = ox0 - args.left, oy0 - args.top
    nx1, ny1 = ox1 + args.right, oy1 + args.bottom

    tol = 1.5  # mm: pad land may cross edge by design
    moved = []
    for fp in board.GetFootprints():
        bb = fp.GetBoundingBox()
        fx0, fy0 = pcbnew.ToMM(bb.GetX()), pcbnew.ToMM(bb.GetY())
        fx1 = fx0 + pcbnew.ToMM(bb.GetWidth())
        fy1 = fy0 + pcbnew.ToMM(bb.GetHeight())
        dx = dy = 0.0
        crosses_right = fx1 > ox1 - tol and fx0 > ox0 + tol
        crosses_left = fx0 < ox0 + tol and fx1 < ox1 - tol
        crosses_bottom = fy1 > oy1 - tol and fy0 > oy0 + tol
        crosses_top = fy0 < oy0 + tol and fy1 < oy1 - tol
        if crosses_right:
            dx += args.right
        elif crosses_left:
            dx -= args.left
        if crosses_bottom:
            dy += args.bottom
        elif crosses_top:
            dy -= args.top
        if dx or dy:
            fp.Move(pcbnew.VECTOR2I(pcbnew.FromMM(dx), pcbnew.FromMM(dy)))
            moved.append((fp.GetReference(), dx, dy))
    print(f"moved {len(moved)} footprints:", moved)

    # Replace Edge.Cuts rectangle
    for d in list(board.GetDrawings()):
        if int(d.GetLayer()) == int(pcbnew.Edge_Cuts):
            board.Remove(d)

    def add_edge(x1, y1, x2, y2):
        seg = pcbnew.PCB_SHAPE(board)
        seg.SetShape(pcbnew.SHAPE_T_SEGMENT)
        seg.SetStart(pcbnew.VECTOR2I(pcbnew.FromMM(x1), pcbnew.FromMM(y1)))
        seg.SetEnd(pcbnew.VECTOR2I(pcbnew.FromMM(x2), pcbnew.FromMM(y2)))
        seg.SetLayer(int(pcbnew.Edge_Cuts))
        seg.SetWidth(pcbnew.FromMM(0.05))
        board.Add(seg)

    add_edge(nx0, ny0, nx1, ny0)
    add_edge(nx1, ny0, nx1, ny1)
    add_edge(nx1, ny1, nx0, ny1)
    add_edge(nx0, ny1, nx0, ny0)

    # Rebuild GND reference zones (F.Cu and In1) to new outline; drop other
    # GND-copper zones on other layers too so refs stay consistent.
    gnd = board.FindNet("GND")
    if gnd is None:
        raise RuntimeError("GND net missing")
    removed_zones = 0
    zone_snapshot = []
    zones_tuple = board.Zones()
    for index in range(len(zones_tuple)):
        z = zones_tuple[index]
        zone_snapshot.append(z)
    for z in zone_snapshot:
        if z.GetNetname() == "GND":
            board.Remove(z)
            removed_zones += 1
    print(f"removed {removed_zones} old GND zones")

    inset = 0.25
    for layer in (pcbnew.F_Cu, pcbnew.In1_Cu):
        z = pcbnew.ZONE(board)
        z.SetLayer(layer)
        z.SetNet(gnd)
        z.SetPadConnection(pcbnew.ZONE_CONNECTION_FULL)
        z.SetLocalClearance(pcbnew.FromMM(0.15))
        z.SetMinThickness(pcbnew.FromMM(0.12))
        pts = pcbnew.VECTOR_VECTOR2I()
        for x, y in (
            (nx0 + inset, ny0 + inset),
            (nx1 - inset, ny0 + inset),
            (nx1 - inset, ny1 - inset),
            (nx0 + inset, ny1 - inset),
        ):
            pts.append(pcbnew.VECTOR2I(pcbnew.FromMM(x), pcbnew.FromMM(y)))
        z.AddPolygon(pts)
        board.Add(z)

    filler = pcbnew.ZONE_FILLER(board)
    filler.Fill(board.Zones())
    ok = pcbnew.SaveBoard(args.board, board)
    print("saved:", ok, f"new outline: ({nx0:.2f},{ny0:.2f})-({nx1:.2f},{ny1:.2f})")


if __name__ == "__main__":
    main()

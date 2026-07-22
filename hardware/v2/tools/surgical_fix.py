#!/usr/bin/env python3
"""Surgical, per-violation micro-fixes instead of whole-net rip-up-and-reroute.

Whole-net rip-up-and-reroute has been tried repeatedly this session and
plateaus around the same total violation count (clearance keeps landing at
exactly 500 across three independent full-cluster sweeps) -- rerouting a
net just relocates its conflicts elsewhere on a board this dense, it
doesn't remove them. This tool instead:

1. Reads the latest DRC report and ranks the exact copper objects (by
   UUID) causing clearance/shorting_items/tracks_crossing violations.
2. For each offending VIA (worst offenders first), tries nudging it a
   small distance to a nearby position that is clear of everything
   except its own net, then re-stitches the connected track segment
   endpoints to the via's new position.
3. For each offending TRACK segment, removes just that one segment
   (leaving the rest of the net's copper untouched) and re-searches a
   replacement path between the same two endpoints with a stricter
   clearance target, splicing the result back in.

Only ever touches the single flagged object (or the one segment), never
the whole net -- this is intentionally much more conservative than the
declutter-and-reflow tools.
"""
from __future__ import annotations

import json
import math
import re
import sys
import time
from collections import Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import auto_router as ar
import pcbnew

ROOT = Path(__file__).resolve().parents[1]
PCB_PATH = ROOT / "Code-SDR-V2.kicad_pcb"

TARGET_TYPES = ("clearance", "shorting_items", "tracks_crossing")
SKIP_NETS = {"GND"}  # zone-filled, not a discrete track/via we can nudge


def mm(v):
    return pcbnew.ToMM(v)


def to_iu(v):
    return pcbnew.FromMM(v)


def load_offenders(drc_report_path):
    d = json.loads(Path(drc_report_path).read_text())
    uuid_count = Counter()
    for x in d.get("violations", []):
        if x.get("type") not in TARGET_TYPES:
            continue
        for it in x.get("items", []):
            u = it.get("uuid")
            if u:
                uuid_count[u] += 1
    return uuid_count


def point_clear(obstacles, x, y, radius, net_name, extra_pad_mm=1.0):
    """True if a circle of `radius` at (x,y) doesn't overlap any other-net
    pad or track/via, using the same shapes the router's search uses."""
    pad_circles = obstacles.nearby_pad_circles(x, y, x, y, extra_pad_mm, net_name)
    for (cx, cy, r, layer_filter, obs_net, halfw, halfh, angle_deg) in pad_circles:
        # conservative circular check even for oriented pads here (fine --
        # small local nudges don't need the tighter oriented-rect precision)
        if math.hypot(x - cx, y - cy) < radius + math.hypot(halfw, halfh):
            return False
    track_circles = obstacles.nearby_track_circles(x, y, x, y, extra_pad_mm, net_name)
    for (sx, sy, ex, ey, r, layer, is_via, obs_net) in track_circles:
        if ar.point_seg_dist(x, y, sx, sy, ex, ey) < radius + r:
            return False
    return True


def try_nudge_via(board, obstacles, via, search_radius_mm=0.5, step_mm=0.05):
    net_name = via.GetNetname()
    pos = via.GetPosition()
    x0, y0 = mm(pos.x), mm(pos.y)
    via_r = mm(via.GetWidth()) / 2.0
    clearance_target = 0.15  # comfortably above the 0.1mm true minimum

    candidates = []
    r = step_mm
    while r <= search_radius_mm:
        n_angles = max(8, int(r / step_mm) * 4)
        for k in range(n_angles):
            ang = 2 * math.pi * k / n_angles
            nx, ny = x0 + r * math.cos(ang), y0 + r * math.sin(ang)
            candidates.append((r, nx, ny))
        r += step_mm

    for (r, nx, ny) in candidates:
        if point_clear(obstacles, nx, ny, via_r + clearance_target, net_name):
            return nx, ny
    return None


def restitch_via_move(board, via, old_x, old_y, new_x, new_y):
    """Move the via and extend/shrink whichever track endpoints sat exactly
    at its old position so the net stays continuously connected."""
    net_name = via.GetNetname()
    tol_iu = to_iu(0.01)
    old_pos = pcbnew.VECTOR2I(to_iu(old_x), to_iu(old_y))
    new_pos = pcbnew.VECTOR2I(to_iu(new_x), to_iu(new_y))
    moved_segments = 0
    for t in board.GetTracks():
        if isinstance(t, pcbnew.PCB_VIA) or t.GetNetname() != net_name:
            continue
        s, e = t.GetStart(), t.GetEnd()
        if abs(s.x - old_pos.x) <= tol_iu and abs(s.y - old_pos.y) <= tol_iu:
            t.SetStart(new_pos)
            moved_segments += 1
        if abs(e.x - old_pos.x) <= tol_iu and abs(e.y - old_pos.y) <= tol_iu:
            t.SetEnd(new_pos)
            moved_segments += 1
    via.SetPosition(new_pos)
    return moved_segments


def try_reroute_segment(board, obstacles, track, net):
    """Remove just this one track segment and search a replacement path
    between its two endpoints at a stricter clearance, leaving the rest of
    the net's copper in place as context (not as an obstacle -- excluded by
    same-net filtering, same as everywhere else in this router)."""
    net_name = track.GetNetname()
    s, e = track.GetStart(), track.GetEnd()
    src = (mm(s.x), mm(s.y))
    dst = (mm(e.x), mm(e.y))
    layer = track.GetLayer()
    width_mm = mm(track.GetWidth())
    allow_vias = not ar.is_rf_net(net_name)

    board.Remove(track)

    attempts = [
        dict(extra_pad_mm=8.0, track_width_mm=width_mm, clearance_mm=0.2, pitch_mm=0.1, margin_cap_mm=8.0),
        dict(extra_pad_mm=15.0, track_width_mm=width_mm, clearance_mm=0.18, pitch_mm=0.1, margin_cap_mm=15.0),
        dict(extra_pad_mm=25.0, track_width_mm=width_mm, clearance_mm=0.16, pitch_mm=0.12, margin_cap_mm=25.0,
             max_cells=800000, max_pops=600000),
    ]
    path = None
    for kw in attempts:
        path = ar.build_grid_and_search(obstacles, src, dst, allow_vias, net_name,
                                         src_layers={layer}, dst_layers={layer}, **kw)
        if path is not None:
            break
    if path is None:
        # restore the original segment untouched -- don't leave a gap
        seg = pcbnew.PCB_TRACK(board)
        seg.SetStart(s)
        seg.SetEnd(e)
        seg.SetWidth(track.GetWidth())
        seg.SetLayer(layer)
        seg.SetNet(net)
        board.Add(seg)
        return False

    path = ar.simplify_path(path)
    ar.add_track_path(board, path, net, width_mm)
    return True


def main():
    drc_report = ROOT / "build" / (sys.argv[1] if len(sys.argv) > 1 else "drc-report26.json")
    max_fixes = int(sys.argv[2]) if len(sys.argv) > 2 else 60

    uuid_count = load_offenders(drc_report)
    print(f"Loaded {len(uuid_count)} distinct offending objects from {drc_report}")

    board = pcbnew.LoadBoard(str(PCB_PATH))
    uuid_to_track = {}
    for t in board.GetTracks():
        uuid_to_track[t.m_Uuid.AsString()] = t

    ranked = [u for u, _ in uuid_count.most_common() if u in uuid_to_track]
    print(f"{len(ranked)} of those are track/via objects on the current board")

    t_start = time.time()
    n_via_fixed = 0
    n_seg_fixed = 0
    n_skipped = 0

    obstacles = ar.Obstacles(board)

    for idx, u in enumerate(ranked[:max_fixes]):
        t = uuid_to_track.get(u)
        if t is None:
            continue
        net_name = t.GetNetname()
        if net_name in SKIP_NETS:
            n_skipped += 1
            continue
        try:
            if isinstance(t, pcbnew.PCB_VIA):
                pos = t.GetPosition()
                x0, y0 = mm(pos.x), mm(pos.y)
                result = try_nudge_via(board, obstacles, t)
                if result is not None:
                    nx, ny = result
                    moved = restitch_via_move(board, t, x0, y0, nx, ny)
                    print(f"[{idx}] via {net_name} nudged {math.hypot(nx-x0, ny-y0)*1000:.0f}um, "
                          f"{moved} segment endpoint(s) restitched")
                    n_via_fixed += 1
                    obstacles = ar.Obstacles(board)  # refresh after a real change
                else:
                    print(f"[{idx}] via {net_name}: no clear nudge position found within search radius")
            else:
                net = board.FindNet(net_name)
                if net is None:
                    continue
                seg_len_mm = mm(t.GetLength())
                if seg_len_mm > 15.0:
                    # this "segment" almost certainly spans most of the net
                    # (a direct-fallback straight line, or a long stretch
                    # that was never broken into shorter waypoints) -- a
                    # small local search can't route around a problem this
                    # size. These need the declutter-and-reflow/whole-net
                    # tools instead, not a surgical nudge.
                    print(f"[{idx}] track {net_name}: segment is {seg_len_mm:.1f}mm, "
                          f"too long for a local fix -- skipping (needs net-level reroute)")
                    n_skipped += 1
                    continue
                ok = try_reroute_segment(board, obstacles, t, net)
                if ok:
                    print(f"[{idx}] track {net_name}: locally rerouted")
                    n_seg_fixed += 1
                    obstacles = ar.Obstacles(board)
                else:
                    print(f"[{idx}] track {net_name}: no clear local replacement found, restored original")
        except Exception as e:
            print(f"[{idx}] ERROR on {net_name}: {e!r} -- leaving untouched")
            continue

        if idx > 0 and idx % 20 == 0:
            board.Save(str(PCB_PATH))
            print(f"[checkpoint] saved after {idx} objects")

    board.Save(str(PCB_PATH))
    print(f"\nDone. vias nudged: {n_via_fixed}  segments rerouted: {n_seg_fixed}  skipped: {n_skipped}")
    print(f"Elapsed: {time.time()-t_start:.0f}s")


if __name__ == "__main__":
    main()

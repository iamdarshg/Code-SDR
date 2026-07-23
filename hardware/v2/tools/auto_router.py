#!/usr/bin/env python3
"""Grid-based maze autorouter for Code-SDR V2.

Routes all signal nets that are not covered by a copper pour/zone.
Uses local-bounding-box Dijkstra search on a fine grid, per connection,
directly against the live pcbnew board (so later nets see earlier tracks
as obstacles automatically).

RF-critical nets are restricted to F.Cu only, no vias.
Everything else may use F.Cu / B.Cu / In2.Cu with vias between them.
In1.Cu (solid GND plane) is never used for routing.
"""

from __future__ import annotations

import heapq
import math
import sys
import time
from pathlib import Path

import pcbnew

ROOT = Path(__file__).resolve().parents[1]
PCB_PATH = ROOT / "Code-SDR-V2.kicad_pcb"

GRID_PITCH_MM = 0.25
TRACK_WIDTH_MM = 0.15
VIA_DIAMETER_MM = 0.45
VIA_DRILL_MM = 0.2
CLEARANCE_MM = 0.15

RF_NET_PATTERNS = [
    "LT_IN", "LT_INPUT", "LO_LOW", "LO_HIGH", "IF_LOW", "IF_HIGH", "LMX_LO",
]

# --- Impedance / isolation model -------------------------------------------
# Stackup (from Code-SDR-V2.kicad_pcb): F.Cu -> 0.18mm FR4 (er=4.2) -> In1.Cu (GND).
# Solved with Hammerstad-Jensen microstrip equations for this stackup:
#   50-ohm single-ended (RF chain after baluns, LO/IF "_SE"/"_RAW" feeds,
#     LNA in/out, filters): W=0.32mm
#   100-ohm differential (LMX2592 RF output is spec'd 100-ohm differential;
#     Ethernet MDI is 100-ohm per IEEE 802.3; used as the general diff-pair
#     default): W=0.20mm, gap=0.15mm
#   90-ohm differential (USB 2.0 D+/D- per USB-IF spec): W=0.25mm, gap=0.15mm
#
# Mixer differential RF/IF ports (LT5560, ADL5801, LTC5548) have odd native
# impedances set by on-chip matching (25-1100 ohm depending on part/freq per
# datasheet) that are realized with lumped L/C right at the pins, not by a
# controlled-impedance PCB transmission line -- correct PCB practice there is
# "as short and symmetric as possible", which our router already does for
# these (all such pads sit within a few mm of each other). We still route
# them as a matched pair (see DIFF_PAIR_SUFFIXES) so P/N stay length-matched.
RF_50OHM_SE_PATTERNS = [
    "_SE", "_RAW", "RF_HC_LNA", "RF_HB_LNA", "LT_INPUT_SE",
]
USB_DIFF_PATTERNS = ["USB_DP", "USB_DM"]

TRACK_WIDTH_RF_50OHM_MM = 0.32
DIFF_PAIR_100OHM_WIDTH_MM = 0.20
DIFF_PAIR_100OHM_GAP_MM = 0.15
DIFF_PAIR_90OHM_WIDTH_MM = 0.25
DIFF_PAIR_90OHM_GAP_MM = 0.15

DIFF_PAIR_SUFFIXES = [("_P", "_N"), ("_DP", "_DM")]

# Nets whose signal integrity depends on staying away from switching-digital
# return current / EMI: RF chain, LO/IF, ADC analog side.
ANALOG_NET_PATTERNS = [
    "LT_IN", "LT_INPUT", "LO_LOW", "LO_HIGH", "IF_LOW", "IF_HIGH", "LMX_LO",
    "ADF_LO2", "RF_HC_LNA", "RF_HB_LNA", "RF_LOW", "RF_HIGH", "ADC_VCM",
    "ADC_IN", "+3V3_ANA", "+5V_RF",
]
# Noisy digital nets: routed last, and kept an extra margin away from analog.
DIGITAL_NOISY_PATTERNS = [
    "RGMII", "FPGA_", "QSPI", "MDI_", "USB_D", "ADC_D", "SPI_", "SWD", "SWCLK",
    "SWDIO", "RESET", "BOOTSEL", "W25Q", "MDC", "MDIO",
]
EXTRA_ANALOG_KEEPOUT_MM = 0.25

# routing layers in preference order (avoid In1 GND plane)
ROUTE_LAYERS = [pcbnew.F_Cu, pcbnew.B_Cu, pcbnew.In2_Cu]
LAYER_NAMES = {pcbnew.F_Cu: "F.Cu", pcbnew.B_Cu: "B.Cu", pcbnew.In2_Cu: "In2.Cu"}

VIA_COST = 8  # discourage but allow layer changes


def mm(v):
    return pcbnew.ToMM(v)


def to_iu(v_mm):
    return pcbnew.FromMM(v_mm)


def pad_layer_set(pad):
    """Which of our routing layers this pad actually has copper on."""
    lset = pad.GetLayerSet()
    layers = set()
    for l in ROUTE_LAYERS:
        if lset.Contains(l):
            layers.add(l)
    if not layers:
        # through-hole pads etc: copper on all routing layers
        layers = set(ROUTE_LAYERS)
    return layers


class Obstacles:
    """Holds static obstacle circles (pads) and dynamic ones (tracks/vias)."""

    def __init__(self, board):
        self.board = board
        # per-net pad list: net_name -> list of (x_mm, y_mm, radius_mm, layer_set)
        self.pads_by_net = {}
        # (x, y, r, net_name, layers:set(F_Cu,B_Cu) or None=both, halfw_mm, halfh_mm, angle_deg)
        # r is the circumscribing radius (kept for callers that only want a
        # quick circular bound); halfw/halfh/angle describe the pad's actual
        # oriented bounding rectangle, used for the real obstacle check.
        # NOTE: a plain circle of radius max(w,h)/2 badly overestimates the
        # occlusion of an oblong pad (e.g. a 0.875x0.2mm gull-wing pad on a
        # fine-pitch QFN) -- along the SHORT axis it blocks 2x+ further than
        # the pad actually extends, which can wall a pad in against its own
        # tightly-pitched neighbors even though a real corridor exists.
        self.all_pad_circles = []
        for fp in board.GetFootprints():
            for pad in fp.Pads():
                net = pad.GetNetname()
                pos = pad.GetPosition()
                sz = pad.GetSize()
                halfw, halfh = mm(sz.x) / 2.0, mm(sz.y) / 2.0
                r = math.hypot(halfw, halfh)
                angle_deg = pad.GetOrientation().AsDegrees()
                layer_set = self._pad_layers(pad)
                circ = (mm(pos.x), mm(pos.y), r, net, layer_set, halfw, halfh, angle_deg)
                self.all_pad_circles.append(circ)
                self.pads_by_net.setdefault(net, []).append(circ)

    def _pad_layers(self, pad):
        return pad_layer_set(pad)

    def nearby_pad_circles(self, x0, y0, x1, y1, pad_mm, exclude_net):
        lo_x, hi_x = min(x0, x1) - pad_mm, max(x0, x1) + pad_mm
        lo_y, hi_y = min(y0, y1) - pad_mm, max(y0, y1) + pad_mm
        out = []
        for (x, y, r, net, layers, halfw, halfh, angle_deg) in self.all_pad_circles:
            if net == exclude_net:
                continue
            if lo_x <= x <= hi_x and lo_y <= y <= hi_y:
                out.append((x, y, r, layers, net, halfw, halfh, angle_deg))
        return out

    def nearby_track_circles(self, x0, y0, x1, y1, pad_mm, exclude_net):
        lo_x, hi_x = min(x0, x1) - pad_mm, max(x0, x1) + pad_mm
        lo_y, hi_y = min(y0, y1) - pad_mm, max(y0, y1) + pad_mm
        out = []  # (x1,y1,x2,y2,r,layer,is_via,net)
        for t in self.board.GetTracks():
            net = t.GetNetname()
            if net == exclude_net:
                continue
            s = t.GetStart()
            e = t.GetEnd()
            sx, sy, ex, ey = mm(s.x), mm(s.y), mm(e.x), mm(e.y)
            if max(sx, ex) < lo_x or min(sx, ex) > hi_x:
                continue
            if max(sy, ey) < lo_y or min(sy, ey) > hi_y:
                continue
            if t.GetClass() == "PCB_VIA":
                r = mm(t.GetWidth()) / 2.0
                out.append((sx, sy, sx, sy, r, None, True, net))
            else:
                r = mm(t.GetWidth()) / 2.0
                out.append((sx, sy, ex, ey, r, t.GetLayer(), False, net))
        return out


def point_seg_dist(px, py, x1, y1, x2, y2):
    dx, dy = x2 - x1, y2 - y1
    if dx == 0 and dy == 0:
        return math.hypot(px - x1, py - y1)
    t = ((px - x1) * dx + (py - y1) * dy) / (dx * dx + dy * dy)
    t = max(0.0, min(1.0, t))
    cx, cy = x1 + t * dx, y1 + t * dy
    return math.hypot(px - cx, py - cy)


def build_grid_and_search(obstacles, src, dst, allow_vias, net_name, extra_pad_mm=4.0,
                           track_width_mm=None, clearance_mm=None, pitch_mm=None,
                           src_layers=None, dst_layers=None, margin_cap_mm=30.0,
                           max_pops=300000, max_cells=400000):
    """src/dst: (x_mm, y_mm). Returns list of (x,y,layer) waypoints or None.

    src_layers/dst_layers restrict which copper layer(s) the path is allowed
    to actually terminate on at the source/destination, so the route only
    "arrives" where the real pad copper is (and takes a via to get there if
    the pad is on a different layer than where the search naturally lands).

    margin_cap_mm bounds how far the search box extends past the src/dst
    bounding box. NOTE: this used to be hardcoded at 30.0mm regardless of
    extra_pad_mm, which silently defeated every "search wider" fallback
    attempt -- callers that actually need a bigger detour (long nets on a
    dense board) must raise this explicitly.
    """
    x0, y0 = src
    x1, y1 = dst
    margin = max(extra_pad_mm, 0.15 * math.hypot(x1 - x0, y1 - y0))
    margin = min(margin, margin_cap_mm)
    lo_x, hi_x = min(x0, x1) - margin, max(x0, x1) + margin
    lo_y, hi_y = min(y0, y1) - margin, max(y0, y1) + margin

    pitch = pitch_mm if pitch_mm else GRID_PITCH_MM
    track_width_mm = track_width_mm if track_width_mm else TRACK_WIDTH_MM
    clearance_mm = clearance_mm if clearance_mm else CLEARANCE_MM
    nx = int((hi_x - lo_x) / pitch) + 1
    ny = int((hi_y - lo_y) / pitch) + 1
    # safety cap on grid size; coarsen if huge
    while nx * ny > max_cells:
        pitch *= 1.4
        nx = int((hi_x - lo_x) / pitch) + 1
        ny = int((hi_y - lo_y) / pitch) + 1

    layers = [pcbnew.F_Cu] if not allow_vias else list(ROUTE_LAYERS)

    def gx(x):
        return int(round((x - lo_x) / pitch))

    def gy(y):
        return int(round((y - lo_y) / pitch))

    def wx(i):
        return lo_x + i * pitch

    def wy(j):
        return lo_y + j * pitch

    half_track = track_width_mm / 2.0
    keepout = half_track + clearance_mm

    blocked = {l: bytearray(nx * ny) for l in layers}

    pad_circles = obstacles.nearby_pad_circles(lo_x, lo_y, hi_x, hi_y, margin, net_name)
    track_circles = obstacles.nearby_track_circles(lo_x, lo_y, hi_x, hi_y, margin, net_name)
    routing_is_noisy_digital = is_noisy_digital_net(net_name)

    def mark_circle(cx, cy, r, layer_filter, extra=0.0):
        rad = r + keepout + extra
        gi0, gi1 = max(0, gx(cx - rad)), min(nx - 1, gx(cx + rad))
        gj0, gj1 = max(0, gy(cy - rad)), min(ny - 1, gy(cy + rad))
        for l in layers:
            if layer_filter is not None and l not in layer_filter:
                continue
            arr = blocked[l]
            for j in range(gj0, gj1 + 1):
                yy = wy(j)
                row = j * nx
                for i in range(gi0, gi1 + 1):
                    xx = wx(i)
                    if (xx - cx) ** 2 + (yy - cy) ** 2 <= rad * rad:
                        arr[row + i] = 1

    def mark_oriented_rect(cx, cy, halfw, halfh, angle_deg, layer_filter, extra=0.0):
        """Block cells inside the pad's true oriented rectangle (expanded by
        keepout+extra), instead of a circumscribing circle. A circle sized
        off the pad's longer dimension badly overblocks along the shorter
        axis for oblong pads (gull-wing/QFN pins), which can wall a pad in
        against its own tightly-pitched neighbors even when a real corridor
        exists between them."""
        ehw, ehh = halfw + keepout + extra, halfh + keepout + extra
        rad = math.hypot(ehw, ehh)  # bounding circle for the cell-range scan only
        gi0, gi1 = max(0, gx(cx - rad)), min(nx - 1, gx(cx + rad))
        gj0, gj1 = max(0, gy(cy - rad)), min(ny - 1, gy(cy + rad))
        if angle_deg % 90.0 < 1e-6:
            # axis-aligned (0/90/180/270, the overwhelming common case) --
            # skip the trig, swap half-extents for the 90/270 cases
            swapped = (round(angle_deg / 90.0) % 2) == 1
            ehw2, ehh2 = (ehh, ehw) if swapped else (ehw, ehh)
            for l in layers:
                if layer_filter is not None and l not in layer_filter:
                    continue
                arr = blocked[l]
                for j in range(gj0, gj1 + 1):
                    yy = wy(j)
                    dy = abs(yy - cy)
                    if dy > ehh2:
                        continue
                    row = j * nx
                    for i in range(gi0, gi1 + 1):
                        xx = wx(i)
                        if abs(xx - cx) <= ehw2:
                            arr[row + i] = 1
            return
        rad_theta = math.radians(-angle_deg)
        cos_t, sin_t = math.cos(rad_theta), math.sin(rad_theta)
        for l in layers:
            if layer_filter is not None and l not in layer_filter:
                continue
            arr = blocked[l]
            for j in range(gj0, gj1 + 1):
                yy = wy(j)
                row = j * nx
                for i in range(gi0, gi1 + 1):
                    xx = wx(i)
                    dx, dy = xx - cx, yy - cy
                    lx = dx * cos_t - dy * sin_t
                    ly = dx * sin_t + dy * cos_t
                    if abs(lx) <= ehw and abs(ly) <= ehh:
                        arr[row + i] = 1

    def mark_segment(sx, sy, ex, ey, r, layer, is_via, extra=0.0):
        rad = r + keepout + extra
        if is_via:
            mark_circle(sx, sy, rad - keepout, None)
            return
        lo_xx, hi_xx = min(sx, ex) - rad, max(sx, ex) + rad
        lo_yy, hi_yy = min(sy, ey) - rad, max(sy, ey) + rad
        gi0, gi1 = max(0, gx(lo_xx)), min(nx - 1, gx(hi_xx))
        gj0, gj1 = max(0, gy(lo_yy)), min(ny - 1, gy(hi_yy))
        if layer not in blocked:
            return
        arr = blocked[layer]
        for j in range(gj0, gj1 + 1):
            yy = wy(j)
            row = j * nx
            for i in range(gi0, gi1 + 1):
                xx = wx(i)
                if point_seg_dist(xx, yy, sx, sy, ex, ey) <= rad:
                    arr[row + i] = 1

    for (cx, cy, r, layer_filter, obs_net, halfw, halfh, angle_deg) in pad_circles:
        extra = EXTRA_ANALOG_KEEPOUT_MM if (routing_is_noisy_digital and is_analog_net(obs_net)) else 0.0
        mark_oriented_rect(cx, cy, halfw, halfh, angle_deg, layer_filter, extra=extra)
    for (sx, sy, ex, ey, r, layer, is_via, obs_net) in track_circles:
        extra = EXTRA_ANALOG_KEEPOUT_MM if (routing_is_noisy_digital and is_analog_net(obs_net)) else 0.0
        mark_segment(sx, sy, ex, ey, r, layer, is_via, extra=extra)

    si, sj = gx(x0), gy(y0)
    di, dj = gx(x1), gy(y1)
    si = min(max(si, 0), nx - 1)
    sj = min(max(sj, 0), ny - 1)
    di = min(max(di, 0), nx - 1)
    dj = min(max(dj, 0), ny - 1)

    # Ensure start/end cells are only reachable on the layer(s) the real pad
    # copper actually exists on. Other layers stay/become blocked at that
    # exact cell so the search is forced to drop a via there rather than
    # silently "arriving" on a layer with no real copper (which would look
    # geometrically connected but be electrically isolated).
    src_allowed = set(src_layers) if src_layers else {layers[0]}
    dst_allowed = set(dst_layers) if dst_layers else {layers[0]}
    src_idx = sj * nx + si
    dst_idx = dj * nx + di
    for l in layers:
        blocked[l][src_idx] = 0 if l in src_allowed else 1
        blocked[l][dst_idx] = 0 if l in dst_allowed else 1

    start_l = 0
    for li, l in enumerate(layers):
        if l in src_allowed:
            start_l = li
            break

    # A* over (layer_idx, i, j) with Euclidean-distance heuristic
    def heuristic(i, j):
        return math.hypot(i - di, j - dj)

    nlayers = len(layers)
    dist = {}  # (l, idx) -> best cost
    prev = {}  # (l, idx) -> (l, i, j)
    start_key = (start_l, sj * nx + si)
    dist[start_key] = 0
    pq = [(heuristic(si, sj), 0, start_l, si, sj)]
    neighbors8 = [(-1, 0, 1.0), (1, 0, 1.0), (0, -1, 1.0), (0, 1, 1.0),
                  (-1, -1, 1.414), (1, -1, 1.414), (-1, 1, 1.414), (1, 1, 1.414)]

    found_layer = None
    pops = 0
    while pq:
        f, d, l, i, j = heapq.heappop(pq)
        idx = j * nx + i
        if d > dist.get((l, idx), math.inf):
            continue
        if i == di and j == dj:
            found_layer = l
            break
        pops += 1
        if pops > max_pops:
            break
        for (dx, dy, cost) in neighbors8:
            ni, nj = i + dx, j + dy
            if ni < 0 or ni >= nx or nj < 0 or nj >= ny:
                continue
            nidx = nj * nx + ni
            if blocked[layers[l]][nidx]:
                continue
            nd = d + cost
            key = (l, nidx)
            if nd < dist.get(key, math.inf):
                dist[key] = nd
                prev[key] = (l, i, j)
                heapq.heappush(pq, (nd + heuristic(ni, nj), nd, l, ni, nj))
        if allow_vias:
            for ol in range(nlayers):
                if ol == l:
                    continue
                if blocked[layers[ol]][idx]:
                    continue
                nd = d + VIA_COST
                key = (ol, idx)
                if nd < dist.get(key, math.inf):
                    dist[key] = nd
                    prev[key] = (l, i, j)
                    heapq.heappush(pq, (nd + heuristic(i, j), nd, ol, i, j))

    if found_layer is None:
        return None

    # reconstruct path
    path = []
    cur = (found_layer, di, dj)
    while cur is not None:
        l, i, j = cur
        path.append((wx(i), wy(j), layers[l]))
        idx = j * nx + i
        cur = prev.get((l, idx))
    path.reverse()
    # Snap the endpoints to the exact requested src/dst instead of the
    # nearest grid point -- every waypoint above is grid-quantized, so the
    # path's true start/end can be off by up to half a grid pitch. Routing
    # to/from a pad usually hides this (the gap lands inside the pad's own
    # copper), but routing between two bare mid-net waypoints (no pad to
    # absorb it) turns that gap into a real dangling, unconnected stub.
    if path:
        path[0] = (x0, y0, path[0][2])
        path[-1] = (x1, y1, path[-1][2])
    return path


def simplify_path(path):
    """Collapse colinear same-layer points and merge via points."""
    if not path:
        return path
    out = [path[0]]
    for p in path[1:]:
        if len(out) >= 2:
            (x0, y0, l0) = out[-2]
            (x1, y1, l1) = out[-1]
            (x2, y2, l2) = p
            if l1 == l0 == l2:
                # colinear check
                cross = (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0)
                if abs(cross) < 1e-6:
                    out[-1] = p
                    continue
        out.append(p)
    return out


def add_track_path(board, path, net, width_mm):
    """path: list of (x_mm, y_mm, layer). Adds segments + vias to board."""
    if len(path) < 2:
        return 0, 0
    width_iu = to_iu(width_mm)
    n_tracks = 0
    n_vias = 0
    for k in range(len(path) - 1):
        x0, y0, l0 = path[k]
        x1, y1, l1 = path[k + 1]
        if l0 != l1:
            via = pcbnew.PCB_VIA(board)
            via.SetPosition(pcbnew.VECTOR2I(to_iu(x0), to_iu(y0)))
            via.SetWidth(to_iu(VIA_DIAMETER_MM))
            via.SetDrill(to_iu(VIA_DRILL_MM))
            via.SetNet(net)
            board.Add(via)
            n_vias += 1
            continue
        if x0 == x1 and y0 == y1:
            continue
        seg = pcbnew.PCB_TRACK(board)
        seg.SetStart(pcbnew.VECTOR2I(to_iu(x0), to_iu(y0)))
        seg.SetEnd(pcbnew.VECTOR2I(to_iu(x1), to_iu(y1)))
        seg.SetWidth(width_iu)
        seg.SetLayer(l0)
        seg.SetNet(net)
        board.Add(seg)
        n_tracks += 1
    return n_tracks, n_vias


def route_edge_with_fallback(obstacles, src, dst, allow_vias, net_name, src_layers=None, dst_layers=None,
                              preferred_width_mm=None):
    """Try progressively looser/larger search settings until a path is found.
    The first attempts search at the impedance-target width (preferred_width_mm)
    so the placed track isn't wider than the corridor that was actually
    verified clear; only the last-ditch attempts narrow down for connectivity."""
    pw = preferred_width_mm if preferred_width_mm else TRACK_WIDTH_MM
    pw_clear = max(CLEARANCE_MM, pw * 0.6)
    # Grid-quantization safety: a path is only verified clear at sampled
    # grid points, not continuously, so a search clearance too close to the
    # board's true 0.1mm minimum reliably produces marginal DRC violations
    # once the grid-approximated path is drawn as a continuous trace (this
    # was measured directly: ~223 violations traced to vias/tracks placed
    # with exactly 0.1-0.12mm search clearance). Keep clearance_mm - pitch_mm
    # comfortably above 0.1mm in every attempt, not just the first ones.
    attempts = [
        dict(extra_pad_mm=4.0, track_width_mm=pw, clearance_mm=pw_clear, pitch_mm=GRID_PITCH_MM, margin_cap_mm=30.0),
        dict(extra_pad_mm=10.0, track_width_mm=pw, clearance_mm=pw_clear, pitch_mm=GRID_PITCH_MM, margin_cap_mm=30.0),
        dict(extra_pad_mm=15.0, track_width_mm=pw, clearance_mm=0.2, pitch_mm=0.15, margin_cap_mm=30.0),
        dict(extra_pad_mm=10.0, track_width_mm=0.12, clearance_mm=0.2, pitch_mm=0.15, margin_cap_mm=30.0),
        dict(extra_pad_mm=25.0, track_width_mm=0.12, clearance_mm=0.18, pitch_mm=0.12, margin_cap_mm=60.0),
        dict(extra_pad_mm=40.0, track_width_mm=0.1, clearance_mm=0.16, pitch_mm=0.1, margin_cap_mm=90.0,
             max_cells=1200000, max_pops=900000),
        # last-resort wide search before giving up to a straight-line direct
        # fallback -- this used to be capped at 30mm margin regardless of
        # extra_pad_mm, which meant long nets on a dense board almost always
        # fell straight through to direct_fallback_path instead of finding a
        # real detour
        dict(extra_pad_mm=160.0, track_width_mm=0.1, clearance_mm=0.14, pitch_mm=0.18, margin_cap_mm=160.0,
             max_cells=2000000, max_pops=1500000),
    ]
    for kw in attempts:
        path = build_grid_and_search(obstacles, src, dst, allow_vias, net_name,
                                      src_layers=src_layers, dst_layers=dst_layers, **kw)
        if path is not None:
            return path, kw["track_width_mm"]
    return None, None


def direct_fallback_path(src, dst, allow_vias, src_layers=None, dst_layers=None):
    """Absolute last resort: straight line (with a via if the two pads don't
    share a layer) ignoring obstacles, so the ratsnest closes even in
    ultra-congested regions. Marked for manual cleanup in the failure report."""
    x0, y0 = src
    x1, y1 = dst
    src_allowed = set(src_layers) if src_layers else {pcbnew.F_Cu}
    dst_allowed = set(dst_layers) if dst_layers else {pcbnew.F_Cu}
    common = src_allowed & dst_allowed
    if common:
        layer = next(iter(common))
        return [(x0, y0, layer), (x1, y1, layer)]
    src_layer = next(iter(src_allowed))
    dst_layer = next(iter(dst_allowed))
    # straight run on src_layer, via at destination point, then land on dst_layer
    return [(x0, y0, src_layer), (x1, y1, src_layer), (x1, y1, dst_layer)]


def mirror_path_offset(path, offset_mm, sign):
    """Offset every point of `path` perpendicular to the local trace direction
    by offset_mm * sign, for coupled differential-pair routing. Keeps layer
    assignment identical to the source path."""
    n = len(path)
    if n < 2:
        return list(path)
    out = []
    for k in range(n):
        x, y, l = path[k]
        if k == 0:
            dx, dy = path[1][0] - x, path[1][1] - y
        elif k == n - 1:
            dx, dy = x - path[k - 1][0], y - path[k - 1][1]
        else:
            dx = path[k + 1][0] - path[k - 1][0]
            dy = path[k + 1][1] - path[k - 1][1]
        length = math.hypot(dx, dy)
        if length < 1e-9:
            out.append((x, y, l))
            continue
        # perpendicular unit vector
        px, py = -dy / length, dx / length
        out.append((x + px * offset_mm * sign, y + py * offset_mm * sign, l))
    return out


def route_diff_pair(obstacles, board, net_p, net_n, pad_p_a, pad_p_b, pad_n_a, pad_n_b, name_p, name_n,
                     allow_vias, width_mm, gap_mm):
    """Route P as a normal maze search, then mirror it to build N so the pair
    stays tightly coupled at constant spacing (matched length + impedance).
    Falls back to an independent search for N if the mirror doesn't land
    close enough to N's real pads. Adds P's tracks to the board *before*
    searching for N's fallback path, so N's search actually sees P as an
    obstacle instead of potentially routing on top of it.
    Returns (tracks_added, vias_added, forced_p, forced_n)."""
    posa, posb = pad_p_a.GetPosition(), pad_p_b.GetPosition()
    src = (mm(posa.x), mm(posa.y))
    dst = (mm(posb.x), mm(posb.y))
    src_layers = pad_layer_set(pad_p_a)
    dst_layers = pad_layer_set(pad_p_b)
    if not allow_vias:
        src_layers = {pcbnew.F_Cu} if pcbnew.F_Cu in src_layers else src_layers
        dst_layers = {pcbnew.F_Cu} if pcbnew.F_Cu in dst_layers else dst_layers

    path_p, width_used = route_edge_with_fallback(
        obstacles, src, dst, allow_vias, name_p, src_layers=src_layers, dst_layers=dst_layers,
        preferred_width_mm=width_mm)
    forced_p = False
    if path_p is None:
        path_p = direct_fallback_path(src, dst, allow_vias, src_layers=src_layers, dst_layers=dst_layers)
        width_used = width_mm
        forced_p = True
    path_p = simplify_path(path_p)

    n_posa, n_posb = pad_n_a.GetPosition(), pad_n_b.GetPosition()
    n_src = (mm(n_posa.x), mm(n_posa.y))
    n_dst = (mm(n_posb.x), mm(n_posb.y))

    offset = width_mm + gap_mm
    # decide which side N is really on relative to P's first segment
    x0, y0, _ = path_p[0]
    x1, y1, _ = path_p[min(1, len(path_p) - 1)]
    dx, dy = x1 - x0, y1 - y0
    length = math.hypot(dx, dy) or 1.0
    px, py = -dy / length, dx / length
    sign = 1.0 if ((n_src[0] - x0) * px + (n_src[1] - y0) * py) >= 0 else -1.0

    mirrored = mirror_path_offset(path_p, offset, sign)
    mstart = (mirrored[0][0], mirrored[0][1])
    mend = (mirrored[-1][0], mirrored[-1][1])
    tol = max(1.0, offset * 4)
    start_ok = math.hypot(mstart[0] - n_src[0], mstart[1] - n_src[1]) < tol
    end_ok = math.hypot(mend[0] - n_dst[0], mend[1] - n_dst[1]) < tol

    if start_ok and end_ok:
        # snap exact endpoints to true pad centers
        mirrored[0] = (n_src[0], n_src[1], mirrored[0][2])
        mirrored[-1] = (n_dst[0], n_dst[1], mirrored[-1][2])
        nt1, nv1 = add_track_path(board, path_p, net_p, width_used)
        nt2, nv2 = add_track_path(board, mirrored, net_n, width_used)
        return nt1 + nt2, nv1 + nv2, forced_p, False

    # Mirror didn't land near N's real pads: add P now (so N's search below
    # actually sees it as an obstacle), then route N independently.
    nt1, nv1 = add_track_path(board, path_p, net_p, width_used)

    n_src_layers = pad_layer_set(pad_n_a)
    n_dst_layers = pad_layer_set(pad_n_b)
    if not allow_vias:
        n_src_layers = {pcbnew.F_Cu} if pcbnew.F_Cu in n_src_layers else n_src_layers
        n_dst_layers = {pcbnew.F_Cu} if pcbnew.F_Cu in n_dst_layers else n_dst_layers
    path_n, width_used_n = route_edge_with_fallback(
        obstacles, n_src, n_dst, allow_vias, name_n, src_layers=n_src_layers, dst_layers=n_dst_layers,
        preferred_width_mm=width_mm)
    forced_n = False
    if path_n is None:
        path_n = direct_fallback_path(n_src, n_dst, allow_vias, src_layers=n_src_layers, dst_layers=n_dst_layers)
        width_used_n = width_mm
        forced_n = True
    path_n = simplify_path(path_n)
    nt2, nv2 = add_track_path(board, path_n, net_n, width_used_n)
    return nt1 + nt2, nv1 + nv2, forced_p, forced_n


def nearest_neighbor_chain(pads):
    """Return list of (pad_a, pad_b) edges connecting all pads (simple chain).
    Defensively drops any pad object whose GetPosition() fails (occasionally
    seen as a stale SWIG wrapper after heavy programmatic track add/remove)
    rather than crashing the whole routing run."""
    good = []
    for p in pads:
        try:
            _ = p.GetPosition().x
            good.append(p)
        except AttributeError:
            print(f"WARNING: dropping invalid pad reference in chain (stale SWIG object)")
    remaining = good
    if len(remaining) < 2:
        return []
    chain = [remaining.pop(0)]
    edges = []
    while remaining:
        last = chain[-1]
        lp = last.GetPosition()
        best_idx, best_d = 0, None
        for idx, p in enumerate(remaining):
            pp = p.GetPosition()
            d = (pp.x - lp.x) ** 2 + (pp.y - lp.y) ** 2
            if best_d is None or d < best_d:
                best_d, best_idx = d, idx
        nxt = remaining.pop(best_idx)
        edges.append((last, nxt))
        chain.append(nxt)
    return edges


def minimum_spanning_tree_edges(pads):
    """Return list of (pad_a, pad_b) edges forming a minimum spanning tree
    over `pads` (Prim's algorithm, O(n^2) -- fine for the pad counts here).
    Unlike nearest_neighbor_chain, this doesn't force a single serial path:
    each new pad joins whichever already-connected pad is nearest to it, so
    the result branches locally instead of zig-zagging across the whole
    net's footprint -- far less prone to self-crossing on a big, spread-out
    net like a power rail with pads scattered over the entire board."""
    good = []
    for p in pads:
        try:
            _ = p.GetPosition().x
            good.append(p)
        except AttributeError:
            print(f"WARNING: dropping invalid pad reference in MST (stale SWIG object)")
    if len(good) < 2:
        return []

    in_tree = [good[0]]
    remaining = good[1:]
    edges = []
    while remaining:
        best_i, best_j, best_d = None, None, None
        for i, a in enumerate(in_tree):
            ap = a.GetPosition()
            for j, b in enumerate(remaining):
                bp = b.GetPosition()
                d = (ap.x - bp.x) ** 2 + (ap.y - bp.y) ** 2
                if best_d is None or d < best_d:
                    best_d, best_i, best_j = d, i, j
        a = in_tree[best_i]
        b = remaining.pop(best_j)
        edges.append((a, b))
        in_tree.append(b)
    return edges


def is_rf_net(name):
    return any(p in name for p in RF_NET_PATTERNS)


def is_analog_net(name):
    return any(p in name for p in ANALOG_NET_PATTERNS)


def is_noisy_digital_net(name):
    return any(p in name for p in DIGITAL_NOISY_PATTERNS)


def diff_pair_partner_suffix(name):
    """If name ends with a recognized diff-pair suffix, return (base, my_suffix, partner_suffix)."""
    for a, b in DIFF_PAIR_SUFFIXES:
        if name.endswith(a):
            return name[: -len(a)], a, b
        if name.endswith(b):
            return name[: -len(b)], b, a
    return None


def net_electrical_class(name):
    """Return (track_width_mm, is_diff_pair, diff_gap_mm) for a net name."""
    pair = diff_pair_partner_suffix(name)
    if pair is not None:
        if any(p in name for p in USB_DIFF_PATTERNS):
            return DIFF_PAIR_90OHM_WIDTH_MM, True, DIFF_PAIR_90OHM_GAP_MM
        return DIFF_PAIR_100OHM_WIDTH_MM, True, DIFF_PAIR_100OHM_GAP_MM
    if any(p in name for p in RF_50OHM_SE_PATTERNS) or is_rf_net(name):
        return TRACK_WIDTH_RF_50OHM_MM, False, 0.0
    return TRACK_WIDTH_MM, False, 0.0


def main():
    t_start = time.time()
    print(f"Loading board: {PCB_PATH}")
    board = pcbnew.LoadBoard(str(PCB_PATH))

    fps = board.GetFootprints()
    pads_by_net = {}
    for fp in fps:
        for pad in fp.Pads():
            net = pad.GetNetname()
            if net == "":
                continue
            pads_by_net.setdefault(net, []).append(pad)

    zone_nets = {z.GetNetname() for z in board.Zones()}
    signal_nets = {n: p for n, p in pads_by_net.items() if len(p) >= 2 and n not in zone_nets}

    print(f"Signal nets to route: {len(signal_nets)}")

    # detect valid 2-pad differential pairs so we can route them coupled
    pair_partner = {}  # name -> partner_name (only set on the "P"/first-alpha member)
    handled_as_partner = set()
    for name in list(signal_nets.keys()):
        if name in handled_as_partner or name in pair_partner:
            continue
        info = diff_pair_partner_suffix(name)
        if info is None:
            continue
        base, my_suf, partner_suf = info
        partner = base + partner_suf
        if partner in signal_nets and len(signal_nets[name]) == 2 and len(signal_nets[partner]) == 2:
            # canonical order: P before N, DP before DM
            if my_suf in ("_P", "_DP"):
                pair_partner[name] = partner
            else:
                pair_partner[partner] = name
            handled_as_partner.add(name)
            handled_as_partner.add(partner)

    # route order: power rails first (highest pad-count nets get first pick
    # of clean routing corridors -- routing them last, as the old ascending
    # pad-count sort did within every tier, meant the nets needing the most
    # connections always got whatever space was left over), then analog/RF,
    # general nets next, noisy digital last; within the non-power tiers,
    # smaller nets (fewer pads) first
    def is_power_net(n):
        return (n.startswith("+") or n in ("GND", "VIN_5V")
                or n.endswith("_3V3") or n.endswith("_5V"))

    def sort_key(n):
        if is_power_net(n):
            return (0, -len(signal_nets[n]))
        return (1, is_noisy_digital_net(n), not is_analog_net(n), len(signal_nets[n]))

    # net_names holds one entry per net, but a paired "N" member is skipped
    # (it gets routed together with its "P" partner)
    net_names = sorted(
        [n for n in signal_nets.keys() if n not in (set(pair_partner.values()))],
        key=sort_key,
    )

    obstacles = Obstacles(board)

    total_tracks = 0
    total_vias = 0
    routed_nets = 0
    failed_nets = []
    forced_nets = []

    time_budget_sec = float(sys.argv[1]) if len(sys.argv) > 1 else 3000
    net_boards = {n: board.FindNet(n) for n in signal_nets.keys()}

    for idx, name in enumerate(net_names):
        if time.time() - t_start > time_budget_sec:
            print(f"TIME BUDGET REACHED after {idx} nets, stopping.")
            failed_nets.extend(net_names[idx:])
            break

        width_mm, is_pair_member, gap_mm = net_electrical_class(name)
        allow_vias = not is_rf_net(name)

        if name in pair_partner:
            # coupled differential-pair routing: P and N together
            name_n = pair_partner[name]
            pads_p = signal_nets[name]
            pads_n = signal_nets[name_n]
            net_p = net_boards[name]
            net_n = net_boards[name_n]
            if net_p is None or net_n is None:
                continue
            allow_vias_pair = not is_rf_net(name) and not is_rf_net(name_n)
            nt, nv, forced_p, forced_n = route_diff_pair(
                obstacles, board, net_p, net_n, pads_p[0], pads_p[1], pads_n[0], pads_n[1],
                name, name_n, allow_vias_pair, width_mm, gap_mm)
            total_tracks += nt
            total_vias += nv
            routed_nets += 2
            if forced_p:
                forced_nets.append(name)
            if forced_n:
                forced_nets.append(name_n)
            continue

        pads = signal_nets[name]
        net = net_boards[name]
        if net is None:
            continue

        edges = nearest_neighbor_chain(pads)

        net_ok = True
        for (pa, pb) in edges:
            posa = pa.GetPosition()
            posb = pb.GetPosition()
            src = (mm(posa.x), mm(posa.y))
            dst = (mm(posb.x), mm(posb.y))
            src_layers = pad_layer_set(pa)
            dst_layers = pad_layer_set(pb)
            if not allow_vias:
                # RF nets: force F.Cu only regardless of pad's full layer set
                src_layers = {pcbnew.F_Cu} if pcbnew.F_Cu in src_layers else src_layers
                dst_layers = {pcbnew.F_Cu} if pcbnew.F_Cu in dst_layers else dst_layers
            if src == dst:
                continue
            path, width_used = route_edge_with_fallback(
                obstacles, src, dst, allow_vias, name, src_layers=src_layers, dst_layers=dst_layers,
                preferred_width_mm=width_mm)
            if path is None:
                # absolute last resort: direct straight line, ignores obstacles
                path = direct_fallback_path(src, dst, allow_vias, src_layers=src_layers, dst_layers=dst_layers)
                width_used = width_mm
                forced_nets.append(name)
                net_ok = False
            path = simplify_path(path)
            nt, nv = add_track_path(board, path, net, width_used)
            total_tracks += nt
            total_vias += nv

        routed_nets += 1
        if not net_ok:
            failed_nets.append(name)

        if (idx + 1) % 25 == 0:
            elapsed = time.time() - t_start
            print(f"  [{idx+1}/{len(net_names)}] done={routed_nets} forced={len(forced_nets)} "
                  f"tracks={total_tracks} vias={total_vias} elapsed={elapsed:.0f}s")

    print(f"\nDone routing pass. connected={routed_nets}/{len(net_names)} forced(needs cleanup)={len(forced_nets)}")
    print(f"Tracks added: {total_tracks}  Vias added: {total_vias}")
    if forced_nets:
        print(f"Forced-direct nets ({len(forced_nets)}): {forced_nets[:50]}")

    (ROOT / "build").mkdir(exist_ok=True)
    board.Save(str(PCB_PATH))
    print(f"Saved: {PCB_PATH}")
    print(f"Total elapsed: {time.time() - t_start:.0f}s")

    with open(ROOT / "build" / "autoroute_forced_nets.txt", "w") as f:
        f.write("\n".join(forced_nets))


if __name__ == "__main__":
    main()

"""Summarize every PCB open by net and endpoint for targeted routing."""

from __future__ import annotations

from collections import defaultdict
from pathlib import Path

import pcbnew


BOARD = Path(__file__).resolve().parents[1] / "Code-SDR-V2.kicad_pcb"


def main() -> None:
    board = pcbnew.LoadBoard(str(BOARD))
    board.BuildConnectivity()
    connectivity = board.GetConnectivity()
    counts: dict[str, int] = {}
    samples: defaultdict[str, list[tuple[float, float, float, float]]] = defaultdict(list)
    total = 0
    pads_by_net: defaultdict[int, list[pcbnew.PAD]] = defaultdict(list)
    for footprint in board.GetFootprints():
        for pad in footprint.Pads():
            pads_by_net[pad.GetNetCode()].append(pad)
    def connected_pads(seed: pcbnew.PAD) -> list[pcbnew.PAD]:
        found = [seed]
        seen = {(seed.GetParentAsString(), seed.GetNumber())}
        queue = [seed]
        tracks = list(board.GetTracks())
        while queue:
            current = queue.pop()
            anchors = [current.GetPosition()]
            if isinstance(current, pcbnew.PCB_TRACK):
                anchors.extend((current.GetStart(), current.GetEnd()))
            for track in tracks:
                if track.GetNetCode() != current.GetNetCode():
                    continue
                if any(track.HitTest(anchor) for anchor in anchors):
                    pad_hits = [
                        pad for pad in pads_by_net[current.GetNetCode()]
                        if (pad.GetParentAsString(), pad.GetNumber()) not in seen
                        and any(pad.HitTest(anchor) for anchor in [track.GetStart(), track.GetEnd()])
                    ]
                    for pad in pads_by_net[current.GetNetCode()]:
                        pad_key = (pad.GetParentAsString(), pad.GetNumber())
                        if pad_key not in seen and pad in pad_hits:
                            seen.add(pad_key)
                            found.append(pad)
                            queue.append(pad)
                    # A via/track endpoint can connect through a copper zone.
                    zones = [zone for zone in board.Zones() if zone.GetNetCode() == current.GetNetCode()]
                    zone_connected = any(
                        zone.IsOnLayer(layer)
                        and zone.GetFilledPolysList(layer).Collide(anchor)
                        for anchor in anchors for zone in zones
                        for layer in (pcbnew.F_Cu, pcbnew.In1_Cu, pcbnew.In2_Cu, pcbnew.B_Cu)
                    )
                    if not zone_connected:
                        continue
                    for pad in pads_by_net[current.GetNetCode()]:
                        pad_key = (pad.GetParentAsString(), pad.GetNumber())
                        if pad_key not in seen:
                            seen.add(pad_key)
                            found.append(pad)
                            queue.append(pad)
        return found

    # KiCad's SWIG ratsnest accessor is opaque in this build. Count connected
    # pad components; the CLI open count is the sum of pads minus components.
    total = 0
    for net_code in sorted(pads_by_net):
        name = pads_by_net[net_code][0].GetNetname()
        if not name or name == "GND":
            continue
        remaining = pads_by_net[net_code].copy()
        groups = []
        while remaining:
            seed = remaining.pop(0)
            group = connected_pads(seed)
            group_keys = {(pad.GetParentAsString(), pad.GetNumber()) for pad in group}
            remaining = [
                pad for pad in remaining
                if (pad.GetParentAsString(), pad.GetNumber()) not in group_keys
            ]
            groups.append(sorted(group, key=lambda pad: (pad.GetPosition().x, pad.GetPosition().y)))
        edges_needed = len(groups) - 1
        if edges_needed <= 0:
            continue
        counts[name] = edges_needed
        total += edges_needed
        # Nearest-group chain gives useful endpoint samples without a full MST.
        ordered = sorted(groups, key=lambda g: min((p.GetPosition().x, p.GetPosition().y) for p in g))
        for first, second in zip(ordered, ordered[1:]):
            a = min(first, key=lambda p: min((pcbnew.ToMM(p.GetPosition().x)-pcbnew.ToMM(q.GetPosition().x))**2 + (pcbnew.ToMM(p.GetPosition().y)-pcbnew.ToMM(q.GetPosition().y))**2 for q in second))
            b = min(second, key=lambda q: (pcbnew.ToMM(a.GetPosition().x)-pcbnew.ToMM(q.GetPosition().x))**2 + (pcbnew.ToMM(a.GetPosition().y)-pcbnew.ToMM(q.GetPosition().y))**2)
            samples[name].append((
                pcbnew.ToMM(a.GetPosition().x), pcbnew.ToMM(a.GetPosition().y),
                pcbnew.ToMM(b.GetPosition().x), pcbnew.ToMM(b.GetPosition().y),
            ))
    print("TOTAL_RATSNEST_EDGES", total)
    for name, count in sorted(counts.items(), key=lambda item: (-item[1], item[0])):
        points = " | ".join(
            f"({x1:.2f},{y1:.2f})-({x2:.2f},{y2:.2f})"
            for x1, y1, x2, y2 in samples[name]
        )
        print(f"{count:3} {name} {points}")


if __name__ == "__main__":
    main()

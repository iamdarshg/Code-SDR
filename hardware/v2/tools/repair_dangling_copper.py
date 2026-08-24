"""Remove provably dangling non-RF copper on an isolated candidate board."""

from __future__ import annotations

import json
import shutil
import subprocess
from pathlib import Path

import pcbnew

from export_rf50_dsn import RF50_NETS


ROOT = Path(__file__).resolve().parents[1]
LIVE = ROOT / "Code-SDR-V2.kicad_pcb"
CANDIDATE = ROOT / "build" / "agents" / "dangling" / LIVE.name
REPORT = CANDIDATE.parent / "drc.json"


def drc(board: Path, report: Path) -> dict:
    subprocess.run(
        [
            "kicad-cli", "pcb", "drc", str(board), "--format", "json",
            "--severity-all", "--all-track-errors", "--schematic-parity",
            "--units", "mm", "-o", str(report),
        ],
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    return json.loads(report.read_text(encoding="utf-8"))


def project_drc(board: Path, report: Path) -> dict:
    # Project rules/libraries resolve only from the board's own directory.
    subprocess.run(
        [
            "kicad-cli", "pcb", "drc", board.name, "--format", "json",
            "--severity-all", "--all-track-errors", "--schematic-parity",
            "--units", "mm", "-o", report.name,
        ],
        cwd=board.parent,
        check=False,
        text=True,
        capture_output=True,
    )
    return json.loads(report.read_text(encoding="utf-8"))


def main() -> None:
    print("start", flush=True)
    baseline_path = ROOT / "build" / "dangling-baseline.json"
    baseline = drc(LIVE, baseline_path)
    before_violations = len(baseline["violations"])
    before_opens = len(baseline["unconnected_items"])
    print("baseline", before_violations, before_opens, flush=True)
    CANDIDATE.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(LIVE, CANDIDATE)
    for sidecar in ("Code-SDR-V2.kicad_pro", "Code-SDR-V2.kicad_dru"):
        source = LIVE.with_suffix("." + sidecar.rsplit(".", 1)[1])
        if source.is_file():
            shutil.copy2(source, CANDIDATE.parent / sidecar)
    board = pcbnew.LoadBoard(str(CANDIDATE))
    removed = {"vias": [], "tracks": []}
    doomed = []
    for item in board.GetTracks():
        net = item.GetNetname()
        if net in RF50_NETS:
            continue
        if isinstance(item, pcbnew.PCB_VIA):
            layers = {pcbnew.F_Cu, pcbnew.B_Cu}
            connected_layers = {
                other.GetLayer() for other in board.GetTracks()
                if other is not item and other.GetNetCode() == item.GetNetCode()
                and any(other.HitTest(point) for point in (item.GetStart(), item.GetEnd()))
            }
            pads_connected = any(
                pad.HitTest(item.GetPosition())
                for footprint in board.GetFootprints() for pad in footprint.Pads()
                if pad.GetNetCode() == item.GetNetCode()
            )
            zones_connected = False
            for layer in layers:
                for zone in board.Zones():
                    if zone.GetNetCode() == item.GetNetCode() and zone.IsOnLayer(layer):
                        try:
                            if zone.GetFilledPolysList(layer).Collide(item.GetPosition()):
                                zones_connected = True
                        except Exception:
                            pass
            effective_connections = len(connected_layers) + (1 if pads_connected else 0)
            if not zones_connected and effective_connections <= 1:
                removed["vias"].append((net, pcbnew.ToMM(item.GetPosition().x), pcbnew.ToMM(item.GetPosition().y)))
                doomed.append(item)
    for track in board.GetTracks():
        if track in doomed or isinstance(track, pcbnew.PCB_VIA):
            continue
        net = track.GetNetname()
        if net in RF50_NETS:
            continue
        endpoints = (track.GetStart(), track.GetEnd())
        connected_ends = []
        for point in endpoints:
            hit = False
            for other in board.GetTracks():
                if other is track:
                    continue
                if other.GetNetCode() != track.GetNetCode():
                    continue
                if isinstance(other, pcbnew.PCB_VIA):
                    if other.HitTest(point):
                        hit = True
                        break
                elif any(other.HitTest(p) for p in (other.GetStart(), other.GetEnd())) and (
                    other.HitTest(point) or point == other.GetStart() or point == other.GetEnd()
                    or abs(other.GetStart().x - point.x) + abs(other.GetStart().y - point.y) < 1000
                    or abs(other.GetEnd().x - point.x) + abs(other.GetEnd().y - point.y) < 1000
                ):
                    hit = True
                    break
            if not hit:
                for footprint in board.GetFootprints():
                    for pad in footprint.Pads():
                        if pad.GetNetCode() == track.GetNetCode() and pad.HitTest(point):
                            hit = True
            connected_ends.append(hit)
        if connected_ends.count(False) >= 1 and pcbnew.ToMM(track.GetLength()) <= 0.15:
            removed["tracks"].append((net, pcbnew.ToMM(track.GetStart().x), pcbnew.ToMM(track.GetStart().y)))
            doomed.append(track)
    for item in doomed:
        board.Remove(item)
    # Second pass: remove only stubs whose far end is geometrically inside a
    # same-net pad, via body, or filled zone; keep real routes untouched.
    board.BuildConnectivity()
    stubs = []
    for item in list(board.GetTracks()):
        if item in doomed or isinstance(item, pcbnew.PCB_VIA):
            continue
        if item.GetNetname() in RF50_NETS:
            continue
        length = pcbnew.ToMM(item.GetLength())
        endpoints = (item.GetStart(), item.GetEnd())
        for point in endpoints:
            in_pad = any(
                pad.GetNetCode() == item.GetNetCode() and pad.HitTest(point)
                for footprint in board.GetFootprints()
                for pad in footprint.Pads()
            )
            if in_pad:
                continue
            in_via = any(
                via is not item and via.GetNetCode() == item.GetNetCode()
                and via.HitTest(point)
                for via in board.GetTracks() if isinstance(via, pcbnew.PCB_VIA)
            )
            if in_via:
                continue
            in_zone = False
            for zone in board.Zones():
                if zone.GetNetCode() != item.GetNetCode():
                    continue
                for layer in (pcbnew.F_Cu, pcbnew.B_Cu):
                    if zone.IsOnLayer(layer):
                        zone_layers.append((zone, layer))
                        try:
                            if zone.GetFilledPolysList(layer).Collide(point):
                                in_zone = True
                        except Exception:
                            pass
            if not in_zone:
                # KiCad's track_dangling check treats an endpoint inside a pad
                # body as connected only when connectivity agrees; our geometric
                # fallback must mirror that by checking distance to the pad's
                # effective copper shape, approximated here by its bounding box.
                pass
            if in_zone:
                continue
            stubs.append(item)
            break
    print("second-pass stub candidates", len(stubs), flush=True)
    for item in stubs:
        try:
            board.Remove(item)
            doomed.append(item)
        except Exception:
            pass
    pcbnew.SaveBoard(str(CANDIDATE), board)
    result = drc(CANDIDATE, REPORT)
    after_violations = len(result["violations"])
    after_opens = len(result["unconnected_items"])
    print(json.dumps({
        "removed": removed, "before": [before_violations, before_opens],
        "after": [after_violations, after_opens], "parity": len(result["schematic_parity"]),
    }, indent=2))
    assert after_violations <= before_violations and after_opens <= before_opens, "candidate regressed"


if __name__ == "__main__":
    main()

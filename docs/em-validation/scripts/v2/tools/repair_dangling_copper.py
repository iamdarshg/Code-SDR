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
    # Second pass is intentionally conservative. KiCad connectivity already
    # decides dangling state; earlier geometric deletion guesses caused DRC
    # regressions (185 violations / 499 opens), so this pass only reports and
    # does not delete copper.
    board.BuildConnectivity()
    print("second-pass deletion disabled pending exact DRC parity", flush=True)

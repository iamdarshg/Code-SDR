"""Replace one simple RF50 chain with a DRC-gated low-corner polyline."""

from __future__ import annotations

import argparse
import json
import math
import shutil
import subprocess
from pathlib import Path

import pcbnew

from export_rf50_dsn import RF50_NETS
from generate_pcb import enforce_project_rules


ROOT = Path(__file__).resolve().parents[1]
BOARD = ROOT / "Code-SDR-V2.kicad_pcb"
BUILD = ROOT / "build" / "rf50-simplify"
CLI = Path(r"C:\Program Files\KiCad\9.0\bin\kicad-cli.exe")


def point_segment_distance(point, first, second):
    dx = second[0] - first[0]
    dy = second[1] - first[1]
    if dx == 0 and dy == 0:
        return math.dist(point, first)
    scale = max(0.0, min(1.0, (
        (point[0] - first[0]) * dx + (point[1] - first[1]) * dy
    ) / (dx * dx + dy * dy)))
    closest = (first[0] + scale * dx, first[1] + scale * dy)
    return math.dist(point, closest)


def rdp(points, epsilon):
    if len(points) < 3:
        return points
    distances = [
        point_segment_distance(point, points[0], points[-1])
        for point in points[1:-1]
    ]
    maximum = max(distances)
    index = distances.index(maximum) + 1
    if maximum <= epsilon:
        return [points[0], points[-1]]
    return rdp(points[:index + 1], epsilon)[:-1] + rdp(points[index:], epsilon)


def route_chains(board, net_name):
    tracks = [
        item for item in board.GetTracks()
        if item.GetNetname() == net_name and not isinstance(item, pcbnew.PCB_VIA)
    ]
    if any(item.GetLayer() != tracks[0].GetLayer() for item in tracks):
        raise RuntimeError(f"{net_name}: multiple layers")
    graph = {}
    for item in tracks:
        first = (item.GetStart().x, item.GetStart().y)
        second = (item.GetEnd().x, item.GetEnd().y)
        graph.setdefault(first, []).append(second)
        graph.setdefault(second, []).append(first)
    terminals = {point for point, neighbours in graph.items() if len(neighbours) != 2}
    if not terminals:
        raise RuntimeError(f"{net_name}: closed loop route")
    used = set()
    chains = []
    for start in terminals:
        for following in graph[start]:
            edge = frozenset((start, following))
            if edge in used:
                continue
            used.add(edge)
            chain = [start, following]
            previous, current = start, following
            while current not in terminals:
                choices = [point for point in graph[current] if point != previous]
                if len(choices) != 1:
                    raise RuntimeError(f"{net_name}: malformed route graph")
                following = choices[0]
                used.add(frozenset((current, following)))
                chain.append(following)
                previous, current = current, following
            chains.append(chain)
    return tracks, chains, tracks[0].GetLayer()


def drc(tag):
    BUILD.mkdir(parents=True, exist_ok=True)
    enforce_project_rules()
    report = BUILD / f"{tag}.json"
    subprocess.run([
        str(CLI), "pcb", "drc", str(BOARD), "--format", "json",
        "--severity-all", "--all-track-errors", "--schematic-parity",
        "--output", str(report),
    ], check=False)
    return json.loads(report.read_text(encoding="utf-8"))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("net", choices=sorted(RF50_NETS))
    parser.add_argument("--epsilon", type=float, default=0.15)
    args = parser.parse_args()
    baseline = drc(f"{args.net}-baseline")
    allowed = len(baseline.get("violations", []))
    BUILD.mkdir(parents=True, exist_ok=True)
    backup = BUILD / f"{args.net}-backup.kicad_pcb"
    shutil.copy2(BOARD, backup)
    board = pcbnew.LoadBoard(str(BOARD))
    tracks, chains_iu, layer = route_chains(board, args.net)
    chains = [[(pcbnew.ToMM(x), pcbnew.ToMM(y)) for x, y in chain] for chain in chains_iu]
    simplified_chains = [rdp(points, args.epsilon) for points in chains]
    old_segments = sum(len(points) - 1 for points in chains)
    new_segments = sum(len(points) - 1 for points in simplified_chains)
    if new_segments >= old_segments:
        print(f"UNCHANGED {args.net} segments={old_segments}")
        return
    for item in tracks:
        board.Remove(item)
    for simplified in simplified_chains:
        for first, second in zip(simplified, simplified[1:]):
            item = pcbnew.PCB_TRACK(board)
            item.SetNet(board.FindNet(args.net))
            item.SetLayer(layer)
            item.SetWidth(pcbnew.FromMM(0.23))
            item.SetStart(pcbnew.VECTOR2I(pcbnew.FromMM(first[0]), pcbnew.FromMM(first[1])))
            item.SetEnd(pcbnew.VECTOR2I(pcbnew.FromMM(second[0]), pcbnew.FromMM(second[1])))
            board.Add(item)
    pcbnew.SaveBoard(str(BOARD), board)
    report = drc(args.net)
    safe = len(report.get("violations", [])) <= allowed and not report.get("schematic_parity", [])
    print(
        f"{'ACCEPT' if safe else 'REJECT'} {args.net} "
        f"segments={old_segments}->{new_segments} epsilon={args.epsilon}",
        flush=True,
    )
    if not safe:
        shutil.copy2(backup, BOARD)
        raise RuntimeError(f"{args.net}: simplified candidate failed DRC")


if __name__ == "__main__":
    main()

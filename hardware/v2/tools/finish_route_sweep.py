from __future__ import annotations

import json
import math
import re
import shutil
import subprocess
import uuid
from collections import Counter
from pathlib import Path

PROJECT = Path('hardware/v2')
ROOT = PROJECT / 'build' / 'finish-sweep'
BOARD = PROJECT / 'Code-SDR-V2.kicad_pcb'
ACCEPTED = ROOT / 'accepted.kicad_pcb'
CURRENT_DRC = ROOT / 'drc-current.json'
CANDIDATE_DRC = ROOT / 'candidate-drc.json'
TARGET_SIGNAL_OPENS = 16

ROOT.mkdir(parents=True, exist_ok=True)

NET_RE = re.compile(r'^\s*\(net\s+(\d+)\s+"([^"]+)"\)', re.MULTILINE)
SEGMENT_RE = re.compile(r'\n\s*\(segment\s*\n.*?\n\s*\)', re.DOTALL)


def run_drc(output: Path) -> dict:
    subprocess.run(
        [
            'kicad-cli', 'pcb', 'drc', '--format', 'json', '--severity-all',
            '--all-track-errors', '--schematic-parity', '--units', 'mm',
            '-o', str(output), str(BOARD),
        ],
        check=False,
    )
    if not output.is_file():
        raise RuntimeError(f'DRC did not create {output}')
    return json.loads(output.read_text())


def entry_net(entry: dict) -> str:
    text = ' '.join(item.get('description', '') for item in entry.get('items', []))
    match = re.search(r'\[([^\]]+)\]', text)
    return match.group(1) if match else ''


def metrics(report: dict) -> dict:
    by_net = Counter(entry_net(entry) for entry in report.get('unconnected_items', []))
    return {
        'opens': len(report.get('unconnected_items', [])),
        'signal_opens': sum(
            count for net, count in by_net.items()
            if net not in {'GND', 'CHASSIS_GND'}
        ),
        'nonlibrary_violations': sum(
            1 for violation in report.get('violations', [])
            if not violation.get('type', '').startswith('lib_footprint_')
        ),
        'library_violations': sum(
            1 for violation in report.get('violations', [])
            if violation.get('type', '').startswith('lib_footprint_')
        ),
        'parity': len(report.get('schematic_parity', [])),
        'by_net': dict(by_net),
    }


def net_codes(board_text: str) -> dict[str, int]:
    result = {name: int(code) for code, name in NET_RE.findall(board_text)}
    if not result:
        raise RuntimeError('No net declarations found in routed PCB')
    return result


def insert_blocks(board_text: str, blocks: list[str]) -> str:
    index = board_text.rfind('\n)')
    if index < 0:
        raise RuntimeError('Could not find PCB closing parenthesis')
    return board_text[:index] + ''.join(blocks) + board_text[index:]


def segment_block(a, b, width: float, layer: str, net: int) -> str:
    return (
        '\n\t(segment\n'
        f'\t\t(start {a[0]:.6f} {a[1]:.6f})\n'
        f'\t\t(end {b[0]:.6f} {b[1]:.6f})\n'
        f'\t\t(width {width:.6f})\n'
        f'\t\t(layer "{layer}")\n'
        f'\t\t(net {net})\n'
        f'\t\t(uuid "{uuid.uuid4()}")\n'
        '\t)'
    )


def via_block(point, net: int) -> str:
    return (
        '\n\t(via\n'
        f'\t\t(at {point[0]:.6f} {point[1]:.6f})\n'
        '\t\t(size 0.450000)\n'
        '\t\t(drill 0.200000)\n'
        '\t\t(layers "F.Cu" "B.Cu")\n'
        f'\t\t(net {net})\n'
        f'\t\t(uuid "{uuid.uuid4()}")\n'
        '\t)'
    )


def add_path(board_text: str, net_name: str, path, width: float, layer: str, vias=False) -> str:
    codes = net_codes(board_text)
    code = codes[net_name]
    blocks = []
    if vias:
        blocks.extend((via_block(path[0], code), via_block(path[-1], code)))
    blocks.extend(
        segment_block(a, b, width, layer, code)
        for a, b in zip(path, path[1:])
        if math.dist(a, b) > 1e-6
    )
    return insert_blocks(board_text, blocks)


def parse_segment(block: str):
    start = re.search(r'\(start\s+([-\d.]+)\s+([-\d.]+)\)', block)
    end = re.search(r'\(end\s+([-\d.]+)\s+([-\d.]+)\)', block)
    net = re.search(r'\(net\s+(\d+)\)', block)
    if not start or not end or not net:
        return None
    return (
        (float(start.group(1)), float(start.group(2))),
        (float(end.group(1)), float(end.group(2))),
        int(net.group(1)),
    )


def same_edge(start, end, a, b, tolerance=0.003):
    return (
        math.dist(start, a) <= tolerance and math.dist(end, b) <= tolerance
    ) or (
        math.dist(start, b) <= tolerance and math.dist(end, a) <= tolerance
    )


def replace_clock_route(board_text: str) -> str:
    code = net_codes(board_text)['REF_100M_ADC']
    targets = [
        ((58.75, 22.325), (59.4667, 22.325)),
        ((59.4667, 22.325), (59.4667, 24.0849)),
        ((59.4667, 24.0849), (65.6755, 30.2937)),
    ]

    def keep_or_remove(match):
        block = match.group(0)
        parsed = parse_segment(block)
        if parsed is None:
            return block
        start, end, segment_net = parsed
        if segment_net == code and any(same_edge(start, end, a, b) for a, b in targets):
            return ''
        return block

    result = SEGMENT_RE.sub(keep_or_remove, board_text)
    path = [
        (58.75, 22.325),
        (59.80, 22.325),
        (59.80, 24.0849),
        (65.6755, 30.2937),
    ]
    return add_path(result, 'REF_100M_ADC', path, 0.13, 'F.Cu')


def width_for(net: str) -> float:
    if net.startswith(('RF_HC', 'RF_HD')) or net in {
        'LMX_LO_HIGH_N_RAW', 'LMX_LO_HIGH_P_RAW', 'LO_HIGH_P', 'LO_HIGH_N_TERM'
    }:
        return 0.30
    if net.startswith(('RF_', 'LO_', 'LMX_LO', 'PE4312', 'IF_')):
        return 0.25
    if net.startswith(('ADC_', 'RGMII', 'REF_100M')):
        return 0.10
    if net.startswith('+') or net.startswith('FPGA_VCC'):
        return 0.15
    return 0.10


def critical_rf(net: str) -> bool:
    return net.startswith(('RF_HC', 'RF_HD')) or net in {
        'LMX_LO_HIGH_N_RAW', 'LMX_LO_HIGH_P_RAW', 'LO_HIGH_P', 'LO_HIGH_N_TERM'
    }


def geometric_paths(a, b):
    ax, ay = a
    bx, by = b
    dx, dy = bx - ax, by - ay
    paths = [[a, b], [a, (bx, ay), b], [a, (ax, by), b]]
    if abs(dx) > 1e-6 and abs(dy) > 1e-6:
        step = min(abs(dx), abs(dy))
        paths.append([a, (ax + math.copysign(step, dx), ay + math.copysign(step, dy)), b])
    length = max(math.hypot(dx, dy), 1e-6)
    nx, ny = -dy / length, dx / length
    for offset in (1.0, -1.0):
        q1 = (ax + dx * 0.30 + nx * offset, ay + dy * 0.30 + ny * offset)
        q2 = (ax + dx * 0.70 + nx * offset, ay + dy * 0.70 + ny * offset)
        paths.append([a, q1, q2, b])
    unique, seen = [], set()
    for path in paths:
        key = tuple((round(x, 4), round(y, 4)) for x, y in path)
        if key not in seen:
            seen.add(key)
            unique.append(path)
    return unique


def candidate_variants(net, a, b):
    for path in geometric_paths(a, b):
        yield 'F.Cu', path, False
    if not critical_rf(net):
        for path in geometric_paths(a, b):
            yield 'In2.Cu', path, True


def evaluate(label: str, candidate_text: str, current_score: dict, target_net: str | None = None):
    BOARD.write_text(candidate_text)
    report = run_drc(CANDIDATE_DRC)
    score = metrics(report)
    target_better = True
    if target_net is not None:
        target_better = score['by_net'].get(target_net, 0) < current_score['by_net'].get(target_net, 0)
    better = (
        target_better
        and score['signal_opens'] <= current_score['signal_opens']
        and score['nonlibrary_violations'] <= current_score['nonlibrary_violations']
        and score['parity'] <= current_score['parity']
        and (
            score['signal_opens'] < current_score['signal_opens']
            or score['nonlibrary_violations'] < current_score['nonlibrary_violations']
            or score['parity'] < current_score['parity']
        )
    )
    print(label, 'ACCEPT' if better else 'REJECT', json.dumps(score, sort_keys=True), flush=True)
    return better, score


board_text = BOARD.read_text()
board_text = board_text.replace(
    'Inductor_SMD:L_0805_2012Metric_Pad1.15x1.40mm_HandSolder',
    'CodeSDR:Inductor_0805_HandSolder',
)
BOARD.write_text(board_text)
baseline_report = run_drc(ROOT / 'drc-baseline.json')
current = metrics(baseline_report)
print('BASELINE', json.dumps(current, sort_keys=True), flush=True)
ACCEPTED.write_text(board_text)
CURRENT_DRC.write_text(json.dumps(baseline_report, indent=2) + '\n')
accepted = [{'kind': 'baseline', 'metrics': current}]
rejected = []

clock_candidate = replace_clock_route(board_text)
ok, score = evaluate('REF_100M_ADC_clearance_detour', clock_candidate, current)
if ok:
    board_text, current = clock_candidate, score
    ACCEPTED.write_text(board_text)
    shutil.copy2(CANDIDATE_DRC, CURRENT_DRC)
    accepted.append({'kind': 'REF_100M_ADC_clearance_detour', 'metrics': current})
else:
    BOARD.write_text(board_text)
    rejected.append({'kind': 'REF_100M_ADC_clearance_detour', 'metrics': score})

entries = []
report = json.loads(CURRENT_DRC.read_text())
for entry in report.get('unconnected_items', []):
    net = entry_net(entry)
    items = entry.get('items', [])
    if net in {'', 'GND', 'CHASSIS_GND'} or len(items) != 2:
        continue
    a = (float(items[0]['pos']['x']), float(items[0]['pos']['y']))
    b = (float(items[1]['pos']['x']), float(items[1]['pos']['y']))
    distance = math.dist(a, b)
    if distance <= 13.6:
        entries.append((distance, net, a, b))
entries.sort()
print('SHORT_GAP_COUNT', len(entries), flush=True)

attempted = set()
for distance, net, a, b in entries:
    if current['signal_opens'] <= TARGET_SIGNAL_OPENS:
        print('TARGET_REACHED', current['signal_opens'], flush=True)
        break
    key = (net, round(a[0], 3), round(a[1], 3), round(b[0], 3), round(b[1], 3))
    if key in attempted or current['by_net'].get(net, 0) == 0:
        continue
    attempted.add(key)
    for index, (layer, path, vias) in enumerate(candidate_variants(net, a, b)):
        candidate = add_path(board_text, net, path, width_for(net), layer, vias)
        label = f'close_{net}_{layer}_{index}'
        ok, score = evaluate(label, candidate, current, net)
        record = {
            'kind': label,
            'net': net,
            'distance_mm': round(distance, 4),
            'layer': layer,
            'vias': vias,
            'path': path,
            'metrics': score,
        }
        if ok:
            board_text, current = candidate, score
            ACCEPTED.write_text(board_text)
            shutil.copy2(CANDIDATE_DRC, CURRENT_DRC)
            accepted.append(record)
            break
        BOARD.write_text(board_text)
        rejected.append(record)

BOARD.write_text(board_text)
final_report = run_drc(ROOT / 'drc-finish-sweep.json')
final_metrics = metrics(final_report)
shutil.copy2(BOARD, ROOT / 'Code-SDR-V2.kicad_pcb')
(ROOT / 'accepted-patches.json').write_text(json.dumps(accepted, indent=2) + '\n')
(ROOT / 'rejected-patches.json').write_text(json.dumps(rejected, indent=2) + '\n')
(ROOT / 'finish-sweep-summary.json').write_text(json.dumps({
    'before': metrics(baseline_report),
    'after': final_metrics,
    'accepted_patch_count': len(accepted) - 1,
    'rejected_patch_count': len(rejected),
}, indent=2) + '\n')
print('FINAL', json.dumps(final_metrics, sort_keys=True), flush=True)

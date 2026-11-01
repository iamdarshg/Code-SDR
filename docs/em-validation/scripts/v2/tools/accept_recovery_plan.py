"""Import an additive plan into a full-context copy and gate its promotion.

Run with KiCad Python. The baseline DRC must describe the current live board;
the receipt records both hashes so the comparison remains auditable.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess

from freerouting_contract import compare_preservation, snapshot_board
from recovery_board_io import export_board, import_plan
from run_one_pass_freerouting import _copy_inputs


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def fingerprint(v):
    return json.dumps(v, sort_keys=True)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('board', type=Path)
    ap.add_argument('plan', type=Path)
    ap.add_argument('baseline_drc', type=Path)
    ap.add_argument('run_dir', type=Path)
    args = ap.parse_args()
    before_hash = digest(args.board)
    plan = json.loads(args.plan.read_text(encoding='utf-8'))
    if before_hash != plan['source_sha256']:
        raise RuntimeError('Live board changed since planning')
    before = json.loads(args.baseline_drc.read_text(encoding='utf-8-sig'))
    args.run_dir.mkdir(parents=True, exist_ok=False)
    checkpoint, candidate, _ = _copy_inputs(args.board, args.board.with_suffix('.kicad_pro'), args.run_dir)
    snapshot = snapshot_board(checkpoint)
    import_plan(candidate, args.plan, candidate)
    report_path = args.run_dir / 'drc.json'
    subprocess.run(['kicad-cli', 'pcb', 'drc', '--all-track-errors', '--severity-all',
                    '--schematic-parity', '--format', 'json', '-o', str(report_path), str(candidate)], check=True)
    after = json.loads(report_path.read_text(encoding='utf-8-sig'))
    errors = compare_preservation(snapshot, candidate)
    prior = {fingerprint(v) for v in before['violations']}
    errors += [f'New DRC finding: {v}' for v in after['violations'] if fingerprint(v) not in prior]
    errors += [f'Parity finding: {v}' for v in after.get('schematic_parity', [])]
    before_opens, after_opens = len(before['unconnected_items']), len(after['unconnected_items'])
    if after_opens >= before_opens:
        errors.append(f'No connectivity improvement: {before_opens} -> {after_opens}')
    if digest(args.board) != before_hash:
        errors.append('Live board changed during validation')
    receipt = dict(source_sha256=before_hash, candidate_sha256=digest(candidate),
                   baseline_drc=str(args.baseline_drc), baseline_drc_sha256=digest(args.baseline_drc),
                   before_opens=before_opens, after_opens=after_opens,
                   after_violations=len(after['violations']), errors=errors, accepted=not errors)
    (args.run_dir / 'receipt.json').write_text(json.dumps(receipt, indent=2)+'\n', encoding='utf-8')
    if errors:
        raise RuntimeError(json.dumps(receipt, indent=2))
    shutil.copy2(candidate, args.board)
    export_board(args.board, args.run_dir / 'geometry.json')
    print(json.dumps(receipt, indent=2), flush=True)


if __name__ == '__main__':
    main()

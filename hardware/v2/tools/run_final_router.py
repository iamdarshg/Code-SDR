"""Bounded, additive Freerouting runner for an explicitly supplied checkpoint.

Uses the existing candidate/checkpoint and DRC acceptance pipeline. Every prior
track/via is protected, RF50 additions keep nominal 0.23 mm width, and the Java
process is bounded to 768 MiB heap and one available processor by default.
"""
from __future__ import annotations

import argparse
from collections import Counter
from dataclasses import replace
import json
import os
from pathlib import Path
import re
import subprocess

import freerouting_contract as contract
import run_one_pass_freerouting as base


def open_nets(report):
    """Use actual KiCad connectivity; the presence of track pieces is insufficient."""
    return {net for issue in report['unconnected_items'] for item in issue['items']
            for net in re.findall(r'\[([^\]]+)\]', item['description'])}


def pair_quality(before, after, reports):
    if len(reports) != 2:
        return ['Missing before/after connectivity evidence']
    old_open, new_open = map(open_nets, reports)
    errors = []
    for name, old in before.pair_measurements.items():
        new = after.pair_measurements[name]
        names = set(old.positive + old.negative)
        old_connected = old.connected and not names.intersection(old_open)
        new_connected = new.connected and not names.intersection(new_open)
        old_valid = old_connected and old.skew_mm <= old.tolerance_mm and old.positive_vias == old.negative_vias
        new_valid = new_connected and new.skew_mm <= new.tolerance_mm and new.positive_vias == new.negative_vias
        if old_valid and not new_valid:
            errors.append(f'Completed pair regressed: {name}')
        if new_connected and not new_valid:
            errors.append(f'Connected pair outside skew/via tolerance: {name}')
    return errors


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--board', required=True, type=Path)
    parser.add_argument('--project', required=True, type=Path)
    parser.add_argument('--work-root', required=True, type=Path)
    parser.add_argument('--router-jar', required=True, type=Path)
    parser.add_argument('--router-timeout', type=float, default=600)
    parser.add_argument('--heap-mb', type=int, default=768)
    parser.add_argument('--freeze-net', action='append', default=[])
    parser.add_argument('--route-nets', type=Path, help='JSON list of explicitly selected nets; ignore all other net classes')
    parser.add_argument('--algorithm', choices=['freerouting-router','freerouting-router-v19'], default='freerouting-router')
    parser.add_argument('--dry-run', action='store_true')
    args = parser.parse_args()
    if not 256 <= args.heap_mb <= 768:
        parser.error('heap must be between 256 and 768 MiB')
    if not 10 <= args.router_timeout <= 1200:
        parser.error('router timeout must be between 10 and 1200 seconds')
    for name in ('OPENBLAS_NUM_THREADS', 'OMP_NUM_THREADS', 'MKL_NUM_THREADS'):
        os.environ[name] = '1'
    import pcbnew
    from export_rf50_dsn import RF50_NETS

    original_snapshot = base.snapshot_board

    def snapshot(path):
        value = original_snapshot(path)
        board = pcbnew.LoadBoard(str(path))
        items = Counter(contract._track_signature(board, item, pcbnew) for item in board.GetTracks())
        return replace(value, protected_items=items,
                       rf_protected_items=Counter({k: v for k, v in items.items() if k[1] in RF50_NETS}),
                       editable_nets=frozenset())

    base.snapshot_board = snapshot
    frozen = set(args.freeze_net)
    selected = set(json.loads(args.route_nets.read_text())) if args.route_nets else None

    def export(candidate, dsn, editable, route_only=False):
        board = pcbnew.LoadBoard(str(candidate))
        if not pcbnew.ExportSpecctraDSN(board, str(dsn)):
            raise RuntimeError('DSN export failed')
        source = dsn.read_text(encoding='utf-8')
        # Lock existing wires before rewriting classes; class membership does
        # not grant permission to remove or move existing copper.
        source = contract._protect_wires(source, set())
        pieces, cursor = [], 0
        for start, end, scope in contract._scopes(source, 'via'):
            pieces.append(source[cursor:start])
            if contract._net_from_wiring_scope(scope):
                if '(type route)' in scope:
                    scope = scope.replace('(type route)', '(type protect)')
                elif '(type protect)' not in scope:
                    scope = scope[:-1] + ' (type protect))'
            pieces.append(scope)
            cursor = end
        source = ''.join(pieces) + source[cursor:]
        if selected is None:
            source = contract._rewrite_classes(source, set(RF50_NETS))
        else:
            # A class exclusion still leaves every ratsnest edge in the DSN,
            # which makes Freerouting search the full board before honoring the
            # class filter. Remove only unselected connectivity declarations;
            # protected existing wire/via scopes remain present and immutable.
            network_start, network_end, network = next(contract._scopes(source, 'network'))
            pieces, cursor = [], 0
            while True:
                match = re.search(r'\(net\s+("(?:[^"\\]|\\.)*"|\S+)', network[cursor:])
                if not match:
                    break
                start = cursor + match.start()
                end = contract._scope_end(network, start)
                name = match.group(1).strip('"')
                if name not in selected:
                    scope = network[start:end]
                    scope = re.sub(r'\(pins\s+[^\)]*\)', '(pins)', scope,
                                   count=1, flags=re.S)
                    pieces.append(network[cursor:start] + scope)
                    cursor = end
                else:
                    pieces.append(network[cursor:end])
                    cursor = end
            pieces.append(network[cursor:])
            source = source[:network_start] + ''.join(pieces) + source[network_end:]
            start,end,scope=next(s for s in contract._scopes(source,'class') if re.match(r'\(class\s+kicad_default(?:\s|\))',s[2]))
            first=scope.find('(',1)
            header,body=scope[:first],scope[first:]
            for name in sorted(selected,key=len,reverse=True):
                header=re.sub(rf'(?<!\S){re.escape(name)}(?!\S)','',header)
            classes=[]
            for width,names in ((230,selected & set(RF50_NETS)),(300,{n for n in selected-set(RF50_NETS) if n.startswith(('+','VIN','FPGA_VCC','PHY_AVDD')) or n in ('GND','RP_CORE','CHASSIS_GND')})):
                if names:classes.append((width,names))
            used=set().union(*(names for _,names in classes)) if classes else set()
            if selected-used:classes.append((150,selected-used))
            rewritten=[]
            for width,names in classes:
                rewritten.append(f'(class RECOVERY_{width} '+ ' '.join(sorted(names))+'\n(circuit (use_via "Via[0-3]_450:200_um") (use_layer F.Cu B.Cu))\n'+f'(rule (width {width})(clearance {180 if width==230 else 100})))')
            source=source[:start]+'\n'.join(rewritten)+'\n'+header+body+source[end:]
        source = contract._limit_route_layers(contract._mark_internal_layers_power(source))
        dsn.write_text(source, encoding='utf-8')

    base._export_transformed_dsn = export
    drc_reports = []
    original_drc = base._run_drc

    def drc(board, report):
        result = original_drc(board, report)
        drc_reports.append(result)
        return result

    base._run_drc = drc
    def quality(before, after):
        errors = pair_quality(before, after, drc_reports)
        additions = after.protected_items - before.protected_items
        for sig, count in additions.items():
            if sig[1] in frozen:
                errors.append(f'frozen net gained copper: {sig!r} x{count}')
            if sig[0] == 'segment' and sig[1] in RF50_NETS and sig[-1] != pcbnew.FromMM(0.23):
                errors.append(f'RF50 addition has non-nominal width: {sig!r}')
        if len(drc_reports) == 2:
            old = Counter(issue.get('type') for issue in drc_reports[0].get('violations', []))
            new = Counter(issue.get('type') for issue in drc_reports[1].get('violations', []))
            for kind, count in (new - old).items():
                errors.append(f'DRC violation category increased: {kind} by {count}')
        return errors

    base._pair_quality_errors = quality

    def invoke(jar, dsn, ses, run_dir, timeout):
        if base._router_invoked:
            raise RuntimeError('Router was already invoked')
        base._router_invoked = True
        graceful_seconds = max(1, int(timeout) - 60)
        graceful_limit = f'{graceful_seconds // 3600:02}:{graceful_seconds // 60 % 60:02}:{graceful_seconds % 60:02}'
        command = ['java', f'-Xmx{args.heap_mb}m', '-Xms64m', '-XX:ActiveProcessorCount=1',
                   # Java 25's automatic Unix socket binds but cannot connect on
                   # this Windows host. A nonexistent socket directory invokes
                   # PipeImpl's built-in TCP loopback fallback (JDK src.zip).
                   f'-Djdk.net.unixdomain.tmpdir={run_dir.resolve() / "tcp-loopback-fallback"}',
                   '-XX:+UseSerialGC', '-Djava.awt.headless=true', '-jar', str(jar.resolve()),
                   '-de', str(dsn.resolve()), '-do', str(ses.resolve()), '-mp', '1', '-mt', '1',
                   '--gui.enabled=false', '--router.max_threads=1',
                   f'--router.algorithm={args.algorithm}',
                   f'--router.job_timeout={graceful_limit}',
                   '--router.optimizer.enabled=false', '--router.fanout.enabled=false',
                   '--router.strict_drc=true', '--router.automatic_neckdown=false',
                   '--router.neck_width_um=0', '--api_server.enabled=false',
                   '--mcp_server.enabled=false',
                   f'--user_data_path={run_dir.resolve() / "router-data"}']
        if selected is not None:
            command += ['-inc','kicad_default']
        (run_dir / 'router-command.json').write_text(json.dumps(command, indent=2), encoding='utf-8')
        with (run_dir / 'router-stdout.log').open('w', encoding='utf-8') as stdout, \
                (run_dir / 'router-stderr.log').open('w', encoding='utf-8') as stderr:
            try:
                result = subprocess.run(command, cwd=run_dir, stdout=stdout, stderr=stderr, timeout=timeout)
            except subprocess.TimeoutExpired:
                (run_dir / 'router-result.json').write_text(json.dumps({'timed_out': True, 'invocations': 1}), encoding='utf-8')
                raise RuntimeError(f'Router exceeded {timeout} seconds; candidate not accepted')
        (run_dir / 'router-result.json').write_text(json.dumps({'returncode': result.returncode, 'invocations': 1}), encoding='utf-8')
        if result.returncode or not ses.is_file() or not ses.stat().st_size:
            raise RuntimeError(f'Router returned {result.returncode}, or SES absent; see {run_dir}')
        return 1

    base._invoke_router_once = invoke
    result = base.run_one_pass(args.board, args.project, args.work_root, args.router_jar,
                               dry_run=args.dry_run, router_timeout=args.router_timeout)
    print(json.dumps({'accepted': result.accepted, 'run_dir': str(result.run_dir),
                      'router_invocations': result.router_invocations}))
    if not args.dry_run and not result.accepted:
        raise SystemExit(2)


if __name__ == '__main__':
    main()

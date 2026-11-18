#!/usr/bin/env python3
"""Cross-check diffpair_skew.py against Code-SDR's own pair tooling (read-only).

    $KICAD_PY diffpair_crosscheck.py --repo <Code-SDR> --skew <diffpair_skew.json> \
        --route-stats <ROUTE_STATISTICS.csv written by validate_route.py> --out crosscheck.json

* validate_route.py metric: per-net sum of track lengths from its ROUTE_STATISTICS.csv (no via
  barrels), skew = |sum(P) - sum(N)|, fails if skew > tolerance OR via counts differ.
* tune_sensitive_pairs.route_length(): imported from the repo and called on the board for its
  six PAIRS (same metric, computed live).
"""
import argparse, csv, json, os, sys

import pcbnew


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--repo', required=True)
    ap.add_argument('--skew', required=True)
    ap.add_argument('--route-stats', required=True)
    ap.add_argument('--out', required=True)
    a = ap.parse_args()
    tools = os.path.join(a.repo, 'hardware', 'v2', 'tools')
    sys.path.insert(0, tools)
    import tune_sensitive_pairs as tsp  # noqa: E402  (module only defines functions/constants)

    rs = {}
    for row in csv.DictReader(open(a.route_stats)):
        rs[row['net']] = dict(length_mm=float(row['length_mm']), vias=int(row['vias']), layers=row['layers'])
    sk = json.load(open(a.skew))
    board = pcbnew.LoadBoard(os.path.join(a.repo, 'hardware', 'v2', 'Code-SDR-V2.kicad_pcb'))

    rows = []
    max_dev = 0.0
    for r in sk['pairs']:
        if not r.get('limits'):
            continue
        rp = sum(rs.get(n, {}).get('length_mm', 0.0) for n in r['p'])
        rn = sum(rs.get(n, {}).get('length_mm', 0.0) for n in r['n'])
        vp = sum(rs.get(n, {}).get('vias', 0) for n in r['p'])
        vn = sum(rs.get(n, {}).get('vias', 0) for n in r['n'])
        repo_skew = abs(rp - rn)
        dev = abs(repo_skew - r['skew_track_only_mm'])
        max_dev = max(max_dev, dev)
        rows.append(dict(
            pair=r['name'], p=r['p'], n=r['n'],
            repo_track_p_mm=round(rp, 3), repo_track_n_mm=round(rn, 3), repo_vias=f'{vp}/{vn}',
            repo_skew_mm=round(repo_skew, 3),
            ours_track_only_skew_mm=r['skew_track_only_mm'], ours_with_barrels_skew_mm=r['skew_mm'],
            ours_skew_ps=r['skew_ps'],
            ours_pad_to_pad=r.get('pad_to_pad_skew'),
            deviation_track_only_mm=round(dev, 4),
            limits={l['source'].split()[0].split('/')[-1]: l['max_skew_mm'] for l in r['limits']},
            repo_verdict={l['source'].split()[0].split('/')[-1]:
                          ('PASS' if repo_skew <= l['max_skew_mm'] and (vp == vn or not l.get('require_equal_vias')) else 'FAIL')
                          for l in r['limits']},
        ))
    tsp_rows = []
    for p, n, lim in tsp.PAIRS:
        lp, ln = tsp.route_length(board, p), tsp.route_length(board, n)
        ours = next((x for x in sk['pairs'] if x['p'] == [p] and x['n'] == [n]), None)
        tsp_rows.append(dict(pair=f'{p}/{n}', limit_mm=lim, route_length_p=round(lp, 4), route_length_n=round(ln, 4),
                             skew_mm=round(abs(lp - ln), 4),
                             verdict='PASS' if abs(lp - ln) <= lim else 'FAIL',
                             ours_track_only_skew_mm=ours['skew_track_only_mm'] if ours else None))
    out = dict(validate_route_metric=rows, max_track_only_deviation_mm=round(max_dev, 4),
               tune_sensitive_pairs_route_length=tsp_rows,
               counts=dict(groups=len(rows),
                           validate_route_fail=sum(1 for x in rows if x['repo_verdict'].get('validate_route.py') == 'FAIL'),
                           contract_fail=sum(1 for x in rows if x['repo_verdict'].get('sensitive_route_contract.py') == 'FAIL')))
    json.dump(out, open(a.out, 'w'), indent=1)
    print(json.dumps(out['counts']), 'max deviation', out['max_track_only_deviation_mm'])
    for x in rows:
        print(f"{x['pair'][:28]:28s} repo {x['repo_skew_mm']:7.3f} ours {x['ours_track_only_skew_mm']:7.3f} "
              f"barrels {x['ours_with_barrels_skew_mm']:7.3f} vias {x['repo_vias']:5s} {x['limits']} {x['repo_verdict']}")
    for x in tsp_rows:
        print('tsp', x)


if __name__ == '__main__':
    main()

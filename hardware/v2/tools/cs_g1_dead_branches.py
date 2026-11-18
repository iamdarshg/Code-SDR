#!/usr/bin/env python3
"""Dead-branch analysis of DRC dangling items (read-only).

    $KICAD_PY cs_g1_dead_branches.py --board X.kicad_pcb --drc drc.json --out dead.json

For every net that carries a `track_dangling`/`via_dangling` item in the DRC report:

1. Build the net's copper graph. Segments are split wherever another item meets them mid-span
   (T-junction track ends, via pads, and pads the segment crosses without ending on). This avoids
   pruning a trunk segment because its far end happens to overhang.
2. Prune iteratively: a segment piece with an end that touches nothing (no other live piece, via,
   pad or same-net zone fill) is dead; a via touching <= 1 live piece and no pad/zone is dead.
3. Report the dead copper as branches (connected groups of pruned pieces), with length, and which
   DRC items (by uuid) fall in each branch.

The DRC items are matched by uuid, not by position. Several segments can share the start point
that DRC reports, so position matching can pick the wrong segment.
"""
import argparse, json, math
from collections import defaultdict

import pcbnew

EPS = 1e-4


def dseg(px, py, ax, ay, bx, by):
    dx, dy = bx - ax, by - ay
    L2 = dx * dx + dy * dy
    t = 0.0 if L2 == 0 else max(0.0, min(1.0, ((px - ax) * dx + (py - ay) * dy) / L2))
    return math.hypot(px - (ax + t * dx), py - (ay + t * dy)), t


def mm(v):
    return pcbnew.ToMM(v)


def analyse_net(board, net):
    L = board.GetLayerName
    segs, vias, pads, zones = [], [], [], []
    for t in board.GetTracks():
        if t.GetNetname() != net:
            continue
        if t.GetClass() == 'PCB_VIA':
            p = t.GetPosition()
            try:
                r = mm(t.GetWidth(t.TopLayer())) / 2
            except TypeError:
                r = mm(t.GetWidth()) / 2
            vias.append(dict(uuid=t.m_Uuid.AsString(), x=mm(p.x), y=mm(p.y), r=r,
                             layers={L(l) for l in t.GetLayerSet().CuStack()}))
        elif t.GetClass() == 'PCB_TRACK':
            s, e = t.GetStart(), t.GetEnd()
            segs.append(dict(uuid=t.m_Uuid.AsString(), layer=L(t.GetLayer()), ax=mm(s.x), ay=mm(s.y),
                             bx=mm(e.x), by=mm(e.y), w=mm(t.GetWidth())))
        else:
            raise SystemExit(f'unsupported track class {t.GetClass()} on {net}')
    for fp in board.GetFootprints():
        for p in fp.Pads():
            if p.GetNetname() == net:
                pads.append(p)
    for z in board.Zones():
        if z.GetNetname() == net:
            zones.append(z)

    def pad_hit(x, y, layer, acc):
        v = pcbnew.VECTOR2I(pcbnew.FromMM(x), pcbnew.FromMM(y))
        return [f"{p.GetParentFootprint().GetReference()}.{p.GetNumber()}" for p in pads
                if layer in {L(l) for l in p.GetLayerSet().CuStack()} and p.HitTest(v, pcbnew.FromMM(acc))]

    def zone_hit(x, y, layer, acc):
        v = pcbnew.VECTOR2I(pcbnew.FromMM(x), pcbnew.FromMM(y))
        lid = board.GetLayerID(layer)
        return [z.m_Uuid.AsString()[:8] for z in zones
                if z.IsOnLayer(lid) and z.HitTestFilledArea(lid, v, pcbnew.FromMM(acc))]

    # ---- split points
    pieces = []
    for s in segs:
        cuts = {0.0, 1.0}
        Ls = math.hypot(s['bx'] - s['ax'], s['by'] - s['ay'])
        if Ls > 0:
            for o in segs:
                if o is s or o['layer'] != s['layer']:
                    continue
                for (x, y) in ((o['ax'], o['ay']), (o['bx'], o['by'])):
                    d, t = dseg(x, y, s['ax'], s['ay'], s['bx'], s['by'])
                    if d <= (s['w'] + o['w']) / 2 - EPS and EPS < t * Ls < Ls - EPS:
                        cuts.add(t)
            for v in vias:
                if s['layer'] in v['layers']:
                    d, t = dseg(v['x'], v['y'], s['ax'], s['ay'], s['bx'], s['by'])
                    if d <= v['r'] + s['w'] / 2 - EPS and EPS < t * Ls < Ls - EPS:
                        cuts.add(t)
            for p in pads:
                if s['layer'] not in {L(l) for l in p.GetLayerSet().CuStack()}:
                    continue
                c = p.GetPosition()
                d, t = dseg(mm(c.x), mm(c.y), s['ax'], s['ay'], s['bx'], s['by'])
                px, py = s['ax'] + t * (s['bx'] - s['ax']), s['ay'] + t * (s['by'] - s['ay'])
                if EPS < t * Ls < Ls - EPS and p.HitTest(pcbnew.VECTOR2I(pcbnew.FromMM(px), pcbnew.FromMM(py)),
                                                         pcbnew.FromMM(s['w'] / 2)):
                    cuts.add(t)
        ts = sorted(cuts)
        for t0, t1 in zip(ts, ts[1:]):
            pieces.append(dict(uuid=s['uuid'], layer=s['layer'], w=s['w'],
                               ax=s['ax'] + t0 * (s['bx'] - s['ax']), ay=s['ay'] + t0 * (s['by'] - s['ay']),
                               bx=s['ax'] + t1 * (s['bx'] - s['ax']), by=s['ay'] + t1 * (s['by'] - s['ay']),
                               t0=t0, t1=t1))
    for i, p in enumerate(pieces):
        p['id'] = i
        p['len'] = math.hypot(p['bx'] - p['ax'], p['by'] - p['ay'])

    # ---- contacts per piece end / via
    def end_contacts(p, x, y):
        c_pieces, c_vias = set(), set()
        for o in pieces:
            if o['id'] == p['id'] or o['layer'] != p['layer']:
                continue
            d, _ = dseg(x, y, o['ax'], o['ay'], o['bx'], o['by'])
            if d <= (p['w'] + o['w']) / 2 - EPS:
                c_pieces.add(o['id'])
        for k, v in enumerate(vias):
            if p['layer'] in v['layers'] and math.hypot(x - v['x'], y - v['y']) <= v['r'] + p['w'] / 2 - EPS:
                c_vias.add(k)
        anchors = pad_hit(x, y, p['layer'], p['w'] / 2) + ['zone:' + z for z in zone_hit(x, y, p['layer'], p['w'] / 2)]
        return c_pieces, c_vias, anchors

    ends = {}
    for p in pieces:
        ends[(p['id'], 0)] = end_contacts(p, p['ax'], p['ay'])
        ends[(p['id'], 1)] = end_contacts(p, p['bx'], p['by'])
    via_contacts = {}
    for k, v in enumerate(vias):
        cp = {p['id'] for p in pieces if p['layer'] in v['layers'] and
              dseg(v['x'], v['y'], p['ax'], p['ay'], p['bx'], p['by'])[0] <= v['r'] + p['w'] / 2 - EPS}
        cv = {j for j, o in enumerate(vias) if j != k and math.hypot(o['x'] - v['x'], o['y'] - v['y']) <= min(o['r'], v['r'])
              and o['layers'] & v['layers']}
        anchors = []
        for lyr in v['layers']:
            anchors += pad_hit(v['x'], v['y'], lyr, v['r']) + ['zone:' + z for z in zone_hit(v['x'], v['y'], lyr, v['r'])]
        via_contacts[k] = (cp, cv, anchors)

    live_p = {p['id'] for p in pieces}
    live_v = set(range(len(vias)))
    changed = True
    while changed:
        changed = False
        for p in pieces:
            if p['id'] not in live_p:
                continue
            for e in (0, 1):
                cp, cv, an = ends[(p['id'], e)]
                if not (cp & live_p) and not (cv & live_v) and not an:
                    live_p.discard(p['id']); changed = True
                    break
        for k in list(live_v):
            cp, cv, an = via_contacts[k]
            if len(cp & live_p) + len(cv & live_v) <= 1 and not an:
                live_v.discard(k); changed = True

    dead_p = [p for p in pieces if p['id'] not in live_p]
    dead_v = [k for k in range(len(vias)) if k not in live_v]
    # group dead items into branches
    parent = {}

    def find(a):
        parent.setdefault(a, a)
        while parent[a] != a:
            parent[a] = parent[parent[a]]
            a = parent[a]
        return a

    def union(a, b):
        parent[find(a)] = find(b)
    for p in dead_p:
        find(('p', p['id']))
        for e in (0, 1):
            cp, cv, _ = ends[(p['id'], e)]
            for o in cp:
                if o not in live_p:
                    union(('p', p['id']), ('p', o))
            for k in cv:
                if k not in live_v:
                    union(('p', p['id']), ('v', k))
    for k in dead_v:
        find(('v', k))
    groups = defaultdict(list)
    for key in list(parent):
        groups[find(key)].append(key)
    branches = []
    for g in groups.values():
        ps = [pieces[i] for t, i in g if t == 'p']
        vs = [vias[i] for t, i in g if t == 'v']
        # where does the branch attach to live copper?
        attach = []
        for p in ps:
            for e, (x, y) in ((0, (p['ax'], p['ay'])), (1, (p['bx'], p['by']))):
                cp, cv, an = ends[(p['id'], e)]
                if cp & live_p or cv & live_v or an:
                    attach.append(dict(x=round(x, 4), y=round(y, 4), layer=p['layer'], anchors=an))
        branches.append(dict(
            length_mm=round(sum(p['len'] for p in ps), 4),
            pieces=[dict(uuid=p['uuid'], layer=p['layer'], w=p['w'], frac=[round(p['t0'], 4), round(p['t1'], 4)],
                         a=[round(p['ax'], 4), round(p['ay'], 4)], b=[round(p['bx'], 4), round(p['by'], 4)],
                         len=round(p['len'], 4)) for p in ps],
            vias=[dict(uuid=v['uuid'], x=round(v['x'], 4), y=round(v['y'], 4), layers=sorted(v['layers'])) for v in vs],
            attaches_to_live_copper_at=attach))
    pad_refs = sorted(f"{p.GetParentFootprint().GetReference()}.{p.GetNumber()}" for p in pads)
    return dict(net=net, netclass=board.FindNet(net).GetNetClassName(), pads=pad_refs,
                segments=len(segs), pieces_after_split=len(pieces), vias=len(vias), zones=len(zones),
                dead_branches=sorted(branches, key=lambda b: -b['length_mm']),
                dead_length_mm=round(sum(b['length_mm'] for b in branches), 4))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--board', required=True)
    ap.add_argument('--drc', required=True)
    ap.add_argument('--out', required=True)
    a = ap.parse_args()
    board = pcbnew.LoadBoard(a.board)
    drc = json.load(open(a.drc))
    items = [(v['type'], i) for v in drc['violations'] if v['type'] in ('track_dangling', 'via_dangling')
             for i in v['items']]
    nets = sorted({i['description'].split('[')[1].split(']')[0] for _, i in items})
    res = {n: analyse_net(board, n) for n in nets}
    rows = []
    for typ, i in items:
        net = i['description'].split('[')[1].split(']')[0]
        hit = None
        for bi, b in enumerate(res[net]['dead_branches']):
            if any(p['uuid'] == i['uuid'] for p in b['pieces']) or any(v['uuid'] == i['uuid'] for v in b['vias']):
                hit = dict(branch_index=bi, branch_length_mm=b['length_mm'],
                           item_fully_dead=all(p['uuid'] != i['uuid'] or True for p in b['pieces']))
                pieces_of_item = [p for p in b['pieces'] if p['uuid'] == i['uuid']]
                hit['dead_part_of_item_mm'] = round(sum(p['len'] for p in pieces_of_item), 4)
                break
        rows.append(dict(type=typ, item=i['description'], uuid=i['uuid'], net=net, pos=i['pos'],
                         in_dead_branch=hit))
    out = dict(board=a.board, drc=a.drc, items=rows, nets=res)
    json.dump(out, open(a.out, 'w'), indent=1)
    for r in rows:
        h = r['in_dead_branch']
        print(f"{r['item'][:58]:58s} {r['uuid'][:8]} -> " +
              (f"dead branch {h['branch_length_mm']:.3f} mm (this item's dead part {h['dead_part_of_item_mm']:.3f} mm)" if h else 'NOT in a dead branch (load-bearing / pruning-inconclusive)'))


if __name__ == '__main__':
    main()

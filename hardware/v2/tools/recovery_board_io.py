"""Exchange exact KiCad copper geometry and additive route plans with Python.

Run with KiCad's bundled Python. Geometry planning uses a separate interpreter
with Shapely; every imported plan still requires zone refill and KiCad DRC.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path

import pcbnew as p


def point(v):
    return [p.ToMM(v.x), p.ToMM(v.y)]


def polygon_data(poly):
    def chain(c):
        return [point(c.CPoint(i)) for i in range(c.PointCount())]
    return [
        [chain(poly.COutline(i)), *[chain(poly.CHole(i, j)) for j in range(poly.HoleCount(i))]]
        for i in range(poly.OutlineCount())
    ]


def export_board(board_path, output):
    b = p.LoadBoard(str(board_path))
    layers = [int(x) for x in b.GetEnabledLayers().CuStack()]
    box = b.GetBoardEdgesBoundingBox()
    result = dict(source=str(board_path.resolve()), sha256=hashlib.sha256(board_path.read_bytes()).hexdigest(),
                  layers=layers, layer_names={l: b.GetLayerName(l) for l in layers},
                  bounds=[p.ToMM(box.GetLeft()), p.ToMM(box.GetTop()), p.ToMM(box.GetRight()), p.ToMM(box.GetBottom())],
                  items=[], zones=[], footprints=[])
    for fp in b.GetFootprints():
        fp.BuildCourtyardCaches()
        result['footprints'].append(dict(ref=fp.GetReference(), pos=point(fp.GetPosition()),
            angle=fp.GetOrientationDegrees(), layer=int(fp.GetLayer()),
            courtyard={layer:polygon_data(fp.GetCourtyard(layer)) for layer in (p.F_Cu,p.B_Cu)}))
        for pad in fp.Pads():
            shapes = {}
            for layer in layers:
                if pad.IsOnLayer(layer):
                    poly = p.SHAPE_POLY_SET()
                    pad.TransformShapeToPolygon(poly, layer, 0, p.FromMM(.002), p.ERROR_OUTSIDE)
                    shapes[layer] = polygon_data(poly)
            result['items'].append(dict(uuid=pad.m_Uuid.AsString(), kind='pad', net=pad.GetNetname(),
                ref=fp.GetReference(), number=pad.GetNumber(), pos=point(pad.GetPosition()),
                drill=point(pad.GetDrillSize()), shapes=shapes))
    for t in b.GetTracks():
        via = isinstance(t, p.PCB_VIA)
        result['items'].append(dict(uuid=t.m_Uuid.AsString(), kind='via' if via else 'track', net=t.GetNetname(),
            start=point(t.GetStart()), end=point(t.GetEnd()),
            width=p.ToMM(t.GetWidth(p.F_Cu) if via else t.GetWidth()),
            layers=[l for l in layers if t.IsOnLayer(l)] if via else [int(t.GetLayer())],
            drill=p.ToMM(t.GetDrillValue()) if via else 0))
    for z in b.Zones():
        if z.GetIsRuleArea():
            raise RuntimeError('Rule-area export needs explicit implementation before routing this board')
        result['zones'].append(dict(uuid=z.m_Uuid.AsString(), net=z.GetNetname(),
            layer=int(z.GetLayer()), polygons=polygon_data(z.GetFilledPolysList(z.GetLayer()))))
    output.write_text(json.dumps(result, separators=(',', ':')), encoding='utf-8')
    print(json.dumps(dict(items=len(result['items']), zones=len(result['zones']), output=str(output))), flush=True)


def import_plan(board_path, plan_path, output, refill=False):
    plan = json.loads(plan_path.read_text(encoding='utf-8'))
    actual = hashlib.sha256(board_path.read_bytes()).hexdigest()
    if plan['source_sha256'] != actual:
        raise RuntimeError('Plan source hash does not match board; refusing stale routes')
    b = p.LoadBoard(str(board_path))
    nets = {n.GetNetname(): n for n in b.GetNetsByNetcode().values()}
    def vec(v):
        return p.VECTOR2I(p.FromMM(v[0]), p.FromMM(v[1]))
    for route in plan['routes']:
        net = nets[route['net']]
        for seg in route.get('segments', []):
            t = p.PCB_TRACK(b)
            t.SetStart(vec(seg['start']))
            t.SetEnd(vec(seg['end']))
            t.SetLayer(seg['layer'])
            t.SetWidth(p.FromMM(seg['width']))
            t.SetNet(net)
            b.Add(t)
        for via in route.get('vias', []):
            v = p.PCB_VIA(b)
            v.SetPosition(vec(via['pos']))
            v.SetWidth(p.FromMM(via['diameter']))
            v.SetDrill(p.FromMM(via['drill']))
            v.SetViaType(p.VIATYPE_THROUGH)
            v.SetLayerPair(p.F_Cu, p.B_Cu)
            v.SetNet(net)
            b.Add(v)
    b.BuildConnectivity()
    if refill:
        p.ZONE_FILLER(b).Fill(b.Zones())
    p.SaveBoard(str(output), b)
    print(json.dumps(dict(routes=len(plan['routes']), output=str(output))), flush=True)


if __name__ == '__main__':
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('action', choices=['export', 'import'])
    ap.add_argument('board', type=Path)
    ap.add_argument('output', type=Path)
    ap.add_argument('--plan', type=Path)
    ap.add_argument('--refill', action='store_true',
                    help='Refill zones after import (default: keep existing fills)')
    args = ap.parse_args()
    if args.action == 'export':
        export_board(args.board, args.output)
    else:
        import_plan(args.board, args.plan, args.output, refill=args.refill)

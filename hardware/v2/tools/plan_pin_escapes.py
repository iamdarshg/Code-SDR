"""Plan filled, capped through-via escapes for trapped digital IC pads.

This is an explicit fabrication-dependent candidate, not a board mutation.
No RF net is selected implicitly. Import only together with routes that improve
connectivity, and run whole-board DRC. Any via whose drill intersects a solder
land requires resin fill and copper cap before assembly.
"""
from pathlib import Path
import argparse
import json
from shapely.geometry import Point
from recovery_route import Copper


def add_escapes(copper, nets):
    records = []
    for net in nets:
        groups = copper.groups(net)
        if len(groups) < 2:
            continue
        for group in groups:
            # Only a conductor confined to top-layer IC lands and short escape
            # tracks needs this special escape. Earlier routing passes may have
            # attached a legal stub to the land without reaching another layer.
            nodes = [copper.nodes[u] for u in group]
            if any(n['kind'] not in ('pad', 'track') for n in nodes):
                continue
            if any(any(layer not in copper.outer for layer in n['shapes']) for n in nodes):
                continue
            for node in nodes:
                item = node['item']
                if not item.get('ref', '').startswith('U'):
                    continue
                pt = item['pos']
                shape = Point(pt).buffer(.2, quad_segs=32)
                if any(not copper.copper_clear(shape, l, net, clearance=.100001)
                       for l in copper.layers):
                    continue
                hole_clearance = Point(pt).buffer(.1 + .100001, quad_segs=32)
                if len(copper.hole_tree.query(hole_clearance, predicate='intersects')):
                    continue
                route = dict(net=net, reason='filled and copper-capped IC pad escape',
                             segments=[], vias=[dict(pos=pt, diameter=.4, drill=.2)],
                             assembly_requirement='resin filled and copper capped via in solder land',
                             ref=item['ref'], pin=item['number'])
                copper.accept_planned(route)
                records.append(dict(ref=item['ref'], pin=item['number'], net=net, pos=pt))
                break
    return records


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('geometry', type=Path)
    ap.add_argument('output', type=Path)
    ap.add_argument('--nets', nargs='+', required=True)
    ap.add_argument('--route', action='store_true')
    args = ap.parse_args()
    data = json.loads(args.geometry.read_text(encoding='utf-8'))
    copper = Copper(data)
    escapes = add_escapes(copper, args.nets)
    def checkpoint():
        args.output.write_text(json.dumps(dict(source_sha256=data['sha256'],
            routes=copper.routes, filled_capped_escapes=escapes), indent=2)+'\n', encoding='utf-8')
    checkpoint()
    print(json.dumps(dict(escapes=escapes)), flush=True)
    if args.route:
        copper.maze_nets(args.nets, .1, checkpoint, pair_limit=4)
    checkpoint()


if __name__ == '__main__':
    main()

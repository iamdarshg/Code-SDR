"""Apply reviewed placement ripups and short capacitor via escapes to a candidate.

This is an auditable integration step, not a routing or DRC acceptance gate.
All proposed pad locations and nets are verified before changing any copper.
"""
import argparse
import hashlib
import json
import math
from pathlib import Path
import pcbnew as p


def xy(v):
    return [p.ToMM(v.x), p.ToMM(v.y)]


def vec(v):
    return p.VECTOR2I(p.FromMM(v[0]), p.FromMM(v[1]))


def apply(source, output, placements, proposal):
    if source.resolve() == output.resolve():
        raise ValueError('Use a separate candidate output')
    source_hash = hashlib.sha256(source.read_bytes()).hexdigest()
    review = json.loads(placements.read_text())
    caps = json.loads(proposal.read_text())
    b = p.LoadBoard(str(source))
    fps = {fp.GetReference(): fp for fp in b.GetFootprints()}
    nets = {n.GetNetname(): n for n in b.GetNetsByNetcode().values()}
    tracks = {t.m_Uuid.AsString(): t for t in b.GetTracks()}
    reasons = {}
    for ref, item in review['components'].items():
        for uid in item.get('reroute_track_uuids', []):
            reasons.setdefault(uid, []).append(ref + ': relocated land clearance')
    for ref, item in caps['placements'].items():
        fp = fps[ref]
        if fp.GetLayer() != p.B_Cu:
            raise ValueError(f'{ref}: expected B.Cu')
        pads = {pad.GetNumber(): pad for pad in fp.Pads()}
        for connection in item['connections']:
            pad = pads[connection['pin']]
            if pad.GetNetname() != connection['net'] or math.dist(xy(pad.GetPosition()), connection['pad_xy_mm']) > .002:
                raise ValueError(f'{ref}.{connection["pin"]}: proposal differs from actual pad')
        for uid in item['existing_track_uuids_to_reroute']:
            reasons.setdefault(uid, []).append(ref + ': capacitor escape clearance')
    receipt = dict(source_sha256=source_hash, removed=[], already_absent=[], added=[], reused=[])
    for uid, why in sorted(reasons.items()):
        t = tracks.get(uid)
        if t is None:
            receipt['already_absent'].append(uid)
            continue
        receipt['removed'].append(dict(uuid=uid, net=t.GetNetname(), kind='via' if isinstance(t, p.PCB_VIA) else 'track', start=xy(t.GetStart()), end=xy(t.GetEnd()), reasons=why))
        b.Delete(t)
    # Rebuild references after deletions; never dereference deleted SWIG objects.
    existing_vias = [t for t in b.GetTracks() if isinstance(t, p.PCB_VIA)]
    for ref, item in caps['placements'].items():
        for c in item['connections']:
            net = nets[c['net']]
            via = next((v for v in existing_vias if v.GetNetname() == c['net'] and math.dist(xy(v.GetPosition()), c['via_xy_mm']) < .002), None)
            if via is None:
                if c['reuse_existing_via']:
                    raise ValueError(f'{ref}: required existing via missing')
                via = p.PCB_VIA(b)
                via.SetPosition(vec(c['via_xy_mm']))
                via.SetWidth(p.FromMM(c['via_diameter_mm']))
                via.SetDrill(p.FromMM(c['via_drill_mm']))
                via.SetViaType(p.VIATYPE_THROUGH)
                via.SetLayerPair(p.F_Cu, p.B_Cu)
                via.SetNet(net)
                b.Add(via)
                existing_vias.append(via)
                receipt['added'].append(dict(uuid=via.m_Uuid.AsString(), kind='via', ref=ref, net=c['net'], pos=c['via_xy_mm']))
            else:
                receipt['reused'].append(dict(uuid=via.m_Uuid.AsString(), ref=ref, net=c['net']))
            if math.dist(c['pad_xy_mm'], c['via_xy_mm']) > .001:
                t = p.PCB_TRACK(b)
                t.SetStart(vec(c['pad_xy_mm']))
                t.SetEnd(vec(c['via_xy_mm']))
                t.SetWidth(p.FromMM(c['B_Cu_stub_width_mm']))
                t.SetLayer(p.B_Cu)
                t.SetNet(net)
                b.Add(t)
                receipt['added'].append(dict(uuid=t.m_Uuid.AsString(), kind='track', ref=ref, net=c['net'], start=c['pad_xy_mm'], end=c['via_xy_mm']))
    b.BuildConnectivity()
    p.ZONE_FILLER(b).Fill(b.Zones())
    if hashlib.sha256(source.read_bytes()).hexdigest() != source_hash:
        raise RuntimeError('Source changed during integration')
    p.SaveBoard(str(output), b)
    receipt['output_sha256'] = hashlib.sha256(output.read_bytes()).hexdigest()
    output.with_suffix('.cap-escapes.json').write_text(json.dumps(receipt, indent=2)+'\n')
    print(json.dumps({k:len(receipt[k]) for k in ('removed','already_absent','added','reused')}), flush=True)


if __name__ == '__main__':
    ap = argparse.ArgumentParser(description=__doc__)
    for name in ('source', 'output', 'placements', 'proposal'):
        ap.add_argument(name, type=Path)
    args = ap.parse_args()
    apply(args.source, args.output, args.placements, args.proposal)

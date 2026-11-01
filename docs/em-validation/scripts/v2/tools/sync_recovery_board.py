"""Apply reviewed model/package changes to a routed board without regeneration.

Run with KiCad Python on a full-context candidate. This deliberately does not
promote its output: inspect the removal receipt, refill, run DRC/parity, then
route the new connections before accepting the candidate. New/relocated parts
require explicit reviewed placements in a JSON integration specification.
"""
from __future__ import annotations

import argparse
from collections import defaultdict
import hashlib
import json
import math
from pathlib import Path
import xml.etree.ElementTree as ET

import pcbnew as p
import design_model as model

SYSTEM_LIBS = Path('C:/Program Files/KiCad/9.0/share/kicad/footprints')


def xy(point):
    return [p.ToMM(point.x), p.ToMM(point.y)]


def vec(point):
    return p.VECTOR2I(p.FromMM(point[0]), p.FromMM(point[1]))


def load(identifier):
    library, name = identifier.split(':', 1)
    directory = model.ROOT / 'CodeSDR.pretty' if library == 'CodeSDR' else SYSTEM_LIBS / (library+'.pretty')
    fp = p.FootprintLoad(str(directory), name)
    if fp is None: raise ValueError(f'Cannot load {identifier}')
    fp.SetFPID(p.LIB_ID(library, name))
    return fp


def prune_terminal_branches(board, changed_pads, receipt, retained=None):
    """Prune straight terminal chains; retain unsupported or ambiguous contacts.

    The graph describes the original board, independently of planned deletions.
    Vias and arcs are permanent boundaries, not traversable graph edges. Interior
    contacts are retained for manual review; this helper does not split copper.
    """
    tracks = list(board.GetTracks())
    all_pads = list(board.GetPads())
    changed_ids = {pad.m_Uuid.AsString() for pad in changed_pads}
    removed, reported = set(), set()
    retained = [] if retained is None else retained
    adjacent = defaultdict(list)
    def key(point): return (point.x, point.y)
    def straight(t): return not isinstance(t, (p.PCB_VIA, p.PCB_ARC))
    def layer_contact(obj, layer): return obj.IsOnLayer(layer)
    def keep(t, reason):
        uid = t.m_Uuid.AsString()
        if uid not in reported:
            reported.add(uid)
            retained.append(dict(uuid=uid, net=t.GetNetname(), reason=reason))
    def interior(point, start, end):
        dx, dy = end.x-start.x, end.y-start.y
        dot = (point.x-start.x)*dx + (point.y-start.y)*dy
        return 0 < dot < dx*dx + dy*dy
    def intersects_box(start, end, box, margin=0):
        # Conservative for non-rectangular pads/arcs. False positives retain
        # copper, so an unsupported shape cannot cause an over-deletion.
        lo, hi = 0.0, 1.0
        for a, d, low, high in ((start.x, end.x-start.x, box.GetLeft()-margin, box.GetRight()+margin),
                                (start.y, end.y-start.y, box.GetTop()-margin, box.GetBottom()+margin)):
            if not d:
                if a < low or a > high: return False
            else:
                first, last = sorted(((low-a)/d, (high-a)/d))
                lo, hi = max(lo, first), min(hi, last)
                if lo > hi: return False
        return hi > 0 and lo < 1
    def interior_crossing(t, other):
        a, b, c, d = t.GetStart(), t.GetEnd(), other.GetStart(), other.GetEnd()
        rx, ry, sx, sy = b.x-a.x, b.y-a.y, d.x-c.x, d.y-c.y
        cross = rx*sy-ry*sx
        if cross:
            along = ((c.x-a.x)*sy-(c.y-a.y)*sx)/cross
            across = ((c.x-a.x)*ry-(c.y-a.y)*rx)/cross
            if 0 < along < 1 and 0 <= across <= 1: return True
        # Also retain interior copper contacts near another segment's endpoint,
        # including collinear overlaps, using KiCad's width-aware hit test.
        return any(interior(point, a, b) and t.HitTest(point, (other.GetWidth()+1)//2)
                   for point in (c, d))
    def ambiguous(t):
        start, end, layer, net = t.GetStart(), t.GetEnd(), t.GetLayer(), t.GetNetCode()
        for pad in all_pads:
            if pad.GetNetCode() != net or not layer_contact(pad, layer): continue
            if pad.HitTest(start) or pad.HitTest(end): continue
            if intersects_box(start, end, pad.GetBoundingBox(), (t.GetWidth()+1)//2):
                return 'retained: possible interior pad contact requires manual review'
        for other in tracks:
            if other.m_Uuid == t.m_Uuid or other.GetNetCode() != net or not layer_contact(other, layer): continue
            if straight(other):
                if interior_crossing(t, other):
                    return 'retained: interior track contact requires manual review'
            elif isinstance(other, p.PCB_ARC) or not (other.HitTest(start) or other.HitTest(end)):
                if intersects_box(start, end, other.GetBoundingBox(), (t.GetWidth()+1)//2):
                    return 'retained: possible interior via/arc contact requires manual review'
        return None
    for t in tracks:
        if not straight(t): continue
        adjacent[(t.GetNetCode(), t.GetLayer(), key(t.GetStart()))].append(t)
        if t.GetEnd() != t.GetStart(): adjacent[(t.GetNetCode(), t.GetLayer(), key(t.GetEnd()))].append(t)
    def other_pad(point, net, layer, margin):
        return any(pad.m_Uuid.AsString() not in changed_ids and pad.GetNetCode() == net
                   and layer_contact(pad, layer) and pad.HitTest(point, margin) for pad in all_pads)
    def fixed_boundary(point, net, layer, margin):
        return any(t.GetNetCode() == net and layer_contact(t, layer) and t.HitTest(point, margin)
                   and (not straight(t) or interior(point, t.GetStart(), t.GetEnd())) for t in tracks)
    queue = []
    for pad in changed_pads:
        if not pad.GetNetCode(): continue
        for t in tracks:
            if t.GetNetCode() != pad.GetNetCode(): continue
            if isinstance(t, p.PCB_VIA):
                if any(t.IsOnLayer(layer) for layer in pad.GetLayerSet().CuStack()) and pad.HitTest(t.GetStart()):
                    keep(t, 'retained: via attached to changed terminal')
                continue
            if not pad.IsOnLayer(t.GetLayer()): continue
            if not straight(t):
                if pad.HitTest(t.GetStart()) or pad.HitTest(t.GetEnd()):
                    keep(t, 'retained: arc attached to changed terminal')
                continue
            if pad.HitTest(t.GetStart()): queue.append((t, t.GetStart()))
            elif pad.HitTest(t.GetEnd()): queue.append((t, t.GetEnd()))
            elif intersects_box(t.GetStart(), t.GetEnd(), pad.GetBoundingBox(), (t.GetWidth()+1)//2):
                keep(t, 'retained: changed terminal contacts segment interior')
    while queue:
        t, near = queue.pop()
        uid = t.m_Uuid.AsString()
        if uid in removed: continue
        reason = ambiguous(t)
        if reason:
            keep(t, reason)
            continue
        far = t.GetEnd() if near == t.GetStart() else t.GetStart()
        removed.add(uid)
        receipt.append(dict(uuid=uid, net=t.GetNetname(), start=xy(t.GetStart()), end=xy(t.GetEnd()),
                            kind='segment', layer=t.GetLayerName(), width=p.ToMM(t.GetWidth()),
                            reason='detach changed terminal to first junction or other pad'))
        net = t.GetNetCode()
        margin = (t.GetWidth()+1)//2
        if other_pad(far, net, t.GetLayer(), margin) or fixed_boundary(far, net, t.GetLayer(), margin): continue
        original_items = adjacent[(net, t.GetLayer(), key(far))]
        if len(original_items) == 2:
            next_item = next(n for n in original_items if n.m_Uuid.AsString() != uid)
            queue.append((next_item, far))
    for t in tracks:
        if t.m_Uuid.AsString() in removed: board.Delete(t)


def schematic_pin_nets(netlist_path, items):
    """Validate exported connectivity, retaining KiCad's singleton NC nets.

    Some intentionally open pins survive netlist export as passive+no_connect
    singleton nets. Preserve those exact names on the PCB for schematic parity.
    They must never become routable connections to a second terminal.
    """
    tree = ET.parse(netlist_path).getroot()
    if {n.get('ref') for n in tree.findall('./components/comp')} != set(items):
        raise ValueError('Netlist components differ from model')
    actual = {}
    for net in tree.findall('./nets/net'):
        nodes = net.findall('node')
        for node in nodes:
            key = (node.get('ref'), node.get('pin'))
            if key in actual: raise ValueError(f'Duplicate netlist terminal: {key}')
            actual[key] = (net.get('name'), len(nodes), node.get('pintype', ''))
    expected = {(ref, pin) for ref, item in items.items() for pin in item.pins}
    if set(actual)-expected: raise ValueError('Netlist contains unmodelled terminals')
    result = {}
    for ref, item in items.items():
        for pin, wanted in item.pins.items():
            exported = actual.get((ref, pin))
            name = exported[0] if exported else None
            if wanted is not None and name != wanted:
                raise ValueError(f'Netlist/model mismatch: {ref}.{pin}: {name} != {wanted}')
            if wanted is None and exported:
                if not (name.startswith('unconnected-(') and exported[1] == 1
                        and 'no_connect' in exported[2].split('+')):
                    raise ValueError(f'Intentionally open terminal has a connection: {ref}.{pin}')
            result[(ref, pin)] = name
    return result


def sync(board_path, output, spec_path, paths_path, netlist_path=None):
    board_path, output = Path(board_path).resolve(), Path(output).resolve()
    if board_path == output or (output.exists() and board_path.samefile(output)):
        raise ValueError('Candidate output must differ from the source board')
    spec = json.loads(spec_path.read_text(encoding='utf-8'))
    source_sha = hashlib.sha256(board_path.read_bytes()).hexdigest()
    if not spec.get('source_sha256'):
        raise ValueError('Integration requires source_sha256')
    if spec['source_sha256'] != source_sha:
        raise RuntimeError('Integration source changed')
    paths = json.loads(paths_path.read_text(encoding='utf-8'))['components']
    board = p.LoadBoard(str(board_path))
    footprints = list(board.GetFootprints())
    existing = {fp.GetReference(): fp for fp in footprints}
    items = {item.ref:item for item in model.components}
    pin_nets = (schematic_pin_nets(netlist_path, items) if netlist_path else
                {(ref, pin): net for ref, item in items.items() for pin, net in item.pins.items()})
    if len(existing) != len(footprints): raise ValueError('Duplicate reference in board')
    if len(items) != len(model.components): raise ValueError('Duplicate reference in model')
    if set(existing)-set(items): raise ValueError('Model omits existing board components')
    placements = spec.get('placements', {})
    for ref, placement in placements.items():
        if not isinstance(placement, list) or len(placement) not in (3, 4):
            raise ValueError(f'{ref}: placement must be [x, y, angle, optional copper side]')
        if not all(isinstance(value, (int, float)) and math.isfinite(value) for value in placement[:3]):
            raise ValueError(f'{ref}: placement coordinates and angle must be finite numbers')
        if len(placement) == 4 and placement[3] not in ('F.Cu', 'B.Cu'):
            raise ValueError(f'{ref}: placement side must be F.Cu or B.Cu')
    replace = set(spec.get('replace_footprints', []))
    rename = spec.get('net_renames', {})
    if set(rename.values()) - set(model.all_nets()): raise ValueError('Unknown target net alias')
    if any(zone.GetNetname() in rename for zone in board.Zones()):
        raise ValueError('Net alias used by a zone requires explicit zone handling')
    replace |= {ref for ref,fp in existing.items() if fp.GetFPIDAsString() != items[ref].footprint}
    replace |= set(placements) & set(existing)
    missing = set(items)-set(existing)
    if missing-set(placements): raise ValueError(f'Explicit placement required: {sorted(missing-set(placements))}')
    if (set(placements)|replace)-set(items): raise ValueError('Unknown integration reference')
    nets = {n.GetNetname():n for n in board.GetNetsByNetcode().values()}
    for name in sorted({net for net in pin_nets.values() if net}):
        if name not in nets:
            n=p.NETINFO_ITEM(board,name);board.Add(n);nets[name]=n
    # Explicit whole-net aliases are safe only when their intended terminal
    # changes have already been reviewed in the schematic integration spec.
    renamed=[]
    for obj in list(board.GetTracks())+list(board.GetPads()):
        if obj.GetNetname() in rename:
            old=obj.GetNetname();obj.SetNet(nets[rename[old]])
            renamed.append(dict(uuid=obj.m_Uuid.AsString(),old=old,new=rename[old]))
    detached=[]
    for ref,fp in existing.items():
        for pad in fp.Pads():
            number=pad.GetNumber()
            if not number: continue
            wanted=pin_nets.get((ref, number))
            if ref in replace or pad.GetNetname() != (wanted or ''):
                detached.append(pad)
    receipt=dict(source_sha256=source_sha,replaced=sorted(replace),added=sorted(missing),
                 spec_sha256=hashlib.sha256(spec_path.read_bytes()).hexdigest(),
                 paths_sha256=hashlib.sha256(paths_path.read_bytes()).hexdigest(),
                 model_sha256=hashlib.sha256(Path(model.__file__).read_bytes()).hexdigest(),
                 renamed=renamed,removed_tracks=[],retained_tracks=[],changed_pins=[],placements=placements)
    if netlist_path:
        receipt['netlist_sha256'] = hashlib.sha256(Path(netlist_path).read_bytes()).hexdigest()
        receipt['intentional_no_connect_nets'] = {
            f'{ref}.{pin}': net for (ref, pin), net in pin_nets.items()
            if items[ref].pins[pin] is None and net}
    prune_terminal_branches(board,detached,receipt['removed_tracks'],receipt['retained_tracks'])
    for ref,item in items.items():
        old=existing.get(ref)
        if old is None or ref in replace:
            fp=load(item.footprint)
            if old is not None:
                if old.GetLayer() not in (p.F_Cu, p.B_Cu): raise ValueError(f'Unsupported footprint side: {ref}')
                side = old.GetLayer()
                fp.m_Uuid.Clone(old.m_Uuid)
                fp.SetPath(old.GetPath())
                position=xy(old.GetPosition());angle=old.GetOrientationDegrees()
                by_number=defaultdict(list)
                for pad in old.Pads():
                    if pad.GetNumber(): by_number[pad.GetNumber()].append(pad.m_Uuid.AsString())
                for pad in fp.Pads():
                    if by_number[pad.GetNumber()]: pad.m_Uuid.Clone(p.KIID(by_number[pad.GetNumber()].pop(0)))
            else:
                position=[0,0];angle=item.rotation;side=p.F_Cu
                fp.SetPath(p.KIID_PATH(paths[ref]['path']))
            if ref in placements:
                position=placements[ref][:2];angle=placements[ref][2]
                if len(placements[ref]) == 4:
                    side = p.B_Cu if placements[ref][3] == 'B.Cu' else p.F_Cu
            # Flip consults its parent board's copper stack; a detached library
            # footprint crashes the installed KiCad runtime here.
            if old is not None: board.Delete(old)
            board.Add(fp)
            fp.SetPosition(vec(position))
            if fp.GetLayer() != side:
                fp.Flip(fp.GetPosition(), p.FLIP_DIRECTION_TOP_BOTTOM)
            fp.SetOrientationDegrees(angle)
            fp.SetReference(ref);fp.SetSheetname(item.sheet);fp.SetSheetfile(f'sheets/{item.sheet}.kicad_sch')
            fp.Reference().SetVisible(False);fp.Value().SetVisible(False)
        else: fp=old
        fp.SetValue(item.value);fp.SetDNP(bool(item.dnp))
        for pad in fp.Pads():
            number=pad.GetNumber()
            if not number: continue
            if number not in item.pins: raise ValueError(f'{ref}: footprint has unmodelled pad {number}')
            wanted=pin_nets[(ref, number)]
            old_net=pad.GetNetname()
            if old_net!=(wanted or ''): receipt['changed_pins'].append(dict(ref=ref,pin=number,old=old_net,new=wanted))
            if wanted: pad.SetNet(nets[wanted])
            else: pad.SetNetCode(0)
        physical={pad.GetNumber() for pad in fp.Pads() if pad.GetNumber()}
        if set(item.pins)-physical: raise ValueError(f'{ref}: model pin absent from footprint {set(item.pins)-physical}')
    board.BuildConnectivity();p.ZONE_FILLER(board).Fill(board.Zones())
    if hashlib.sha256(board_path.read_bytes()).hexdigest() != source_sha:
        raise RuntimeError('Integration source changed during planning')
    p.SaveBoard(str(output),board)
    receipt['output_sha256']=hashlib.sha256(output.read_bytes()).hexdigest()
    receipt['component_count']=len(items)
    receipt_path=output.with_suffix('.integration.json')
    receipt_path.write_text(json.dumps(receipt,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(dict(output=str(output),receipt=str(receipt_path),components=len(items),
                         replaced=len(replace),added=len(missing),removed_tracks=len(receipt['removed_tracks']))),flush=True)


if __name__=='__main__':
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('board',type=Path);ap.add_argument('output',type=Path)
    ap.add_argument('spec',type=Path);ap.add_argument('--paths',type=Path,default=model.ROOT/'build/schematic_paths.json')
    ap.add_argument('--netlist',type=Path,help='Fresh KiCad XML netlist; preserves exact singleton no-connect nets')
    args=ap.parse_args();sync(args.board,args.output,args.spec,args.paths,args.netlist)

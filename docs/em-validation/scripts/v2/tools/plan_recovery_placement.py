"""Reserve component courtyards and lands before rebuilding a recovery candidate.

Run export with KiCad Python, solve with system Python. Fixed component lands and
drilled vias are hard obstacles; conflicting track UUIDs are an explicit rerouting
list. This does not modify a board or replace subsequent complete KiCad DRC.
"""
import argparse
import json
import math
from pathlib import Path


def export(board_path, spec_path, output):
    import pcbnew as p
    from recovery_board_io import polygon_data, point
    from sync_recovery_board import load
    import design_model
    models={c.ref:c for c in design_model.components}
    board=p.LoadBoard(str(board_path))
    spec=json.loads(spec_path.read_text())
    refs=set(spec['placements'])
    result={'spec':spec,'footprints':{},'variants':{}}
    for fp in board.GetFootprints():
        ref=fp.GetReference();fp.BuildCourtyardCaches()
        result['footprints'][ref]={'layer':int(fp.GetLayer()),'pos':point(fp.GetPosition()),
            'court':polygon_data(fp.GetCourtyard(fp.GetLayer()))}
        if ref not in refs:continue
        desired=spec['placements'][ref]
        side=p.B_Cu if len(desired)>3 and desired[3]=='B.Cu' else p.F_Cu
        variants=[]
        # Keep IC orientation and RF handedness; allow 180 degree passive reversal.
        angles=[desired[2]] if ref.startswith(('U','Y','T','FL')) else [desired[2],(desired[2]+180)%360]
        for angle in angles:
            temp=p.BOARD();copy=load(models[ref].footprint);temp.Add(copy)
            copy.SetPosition(p.VECTOR2I(0,0))
            if copy.GetLayer()!=side:copy.Flip(copy.GetPosition(),p.FLIP_DIRECTION_TOP_BOTTOM)
            copy.SetOrientationDegrees(angle);copy.BuildCourtyardCaches()
            pads=[]
            for pad in copy.Pads():
                if not pad.GetNumber():continue
                shapes={}
                for layer in (0,4,6,2):
                    if pad.IsOnLayer(layer):
                        poly=p.SHAPE_POLY_SET();pad.TransformShapeToPolygon(poly,layer,0,p.FromMM(.002),p.ERROR_OUTSIDE)
                        shapes[layer]=polygon_data(poly)
                pads.append({'number':pad.GetNumber(),'net':models[ref].pins.get(pad.GetNumber()) or '', 'pos':point(pad.GetPosition()),'drill':point(pad.GetDrillSize()),'shapes':shapes})
            variants.append({'angle':angle,'layer':int(side),'court':polygon_data(copy.GetCourtyard(side)),'pads':pads})
        result['variants'][ref]=variants
    output.write_text(json.dumps(result,separators=(',',':')))
    print('Exported',len(result['variants']),'movable component templates',flush=True)


def solve(inputs_path, geometry_path, output):
    from shapely import STRtree,union_all
    from shapely.affinity import translate
    from shapely.geometry import Point,box
    from recovery_route import Copper,polygons
    inp=json.loads(inputs_path.read_text());data=json.loads(geometry_path.read_text())
    spec=inp['spec'];refs=set(spec['placements']);copper=Copper(data)
    fixed=[(ref,f['layer'],union_all(polygons(f['court']))) for ref,f in inp['footprints'].items() if ref not in refs]
    court_tree=STRtree([f[2] for f in fixed]);reserved=[];reserved_pads=[];reserved_holes=[];report={}
    def overlap_court(shape,layer):
        return any(fixed[i][1]==layer for i in court_tree.query(shape.buffer(.025),predicate='intersects')) or any(l==layer and shape.buffer(.025).intersects(s) for l,s in reserved)
    # Put the largest bodies first, so small passives cannot steal their only pocket.
    order=sorted(refs,key=lambda ref:-union_all(polygons(inp['variants'][ref][0]['court'])).area)
    for ref in order:
        wanted=spec['placements'][ref];best=None
        radius=spec.get('search_radius',{}).get(ref,3.0)
        offsets=[(ix*.2,iy*.2) for ix in range(-math.ceil(radius*5),math.ceil(radius*5)+1) for iy in range(-math.ceil(radius*5),math.ceil(radius*5)+1) if math.hypot(ix*.2,iy*.2)<=radius+.001]
        offsets.sort(key=lambda q:math.hypot(*q))
        for variant in inp['variants'][ref]:
            court=union_all(polygons(variant['court']));layer=variant['layer']
            pp=[(p['net'],{int(l):union_all(polygons(poly)) for l,poly in p['shapes'].items()}) for p in variant['pads']]
            for dx,dy in offsets:
                dist=math.hypot(dx,dy)
                if best and dist>best[0]:continue
                x,y=round(wanted[0]+dx,5),round(wanted[1]+dy,5)
                ct=translate(court,xoff=x,yoff=y)
                if not box(.25,.25,98.75,98.75).covers(ct) or overlap_court(ct,layer):continue
                blockers=set();bad=False;moved_pads=[]
                for net,shapes in pp:
                    for l,shape in shapes.items():
                        moved=translate(shape,xoff=x,yoff=y)
                        moved_pads.append((net,l,moved))
                        if any(l==rl and net!=rn and moved.distance(rs)<.115 for rn,rl,rs in reserved_pads) or any(moved.distance(h)<.115 for h in reserved_holes):bad=True;break
                        for index in copper.trees[l].query(moved.buffer(.115),predicate='intersects'):
                            uid,other=copper.by_layer[l][index];node=copper.nodes[uid]
                            if node['kind']=='zone' or (node['kind']=='pad' and node['item']['ref'] in refs):continue
                            if node['kind']=='via' and moved.distance(Point(node['item']['start']))<node['item']['drill']/2+.115:bad=True;break
                            if node['net']==net:continue
                            if node['kind']=='track':blockers.add(uid)
                            else:bad=True;break
                        if bad:break
                    if bad:break
                if bad:continue
                # Prefer short physical displacement, then fewer tracks to reroute.
                score=dist+min(len(blockers),10)*.05+(.025 if variant['angle']!=wanted[2] else 0)
                holes=[Point(x+p['pos'][0],y+p['pos'][1]).buffer(max(p['drill'])/2) for p in variant['pads'] if max(p['drill'])>0]
                if any(h.distance(s)<.115 for h in holes for n,l,s in reserved_pads):continue
                if best is None or score<best[0]:best=(score,[x,y,variant['angle'],'B.Cu' if layer==2 else 'F.Cu'],ct,layer,sorted(blockers),moved_pads,holes)
        if best is None:
            report[ref]={'status':'no_clear_placement','requested':wanted,'radius':radius}
            print(ref,'NO PLACEMENT',flush=True);continue
        score,placement,ct,layer,blockers,moved_pads,holes=best
        spec['placements'][ref]=placement;reserved.append((layer,ct));reserved_pads.extend(moved_pads);reserved_holes.extend(holes)
        report[ref]={'status':'reserved','placement':placement,'displacement_mm':math.dist(wanted[:2],placement[:2]),'reroute_track_uuids':blockers}
        print(ref,placement,'reroute',len(blockers),flush=True)
    output.write_text(json.dumps(spec,indent=2)+'\n')
    output.with_suffix('.placement-review.json').write_text(json.dumps({'geometry_sha256':data['sha256'],'components':report},indent=2)+'\n')
    if any(r['status']!='reserved' for r in report.values()):raise SystemExit('Unresolved placement entries: inspect report before integration')


if __name__=='__main__':
    ap=argparse.ArgumentParser(description=__doc__);sub=ap.add_subparsers(dest='command',required=True)
    for name in ('export','solve'):
        q=sub.add_parser(name);q.add_argument('first',type=Path);q.add_argument('second',type=Path);q.add_argument('output',type=Path)
    a=ap.parse_args()
    (export if a.command=='export' else solve)(a.first,a.second,a.output)

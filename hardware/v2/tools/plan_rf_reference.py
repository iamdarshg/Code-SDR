"""Plan In2 ground corridors and through stitches from an exported board snapshot.

Geometry only: does not load or save a PCB. The result still requires KiCad zone
import/refill, DRC, ground/power connectivity checks, and transmission-line review.
Run using system Python with Shapely/scipy/numpy/Pillow (recovery_route imports).
"""
from __future__ import annotations
import argparse
import ast
import hashlib
import json
import math
from collections import Counter
from pathlib import Path
from shapely import union_all
from shapely.geometry import LineString, Point, Polygon, box
from recovery_route import Copper, polygons
from sensitive_route_contract import PAIR_GROUPS


def rf_names():
    tree = ast.parse(Path(__file__).with_name('export_rf50_dsn.py').read_text())
    assignment = next(n for n in tree.body if isinstance(n, ast.Assign)
                      and any(isinstance(t, ast.Name) and t.id == 'RF50_NETS' for t in n.targets))
    selected = set(ast.literal_eval(assignment.value)) | {'HF_BYPASS'}
    for pair in PAIR_GROUPS.values():
        for net in pair.positive + pair.negative:
            if net.startswith(('IF', 'LO', 'LMX_LO', 'ADF_LO', 'LT_IN', 'AD8351', 'ADC_VIN')):
                selected.add(net)
    return selected


def as_polygons(shape):
    if shape.is_empty:
        return []
    if isinstance(shape, Polygon):
        return [shape]
    return [p for p in shape.geoms if isinstance(p, Polygon)]


def rings(shape):
    return [[[[round(x, 6), round(y, 6)] for x, y in ring.coords]
             for ring in [p.exterior, *p.interiors]] for p in as_polygons(shape)]


def build_plan(data, margin=1., pitch=1., diameter=.45, drill=.2,
               board_bounds=(0., 0., 99., 99.), edge_clearance=.25,
               plane_clearance=.18, additional_nets=()):
    if data['layers'] != [0, 4, 6, 2]:
        raise ValueError('This planner requires KiCad 9 four-layer order [0,4,6,2].')
    if not (margin > diameter + .1 and 0 < pitch <= 1. and 0 < drill < diameter):
        raise ValueError('Need margin > via diameter + .1 mm, pitch in (0,1] and drill < diameter.')
    selected = rf_names() | set(additional_nets)
    tracks = [i for i in data['items'] if i['kind'] == 'track'
              and i['net'] in selected and i['layers'] == [2]]
    inner_tracks = [i for i in data['items'] if i['kind'] == 'track'
                    and i['net'] in selected and any(l in (4,6) for l in i['layers'])]
    clip = box(*board_bounds).buffer(-edge_clearance)
    if not box(*data['bounds']).covers(clip):
        raise ValueError('Explicit board bounds exceed exported board bounding box.')
    lines = [LineString([t['start'], t['end']]) for t in tracks]
    nominal = union_all([line.buffer(t['width']/2 + margin, join_style=2)
                         for t, line in zip(tracks, lines)]).intersection(clip)
    copper = Copper(data)
    # Fixed copper and holes cause refill voids even inside a priority ground zone.
    obstacles = [shape.buffer(plane_clearance) for uid, shape in copper.by_layer[6]
                 if copper.nodes[uid]['kind'] != 'zone' and copper.nodes[uid]['net'] != 'GND']
    # NPTH drills have no copper shape but still impose a copper clearance.
    obstacles += [Point(i['pos']).buffer(max(i['drill'])/2 + plane_clearance)
                  for i in data['items'] if i['kind']=='pad' and max(i['drill'])>0 and not i['net']]
    target = nominal.difference(union_all(obstacles)).buffer(0)
    ground = union_all([p for z in data['zones'] if z['net']=='GND' and z['layer']==4
                        for p in polygons(z['polygons'])])
    if tracks and ground.is_empty:
        raise ValueError('No filled In1 ground plane in snapshot.')
    existing = []
    for item in data['items']:
        if item['kind'] != 'via' or item['net'] != 'GND' or not {4,6}.issubset(item['layers']):
            continue
        disk = Point(item['start']).buffer(item['width']/2)
        # Filled plane boundaries may trim around the drill; require positive
        # copper overlap, not coverage of the entire drill-centred disk.
        if target.intersection(disk).area > .01 and ground.intersection(disk).area > .01:
            existing.append({'pos':item['start'],'diameter':item['width'],'drill':item['drill'],'uuid':item['uuid']})
    proposed, stations, rejected = [], [], Counter()
    def valid(xy):
        disk = Point(xy).buffer(diameter/2 + .02)
        if not target.covers(disk):
            return 'outside_predicted_corridor'
        if not ground.covers(disk):
            return 'missing_In1_ground'
        if not copper.via_clear(xy,'GND',diameter,drill):
            return 'copper_pad_or_hole_collision'
        # via_clear uses the generic .115-mm clearance; preserve the .18-mm
        # RF-to-ground clearance here as a conservative all-layer requirement.
        via_disk = Point(xy).buffer(diameter/2)
        if any(not copper.copper_clear(via_disk,l,'GND',plane_clearance) for l in data['layers']):
            return 'RF_clearance'
        return None
    def nearest(xy, candidates):
        return min((math.dist(xy,v['pos']) for v in candidates),default=None)
    def add(xy, reason):
        via={'pos':[round(x,6) for x in xy],'diameter':diameter,'drill':drill,'reason':reason}
        proposed.append(via)
        copper.routes.append({'net':'GND','segments':[],'vias':[via]})
        return via
    # Prioritize the return connection at actual RF layer transitions, including
    # orphaned vias whose bottom segment was removed during package integration.
    transitions=[i for i in data['items'] if i['kind']=='via' and i['net'] in selected and 2 in i['layers']]
    for tr in transitions:
        if (nearest(tr['start'],existing+proposed) or math.inf) <= 1.:
            continue
        for radius in (.6,.75,.9):
            found=False
            for deg in range(0,360,30):
                xy=[tr['start'][0]+radius*math.cos(math.radians(deg)),tr['start'][1]+radius*math.sin(math.radians(deg))]
                if valid(xy) is None:
                    add(xy,'RF transition: '+tr['net']); found=True; break
            if found: break
    # Both sides, station separation <= pitch before collision filtering.
    # Coverage records explicitly retain every missing station after filtering.
    for t,line in zip(tracks,lines):
        if line.length < 1e-7: continue
        dx=(t['end'][0]-t['start'][0])/line.length;dy=(t['end'][1]-t['start'][1])/line.length
        n=max(1,math.ceil(line.length/pitch))
        for step in range(n+1):
            center=line.interpolate(step/n,normalized=True)
            for side in (-1,1):
                ideal=[center.x-side*dy*(t['width']/2+.55),center.y+side*dx*(t['width']/2+.55)]
                record={'net':t['net'],'track_uuid':t['uuid'],'center':[center.x,center.y],'side':side,'ideal':ideal}
                near=nearest(ideal,existing+proposed)
                if near is not None and near <= pitch/2:
                    record['status']='covered_existing_or_planned';stations.append(record);continue
                placed=None
                # Keep candidates on the nominated side, including several
                # tangent shifts; never silently accept an obstructed via.
                for tangent in (0.,-.2,.2,-.4,.4):
                    for offset in (.55,.7,.85):
                        off=t['width']/2+offset
                        xy=[center.x+dx*tangent-side*dy*off,center.y+dy*tangent+side*dx*off]
                        why=valid(xy)
                        if why: rejected[why]+=1;continue
                        near=nearest(xy,existing+proposed)
                        if near is not None and near < diameter+.05: continue
                        placed=add(xy,'RF/analog fence: '+t['net']);break
                    if placed:break
                record['status']='proposed' if placed else 'uncovered'
                if placed:record['pos']=placed['pos']
                stations.append(record)
    # Recalculate final station coverage: candidate displacement can leave a gap.
    uncovered=[]
    for record in stations:
        distance=nearest(record['ideal'],existing+proposed)
        record['nearest_ground_mm']=distance
        if distance is None or distance>pitch/2+.001:uncovered.append(record)
    transition_report=[{'net':t['net'],'pos':t['start'],
                        'via_layers':t['layers'],
                        'touching_track_layers':sorted({layer for item in data['items']
                            if item['kind']=='track' and item['net']==t['net']
                            and LineString([item['start'],item['end']]).distance(Point(t['start'])) <= (item['width']+t['width'])/2+.000001
                            for layer in item['layers']}),
                        'nearest_existing_ground_mm':nearest(t['start'],existing),
                        'nearest_after_plan_ground_mm':nearest(t['start'],existing+proposed)} for t in transitions]
    power=[]
    for z in data['zones']:
        if z['layer']!=6 or z['net']=='GND':continue
        shape=union_all(polygons(z['polygons']));remaining=shape.difference(nominal.buffer(plane_clearance))
        parts=as_polygons(remaining)
        power.append({'net':z['net'],'before_mm2':shape.area,'removed_mm2':shape.area-remaining.area,
                      'removed_percent':100*(shape.area-remaining.area)/shape.area if shape.area else 0,
                      'remaining_polygon_areas_mm2':sorted((p.area for p in parts),reverse=True)})
    summary={'bottom_tracks':len(tracks),'bottom_nets':len({t['net'] for t in tracks}),
             'bottom_length_mm':sum(l.length for l in lines),'nominal_zone_mm2':nominal.area,
             'predicted_ground_mm2':target.area,'existing_ground_vias_in_corridor':len(existing),
             'proposed_ground_vias':len(proposed),'fence_stations':len(stations),'uncovered_stations':len(uncovered),
             'sensitive_tracks_on_inner_reference_layers':len(inner_tracks),
             'uncovered_transitions':sum(t['nearest_after_plan_ground_mm'] is None or t['nearest_after_plan_ground_mm']>1 for t in transition_report)}
    return {'schema':'CodeSDR.rf-reference-plan.v1','source_board_sha256':data['sha256'],
            'parameters':{'margin_mm':margin,'pitch_mm':pitch,'via_diameter_mm':diameter,'via_drill_mm':drill,
                          'board_bounds':board_bounds,'edge_clearance_mm':edge_clearance,'plane_clearance_mm':plane_clearance},
            'summary':summary,'selected_nets':sorted(selected),'zones':[{'net':'GND','layer':6,'priority':10,'polygons':rings(nominal)}],
            'predicted_filled_polygons':rings(target),'routes':[{'net':'GND','reason':'RF ground corridor stitches','segments':[],
                         'vias':[{k:v for k,v in p.items() if k!='reason'} for p in proposed]}],
            'proposed_stitches':proposed,'existing_stitches':existing,'fence_coverage':stations,
            'uncovered_stations':uncovered,'transition_coverage':transition_report,'power_plane_impact_approximate':power,
            'sensitive_inner_tracks_requiring_review':inner_tracks,
            'candidate_rejections':dict(rejected),'limits':['No PCB mutations or KiCad refill performed.',
             'Priority-zone holes and power/ground connectivity require final KiCad verification.',
             'Collision-filtered fences can have gaps; uncovered entries are not waived.',
             'Geometric via spacing does not establish controlled impedance or 10 GHz RF performance.']}


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('geometry',type=Path);parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--margin',type=float,default=1.);parser.add_argument('--pitch',type=float,default=1.)
    parser.add_argument('--diameter',type=float,default=.45);parser.add_argument('--drill',type=float,default=.2)
    parser.add_argument('--board-bounds',nargs=4,type=float,default=[0.,0.,99.,99.])
    parser.add_argument('--add-net',action='append',default=[])
    args=parser.parse_args();raw=args.geometry.read_bytes();data=json.loads(raw)
    result=build_plan(data,args.margin,args.pitch,args.diameter,args.drill,args.board_bounds,additional_nets=args.add_net)
    result['geometry_sha256']=hashlib.sha256(raw).hexdigest();result['geometry_source']=str(args.geometry.resolve())
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(result,indent=2))
    print(json.dumps(result['summary'],indent=2));print('Plan:',args.output)


if __name__=='__main__':main()

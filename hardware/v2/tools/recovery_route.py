"""Plan additive, geometry-checked routes without changing existing copper.

This is a planning aid, not a DRC replacement. Import with recovery_board_io.py,
refill zones, and compare fresh whole-board DRC before promoting a candidate.
"""
from __future__ import annotations

import argparse
from collections import Counter, defaultdict
import json
import math
import heapq
from pathlib import Path

import numpy as np
from shapely import STRtree, union_all
from shapely.geometry import LineString, Point, Polygon
from shapely.ops import nearest_points
from PIL import Image, ImageDraw
from scipy.ndimage import distance_transform_edt


def polygons(data):
    return [Polygon(rings[0], rings[1:]).buffer(0) for rings in data if len(rings[0]) >= 3]


class Copper:
    def __init__(self, data):
        self.data = data
        self.layers = data['layers']
        self.outer = [self.layers[0], self.layers[-1]]
        self.nodes = {}
        self.by_layer = defaultdict(list)
        self.pad_shapes = []
        self.pad_nets = []
        self.holes = []
        self.routes = []
        self.bounds = Polygon([(data['bounds'][0], data['bounds'][1]),
                               (data['bounds'][2], data['bounds'][1]),
                               (data['bounds'][2], data['bounds'][3]),
                               (data['bounds'][0], data['bounds'][3])]).buffer(-.25)
        for item in data['items']:
            shape_by_layer = {}
            if item['kind'] == 'pad':
                shape_by_layer = {int(l): union_all(polygons(poly)) for l, poly in item['shapes'].items()}
                if shape_by_layer:
                    self.pad_shapes.append(union_all(list(shape_by_layer.values())))
                    self.pad_nets.append(item['net'])
                dx, dy = item['drill']
                if max(dx, dy) > 0:
                    # Conservative circle encloses an oval drill.
                    self.holes.append(Point(item['pos']).buffer(max(dx, dy) / 2, quad_segs=16))
            else:
                if item['kind'] == 'via':
                    shape = Point(item['start']).buffer(item['width'] / 2, quad_segs=16)
                    self.holes.append(Point(item['start']).buffer(item['drill'] / 2, quad_segs=16))
                    # A through via spans the entire stack: its barrel blocks
                    # every layer, not just start/end.  Without this, inner
                    # layer routing happily crosses F-B vias and DRC shorts.
                    shape_by_layer = {l: shape for l in self.layers}
                else:
                    shape = LineString([item['start'], item['end']]).buffer(item['width'] / 2, quad_segs=8)
                    shape_by_layer = {l: shape for l in item['layers']}
            self.add_node(item['uuid'], item['net'], item['kind'], shape_by_layer, item)
        for zone in data['zones']:
            for i, poly in enumerate(polygons(zone['polygons'])):
                # Disjoint zone polygons are separate conductors, even when
                # they have one KiCad zone UUID.
                self.add_node(f"{zone['uuid']}:{i}", zone['net'], 'zone', {zone['layer']: poly}, zone)
        self.refresh_trees()

    def add_node(self, uid, net, kind, shapes, item):
        self.nodes[uid] = dict(net=net, kind=kind, shapes=shapes, item=item)
        for layer, shape in shapes.items():
            if not shape.is_empty:
                self.by_layer[layer].append((uid, shape))

    def refresh_trees(self):
        self.trees = {l: STRtree([s for _, s in entries]) for l, entries in self.by_layer.items()}
        self.pad_tree = STRtree(self.pad_shapes)
        self.hole_tree = STRtree(self.holes)

    def groups(self, net):
        ids = [u for u, n in self.nodes.items() if n['net'] == net]
        parent = {u: u for u in ids}
        def root(u):
            while parent[u] != u:
                parent[u] = parent[parent[u]]
                u = parent[u]
            return u
        for layer, entries in self.by_layer.items():
            selected = [(u, s) for u, s in entries if u in parent]
            if not selected:
                continue
            shapes = [s for _, s in selected]
            tree = STRtree(shapes)
            for i, (uid, shape) in enumerate(selected):
                for j in tree.query(shape, predicate='dwithin', distance=.0005):
                    if j <= i:
                        continue
                    a, b = root(uid), root(selected[j][0])
                    parent[a] = b
        groups = defaultdict(list)
        for uid in ids:
            groups[root(uid)].append(uid)
        return list(groups.values())

    def copper_clear(self, shape, layer, net, clearance=.115):
        if not self.bounds.covers(shape):
            return False
        # Zone copper is refillable; fixed pads/tracks/vias are obstacles.
        query = shape.buffer(clearance)
        for i in self.trees[layer].query(query, predicate='intersects'):
            uid, other = self.by_layer[layer][i]
            node = self.nodes[uid]
            if node['kind'] == 'zone' or node['net'] == net:
                continue
            if shape.distance(other) < clearance - 1e-6:
                return False
        # Newly planned copper is not yet in the static tree.
        for route in self.routes:
            if route['net'] == net:
                continue
            for seg in route['segments']:
                if seg['layer'] == layer and shape.distance(LineString([seg['start'], seg['end']])) < clearance + seg['width']/2:
                    return False
            for via in route['vias']:
                if shape.distance(Point(via['pos'])) < clearance + via['diameter']/2:
                    return False
        return True

    def via_clear(self, xy, net, diameter=.45, drill=.2):
        shape = Point(xy).buffer(diameter/2, quad_segs=16)
        # Copper covered by a same-net pad creates no new clearance need;
        # exclude it before checking (enables standard via-in-pad escapes).
        for h in self.pad_tree.query(shape.buffer(1e-9), predicate='intersects'):
            if self.pad_nets[h] == net:
                shape = shape.difference(self.pad_shapes[h])
        if any(not self.copper_clear(shape, l, net) for l in self.layers):
            return False
        if shape.is_empty:
            return False
        hole = Point(xy).buffer(drill/2 + .115, quad_segs=16)
        if len(self.hole_tree.query(hole, predicate='intersects')):
            return False
        for route in self.routes:
            for via in route['vias']:
                if math.dist(xy, via['pos']) < .115 + (drill+via['drill'])/2:
                    return False
        return True

    def line_clear(self, a, b, layer, net, width):
        return self.copper_clear(LineString([a,b]).buffer(width/2, quad_segs=8), layer, net)

    def plane_routes(self, nets):
        summary = {}
        for net in nets:
            groups = self.groups(net)
            planes = [n for n in self.nodes.values() if n['net'] == net and n['kind'] == 'zone'
                      and next(iter(n['shapes'])) not in self.outer]
            if not planes:
                continue
            main_plane = max(planes, key=lambda n: next(iter(n['shapes'].values())).area)
            plane_layer, plane_shape = next(iter(main_plane['shapes'].items()))
            plane_interior = plane_shape.buffer(-.08)
            plane_uid = next(u for u, n in self.nodes.items() if n is main_plane)
            missing = [g for g in groups if plane_uid not in g]
            accepted = 0
            skipped = []
            for group in sorted(missing, key=lambda g: -sum(self.nodes[u]['kind']=='pad' for u in g)):
                anchors = []
                for uid in group:
                    node = self.nodes[uid]
                    for layer, shape in node['shapes'].items():
                        if layer not in self.outer:
                            continue
                        if node['kind'] == 'pad':
                            pts = [node['item']['pos']]
                        elif node['kind'] in ('track', 'via'):
                            pts = [node['item']['start'], node['item']['end']]
                        else:
                            pts = [list(shape.representative_point().coords)[0]]
                        anchors.extend((xy, layer, shape) for xy in pts)
                found = None
                width = .25 if net == 'GND' else .30
                # Small offsets first; try existing copper and every land in
                # the disconnected conductor before accepting a long stub.
                for radius in (0, .4, .55, .7, .9, 1.2, 1.6, 2.0, 3.0):
                    if found:
                        break
                    for xy, layer, shape in anchors:
                        if found:
                            break
                        for angle in range(0, 360, 30) if radius else (0,):
                            pos = [round(xy[0]+radius*math.cos(math.radians(angle)),5),
                                   round(xy[1]+radius*math.sin(math.radians(angle)),5)]
                            if not plane_interior.covers(Point(pos)) or not self.via_clear(pos, net):
                                continue
                            # Connect at the nearest piece of existing copper,
                            # with a small overlap so rounding cannot leave a gap.
                            nearest = nearest_points(Point(pos), shape)[1]
                            end = list(nearest.coords)[0]
                            center = shape.representative_point()
                            if math.dist(pos,end) > .01:
                                v = np.array(center.coords[0])-np.array(end)
                                norm = np.linalg.norm(v)
                                if norm:
                                    end = (np.array(end)+v/norm*.025).tolist()
                            if not self.line_clear(end,pos,layer,net,width):
                                continue
                            found = dict(net=net, reason='connect isolated conductor to existing inner plane',
                                from_group=group, segments=[] if math.dist(pos,end)<.001 else
                                [dict(start=list(end),end=pos,layer=layer,width=width)],
                                vias=[dict(pos=pos,diameter=.45,drill=.2)])
                            break
                if found:
                    self.routes.append(found)
                    accepted += 1
                else:
                    skipped.append([self.nodes[u]['item'].get('ref',u) for u in group][:8])
            summary[net] = dict(groups=len(groups), added_connections=accepted, unresolved_groups=skipped)
            print(net, len(groups), 'groups;',accepted,'plane connections', flush=True)
        return summary

    def group_shapes(self, group):
        return {l: union_all([self.nodes[u]['shapes'][l] for u in group if l in self.nodes[u]['shapes']])
                for l in self.layers}

    def maze_route(self, net, source_group, target_group, width=.15, margin=5., pitch=.08,
                   max_pops=1200000, clearance=None):
        source = self.group_shapes(source_group)
        target = self.group_shapes(target_group)
        source_all = union_all([s for s in source.values() if not s.is_empty])
        target_all = union_all([s for s in target.values() if not s.is_empty])
        a,b = nearest_points(source_all,target_all)
        bounds = [max(self.data['bounds'][0]+.3,min(a.x,b.x)-margin),
                  max(self.data['bounds'][1]+.3,min(a.y,b.y)-margin),
                  min(self.data['bounds'][2]-.3,max(a.x,b.x)+margin),
                  min(self.data['bounds'][3]-.3,max(a.y,b.y)+margin)]
        x0,y0,x1,y1=bounds
        nx,ny=int((x1-x0)/pitch)+1,int((y1-y0)/pitch)+1
        if nx*ny>2000000:
            return None
        clip=Polygon([(x0,y0),(x1,y0),(x1,y1),(x0,y1)])
        def draw_geom(draw,geom,fill=1):
            if geom.is_empty:
                return
            if geom.geom_type=='Polygon':
                # Obstacles are rasterized in conservative buffered form.
                def coords(ring):
                    return [((x-x0)/pitch,(y-y0)/pitch) for x,y in ring.coords]
                draw.polygon(coords(geom.exterior),fill=fill)
                for ring in geom.interiors:
                    draw.polygon(coords(ring),fill=0)
            elif hasattr(geom,'geoms'):
                for g in geom.geoms:
                    draw_geom(draw,g,fill)
        def mask(geoms):
            im=Image.new('1',(nx,ny),0);draw=ImageDraw.Draw(im)
            for g in geoms:
                if not g.is_empty and g.intersects(clip):
                    # Draw each polygon separately and OR to avoid an object's
                    # holes erasing a different object's obstacle.
                    tmp=Image.new('1',(nx,ny),0)
                    draw_geom(ImageDraw.Draw(tmp),g.intersection(clip))
                    from PIL import ImageChops
                    im=ImageChops.logical_or(im,tmp)
            return np.array(im,dtype=bool)
        nearby={l:[(u,s) for u,s in entries if s.intersects(clip)] for l,entries in self.by_layer.items()}
        def foreign(l):
            return [s for u,s in nearby[l] if self.nodes[u]['net']!=net and self.nodes[u]['kind']!='zone']
        clearance=.115+pitch*.55 if clearance is None else clearance
        # Route on every copper layer. Inner planes remain refillable zones, so
        # a localized signal track creates a clearance channel without treating
        # the entire pour as a fixed wall. Fixed copper and through-vias still
        # block every layer and the final candidate must pass whole-board DRC.
        routing_layers=self.layers
        blocked=[]
        for l in routing_layers:
            blocked.append(mask([s.buffer(width/2+clearance) for s in foreign(l)]))
        # Via masks cover copper on EVERY layer and drill-to-drill spacing.
        vg=[]
        for l in self.layers:
            vg.extend(s.buffer(.225+clearance) for s in foreign(l))
        vg.extend(s.buffer(.285+pitch*.55) for s in self.pad_shapes if s.intersects(clip))
        vg.extend(s.buffer(.215+pitch*.55) for s in self.holes if s.intersects(clip))
        via_blocked=mask(vg)
        starts=[mask([source[l].buffer(-.015)]) & ~blocked[i] for i,l in enumerate(routing_layers)]
        goals_real=[mask([target[l].buffer(-.015)]) & ~blocked[i] for i,l in enumerate(routing_layers)]
        goals=goals_real
        if not any(s.any() for s in starts) or not any(g.any() for g in goals):
            return None
        goal_union=np.logical_or.reduce(goals)
        heuristic=distance_transform_edt(~goal_union).astype(np.float32)
        size=nx*ny
        layer_count=len(routing_layers)
        dist=np.full(layer_count*size,np.inf,dtype=np.float32)
        prev=np.full(layer_count*size,-1,dtype=np.int32)
        queue=[]
        for l,s in enumerate(starts):
            for j,i in zip(*np.nonzero(s)):
                k=l*size+j*nx+i;dist[k]=0
                heapq.heappush(queue,(float(heuristic[j,i]),0.,k))
        directions=((1,0,1.),(-1,0,1.),(0,1,1.),(0,-1,1.),(1,1,1.414214),(-1,-1,1.414214),(1,-1,1.414214),(-1,1,1.414214))
        found=None;pops=0
        while queue and pops<max_pops:
            _,cost,k=heapq.heappop(queue)
            if cost>dist[k]+1e-3:
                continue
            pops+=1
            l,cell=divmod(k,size);j,i=divmod(cell,nx)
            if goals[l][j,i]:
                found=k;break
            for dx,dy,dc in directions:
                ii,jj=i+dx,j+dy
                if not(0<=ii<nx and 0<=jj<ny) or blocked[l][jj,ii]:
                    continue
                if dx and dy and (blocked[l][j,ii] or blocked[l][jj,i]):
                    continue
                kk=l*size+jj*nx+ii;new=cost+dc
                if new<dist[kk]-1e-3:
                    dist[kk]=new;prev[kk]=k
                    heapq.heappush(queue,(new+heuristic[jj,ii],new,kk))
            if not via_blocked[j,i]:
                for ll in range(layer_count):
                    if ll==l or blocked[ll][j,i]:
                        continue
                    kk=ll*size+cell
                    new=cost+(2.5+0.35*abs(ll-l))/pitch
                    if new<dist[kk]-1e-3:
                        dist[kk]=new;prev[kk]=k
                        heapq.heappush(queue,(new+heuristic[j,i],new,kk))
        if found is None:
            return None
        path=[];k=found
        while k>=0:
            l,cell=divmod(k,size);j,i=divmod(cell,nx)
            path.append((round(x0+i*pitch,6),round(y0+j*pitch,6),l))
            k=int(prev[k])
        path.reverse()
        compact=[path[0]]
        for i in range(1,len(path)-1):
            p0,p1,p2=compact[-1],path[i],path[i+1]
            if p0[2]==p1[2]==p2[2] and abs((p1[0]-p0[0])*(p2[1]-p1[1])-(p1[1]-p0[1])*(p2[0]-p1[0]))<1e-7:
                continue
            compact.append(p1)
        compact.append(path[-1])
        route=dict(net=net,reason='all-layer maze route between disconnected conductors',segments=[],vias=[])
        # Raster masks select cells, not exact copper points. Anchor the route
        # inside the real source/target polygon; otherwise a rounded cell at a
        # pad corner can leave a microscopic open and repeated empty routes.
        def anchor(xy, layer, shape, at_start):
            interior=shape.buffer(-.025)
            if interior.is_empty: interior=shape
            exact=list(nearest_points(Point(xy),interior)[1].coords)[0]
            if math.dist(xy,exact) <= .0005: return True
            if not self.line_clear(exact,xy,layer,net,width): return False
            route['segments'].append(dict(start=list(exact if at_start else xy),
                end=list(xy if at_start else exact),layer=layer,width=width))
            return True
        first=path[0]
        if not anchor(first[:2],routing_layers[first[2]],source[routing_layers[first[2]]],True): return None
        for a,b in zip(compact,compact[1:]):
            if a[2]!=b[2]:
                if not self.via_clear(a[:2],net):return None
                route['vias'].append(dict(pos=list(a[:2]),diameter=.45,drill=.2))
            elif math.dist(a[:2],b[:2])>.001:
                layer=routing_layers[a[2]]
                if not self.line_clear(a[:2],b[:2],layer,net,width):return None
                route['segments'].append(dict(start=list(a[:2]),end=list(b[:2]),layer=layer,width=width))
        l,cell=divmod(found,size);j,i=divmod(cell,nx)
        if not anchor(path[-1][:2],routing_layers[l],target[routing_layers[l]],False):
            return None
        if not route['segments'] and not route['vias']: return None
        return route

    def accept_planned(self,route):
        self.routes.append(route)
        seq=len(self.routes)
        for i,seg in enumerate(route['segments']):
            shape=LineString([seg['start'],seg['end']]).buffer(seg['width']/2,quad_segs=8)
            self.add_node(f'planned:{seq}:s{i}',route['net'],'track',{seg['layer']:shape},seg)
        for i,via in enumerate(route['vias']):
            shape=Point(via['pos']).buffer(via['diameter']/2,quad_segs=16)
            self.add_node(f'planned:{seq}:v{i}',route['net'],'via',{l:shape for l in self.layers},dict(start=via['pos'],end=via['pos']))
            self.holes.append(Point(via['pos']).buffer(via['drill']/2,quad_segs=16))
        self.refresh_trees()

    def maze_nets(self,nets,width,checkpoint,pair_limit=10):
        summary={}
        for net in nets:
            count=0
            for attempt in range(120):
                groups=self.groups(net)
                if len(groups)<2:break
                shapes=[union_all(list(self.group_shapes(g).values())) for g in groups]
                pairs=sorted((shapes[i].distance(shapes[j]),i,j) for i in range(len(groups)) for j in range(i))
                found=None
                for _,i,j in pairs[:pair_limit]:
                    # Route from the smaller group into the larger group, so
                    # existing copper provides useful destination anchors.
                    if shapes[i].area>shapes[j].area:i,j=j,i
                    for margin,pitch in ((4.,.08),(8.,.06),(16.,.10)):
                        found=self.maze_route(net,groups[i],groups[j],width,margin,pitch)
                        if found:break
                    if found:break
                if not found:break
                self.accept_planned(found)
                remaining=len(self.groups(net))
                if remaining>=len(groups):
                    raise RuntimeError(f'{net}: proposed route failed to merge exact copper groups')
                count+=1
                checkpoint()
                print(net,'connected',count,'remaining geometric groups',len(self.groups(net)),flush=True)
            summary[net]=dict(added_connections=count,remaining_groups=len(self.groups(net)))
            print(net,'finished',summary[net],flush=True)
        return summary


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('geometry',type=Path)
    ap.add_argument('output',type=Path)
    ap.add_argument('--nets',nargs='+',default=['GND','+3V3_ANA','+3V3_DIG'])
    ap.add_argument('--maze',action='store_true')
    ap.add_argument('--width',type=float,default=.15)
    ap.add_argument('--pair-limit',type=int,default=10)
    args = ap.parse_args()
    data = json.loads(args.geometry.read_text(encoding='utf-8'))
    copper = Copper(data)
    def checkpoint():
        args.output.write_text(json.dumps(dict(source_sha256=data['sha256'],routes=copper.routes),indent=2),encoding='utf-8')
    summary = copper.maze_nets(args.nets,args.width,checkpoint,args.pair_limit) if args.maze else copper.plane_routes(args.nets)
    args.output.write_text(json.dumps(dict(source_sha256=data['sha256'],routes=copper.routes,summary=summary),indent=2),encoding='utf-8')
    print('saved',len(copper.routes),'routes',args.output,flush=True)


if __name__ == '__main__':
    main()

"""Render exported copper and pad geometry for a local layout inspection."""
import argparse
import json
from collections import defaultdict
from pathlib import Path

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon as PatchPolygon

from recovery_route import polygons


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('geometry', type=Path)
    ap.add_argument('output', type=Path)
    ap.add_argument('--bounds', nargs=4, type=float)
    args = ap.parse_args()
    d = json.loads(args.geometry.read_text())
    bounds = args.bounds or d['bounds']
    fig, ax = plt.subplots(figsize=(14, 10), dpi=150)
    refs = defaultdict(list)
    colors = {d['layers'][0]:'#ae3030', d['layers'][-1]:'#3460b0'}
    for item in d['items']:
        if item['kind'] == 'pad':
            x,y = item['pos']
            if bounds[0] <= x <= bounds[2] and bounds[1] <= y <= bounds[3]:
                refs[item['ref']].append((x,y))
                for poly in polygons(item['shapes'].get(str(d['layers'][0]), [])):
                    ax.add_patch(PatchPolygon(list(poly.exterior.coords), facecolor='#d2ac44', edgecolor='#403000', linewidth=.4))
                if item['number']:
                    ax.text(x,y,item['number'],ha='center',va='center',fontsize=5,color='black')
        elif item['kind'] == 'via':
            x,y=item['start']
            ax.add_patch(plt.Circle((x,y),item['width']/2,color='#676767'))
            ax.add_patch(plt.Circle((x,y),item['drill']/2,color='white'))
        else:
            x0,y0=item['start'];x1,y1=item['end']
            layer=item['layers'][0]
            if layer not in colors:continue
            # Polygons preserve the actual physical width at any zoom.
            from shapely.geometry import LineString
            poly=LineString([(x0,y0),(x1,y1)]).buffer(item['width']/2,quad_segs=4)
            ax.add_patch(PatchPolygon(list(poly.exterior.coords), facecolor=colors[layer], edgecolor='none', alpha=.7))
    for ref,ps in refs.items():
        x=sum(x for x,y in ps)/len(ps); y=sum(y for x,y in ps)/len(ps)
        ax.text(x,y-.65,ref,ha='center',fontsize=7,fontweight='bold',color='#202020',bbox=dict(facecolor='white',alpha=.7,edgecolor='none',pad=.1))
    ax.set_xlim(bounds[0],bounds[2]);ax.set_ylim(bounds[3],bounds[1]);ax.set_aspect('equal')
    ax.set_xlabel('mm');ax.set_ylabel('mm');ax.grid(alpha=.15)
    ax.set_title('Actual copper: front red, back blue, pads gold; pours omitted for clarity')
    fig.savefig(args.output,bbox_inches='tight');plt.close(fig)


if __name__=='__main__':main()

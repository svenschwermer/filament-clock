import pcbnew
from pcbnew import wxPoint as P
from math import cos, sin, pi

# exec(open("/home/sven/filament-clock/separator/place.py").read())

# Parameters
SCALE = 1000000
l = 42 * SCALE # segment length
edge_margin = 5 * SCALE
angle_deg = 10
hole_margin = 8 * SCALE

def line(start, end, layer, width=0.05):
    l = pcbnew.DRAWSEGMENT()
    l.SetShape(pcbnew.S_SEGMENT)
    l.SetStart(start)
    l.SetEnd(end)
    l.SetLayer(layer)
    l.SetWidth(int(width * SCALE))
    return l

def arc(center, start, angle, layer, width=0.05):
    a = pcbnew.DRAWSEGMENT()
    a.SetShape(pcbnew.S_ARC)
    a.SetCenter(center)
    a.SetArcStart(start)
    a.SetAngle(angle * 10)
    a.SetLayer(layer)
    a.SetWidth(int(width * SCALE))
    return a

# Derived Parameters
angle_rad = angle_deg / 360 * 2*pi
angle_sin = sin(angle_rad)
angle_cos = cos(angle_rad)

b = pcbnew.GetBoard()
d = b.FindModuleByReference('D1')

d.SetOrientationDegrees(0)
d.SetPosition(P(0, 0))

# Board Outline
edge = b.GetLayerID('Edge.Cuts')

# Remove existing outline
for drawing in b.GetDrawings():
    if drawing.GetLayer() == edge:
        b.Remove(drawing)

arc = [
    arc(P(l/angle_cos,0), P(l/angle_cos,-edge_margin), 180, edge),
    arc(P(-l/angle_cos,0), P(-l/angle_cos,edge_margin), 180, edge),
]
for a in arc:
    b.Add(a)

b.Add(line(arc[0].GetArcStart(), arc[1].GetArcEnd(), edge))
b.Add(line(arc[1].GetArcStart(), arc[0].GetArcEnd(), edge))

# Mounting holes
h = [b.FindModuleByReference(f'H{i}') for i in [1,2]]
h[0].SetPosition(P((l - hole_margin) / angle_cos, 0))
h[1].SetPosition(P(-(l - hole_margin) / angle_cos, 0))

j1 = b.FindModuleByReference('J1')
j1.SetPosition(P(26.5 * SCALE / angle_cos, 0))

pcbnew.Refresh()

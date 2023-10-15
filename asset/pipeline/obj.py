import logging

log = logging.Logger("obj")
log.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
log.addHandler(ch)


def get_vert_index(e):
    p = e.split("/")
    return int(p[0]) - 1

def face_from_line(parts):
    if len(parts) == 3:
        a = get_vert_index(parts[0])
        b = get_vert_index(parts[1])
        c = get_vert_index(parts[2])
        return [Face(a,b,c)]
    elif len(parts) == 4:
        a = get_vert_index(parts[0])
        b = get_vert_index(parts[1])
        c = get_vert_index(parts[2])
        d = get_vert_index(parts[3])
        return [Face(a,b,c), Face(d,b,c)]
    else:
        raise Exception(f"face must be tri or quad, got {len(parts)}")
        

class Face(object):
    def __init__(self, a, b, c):
        self.a = a
        self.b = b
        self.c = c

def vert_from_line(parts):
    if len(parts) != 3:
        raise Exception("Vertices must be 3D")
    x = float(parts[0])
    y = float(parts[1])
    z = float(parts[2])
    return Vert(x,y,z)

class Vert(object):
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z
    def __str__(self):
        return f"({self.x}, {self.y}, {self.z})"
    def __repr__(self):
        return self.__str__()

def test_bounds(l, i):
    if i < 0 or i >= len(l):
        raise Exception(f"index {i} out of bounds [0, {len(l)}]")
        

class ParsedObj(object):
    def __init__(self):
        self._v = []
        self._f = []
    def add_vert(self, v):
        self._v.append(v)
    def add_faces(self, f):
        self._f += f
    def faces(self):
        return self._f.__iter__()
    def face_coords(self):
        for f in self.faces():
            test_bounds(self._v, f.a)
            test_bounds(self._v, f.b)
            test_bounds(self._v, f.c)
            yield [self._v[f.a], self._v[f.b], self._v[f.c]]

    
def parse(f):
    res = ParsedObj()
    for line in f:
        parts = line.split(" ")
        if parts[0] == "v":
            res.add_vert( vert_from_line(parts[1:]) )
        elif parts[0] == "f":
            res.add_faces( face_from_line(parts[1:]) )
        else:
            log.info(f"unsupported line type: {parts[0]}")
    return res



import sys
import struct

VERTEX_SIZE = 8*4

with open(sys.argv[1], "rb") as f:
    content = f.read()
vertex_count = len(content) / VERTEX_SIZE

for i in range(int(vertex_count)):
    col = struct.unpack_from("<iiii", content, offset=i*VERTEX_SIZE)
    pos = struct.unpack_from("<ffff", content, offset=i*VERTEX_SIZE + 4*4)
    print(f"v {pos[0]}, {pos[1]}, {pos[2]} [{col}]")
print()

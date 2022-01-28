from tga import *
import sys
import argparse

ZERO=0xff000000

class Rect:
    def __init__(self, w, h):
        self.w = w
        self.h = h
        self.grid = [ZERO]*w*h

    def get(self, x, y):
        return self.grid[y*self.w + x]

    def set(self, x, y, a):
        self.grid[y*self.w + x] = a

    def copy_from(self, other, x0, y0):
        for i in range(other.w):
            for j in range(other.h):
                self.set(x0+i, y0+j, other.get(i, j))

    def each(self):
        for j in range(self.h):
            for i in range(self.w):
                yield self.get(i, j)

parser = argparse.ArgumentParser(description="extract palette from TGA file")
parser.add_argument("files", type=str, nargs="+", help="input colour mapped TGA file")

if __name__ == "__main__":
    args = parser.parse_args()
    ppr = 64 // 16
    if len(args.files) > ppr * ppr:
        raise Exception(f"too many inputs, must be {ppr*ppr} at most")
    big_pals = []
    small_pals = []
    for f in args.files:
        with open(f, "rb") as ff:
            print(f"trying {f}")
            bb = ff.read()
            tga = parse(bb)
            if tga.header.img_spec.width <= 16:
                small_pals.append(tga.img)
            elif tga.header.img_spec.width <= 256:
                big_pals.append(tga.img)
            else:
                raise Exception(f"cannot merge pallet with more than 256 colours: {f} = {tga.header.img_spec.width}")


    data_blocks = []
    for p in small_pals:
        r = Rect(16, 16)
        for i in range(8):
            if i < len(p):
                r.set(i, 0, p[i])
            else:
                r.set(i, 0, ZERO)
        for i in range(8):
            if 8 + i < len(p):
                r.set(i, 1, p[i+8])
            else:
                r.set(i, 1, ZERO)
        data_blocks.append(r)
    for p in big_pals:
        r = Rect(16, 16)
        for j in range(16):
            for i in range(16):
                ind = j*16 + i
                if ind < len(p):
                    r.set(i, j, p[i])
                else:
                    r.set(i, j, ZERO)

    if len(data_blocks) < ppr*ppr:
        for i in range(len(data_blocks), ppr*ppr):
            r = Rect(16, 16)
            data_blocks.append(r)

    out_image = Rect(64, 64)
    for xx in range(ppr):
        for yy in range(ppr):
            d = data_blocks[yy * ppr + xx]
            out_image.copy_from(d, xx*16, yy*16)

    img_data = list(out_image.each())

    col_spec = TgaColSpec(0, 0, 0)
    img_spec = TgaImgSpec(0, 0, 64, 64, 32, 0x20)
    out_header = TgaHeader(0, 0, 2, col_spec, img_spec)
    out = TGA(out_header, "", [], img_data)

    with open("pallet_glob.tga", "wb") as f:
        print(out.header)
        data = write(out)
        f.write(data)

    
    

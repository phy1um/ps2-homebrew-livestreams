from tga import parse, write
import sys
import argparse

parser = argparse.ArgumentParser(description="extract palette from TGA file")
parser.add_argument("file", type=str, help="input colour mapped TGA file")
parser.add_argument("output", type=str, help="name of output file for pallet")
parser.add_argument("-t", "--tiny", type=bool, default=False, help="compress indexes to 4 bits")

if __name__ == "__main__":
    args = parser.parse_args()
    with open(args.file, "rb") as f:
        bb = f.read()
        tga = parse(bb)
        tga.col = []
        
        if args.tiny:
            tga.header.img_spec.bits_per_pixel = 4
        else:
            tga.header.img_spec.bits_per_pixel = 8
        tga.header.img_type = 2
        tga.header.img_spec.xorigin = 0
        tga.header.img_spec.yorigin = 0
        tga.header.col_map_type = 0
        tga.header.col_spec.map_len = 0
        tga.header.col_spec.entry_bits = 1
        updated = write(tga)
        with open(args.output, "wb") as out:
            out.write(updated)


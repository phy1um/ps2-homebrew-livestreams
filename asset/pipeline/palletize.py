from tga import parse, write
import sys

if __name__ == "__main__":
    with open(sys.argv[1], "rb") as f:
        bb = f.read()
        tga = parse(bb)
        print(str(tga.header))
        nh = write(tga)
        with open("out.tga", "wb") as r:
            r.write(nh)


local gif = dofile("host:script/gif.lua")
local VRAM = require("vram")
local D2D = require("draw2d")

local gs = nil

function writeToBuffer(b, ints)
  print("writing " .. #ints .. " ints")
  for i=1,#ints,1 do
    b:pushint(ints[i])
  end
end

function PS2PROG.start()
  dma.init(dma.gif)
  gs.setoutput(640, 448, gs.interlaced, gs.ntsc)
  local fb1 = vram.buffer(640, 448, gs.psm24, 256)
  local fb2 = vram.buffer(640, 448, gs.psm24, 256)
  local zb = vram.buffer(640, 448, gs.psmz24, 256)
  gs.setbuffers(fb1, fb2, zb)
  d2d:clearcolour(0x2b, 0x2b, 0x2b)


end

function PS2PROG.frame()
  D2D:frameStart(gs)
  D2D:frameEnd(gs)
end



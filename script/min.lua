PS2PROG.slow2d = false

local gif = require("gif")
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
  DMA.init(DMA.GIF)
  GS.setOutput(640, 448, GS.INTERLACED, GS.NTSC)
  local fb1 = VRAM.buffer(640, 448, GS.PSM24, 256)
  local fb2 = VRAM.buffer(640, 448, GS.PSM24, 256)
  local zb = VRAM.buffer(640, 448, GS.PSMZ24, 256)
  GS.setBuffers(fb1, fb2, zb)
  D2D:clearColour(0x2b, 0x2b, 0x2b)
  D2D:screenDimensions(640, 448)

end

function PS2PROG.frame()
  D2D:frameStart(gs)
  D2D:frameEnd(gs)
end



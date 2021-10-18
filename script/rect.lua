
local GIF = require("gif")
local P = require("ps2const")
local D2D = require("draw2d")
local VRAM = require("vram")

local gs = nil

function PS2PROG.start()
  DMA.init(DMA.GIF)
  gs = GS.setOutput(640, 448, GS.INTERLACED, GS.NTSC)
  local fb1 = VRAM.buffer(640, 448, GS.PSM24, 256)
  local fb2 = VRAM.buffer(640, 448, GS.PSM24, 256)
  local zb = VRAM.buffer(640, 448, GS.PSMZ24, 256)
  GS.setBuffers(fb1, fb2, zb)
  D2D:clearColour(0x2b, 0x2b, 0x2b)

end

function PS2PROG.frame()
  D2D:frameStart(gs)
  D2D:setColour(255,0,0,0x80)
  D2D:rect(-200, -200, 200, 200)
  D2D:frameEnd(gs)
  D2D:kick()
  print("tris/frame = " .. D2D.prev.rawtri .. ", KC=" .. D2D.prev.kc)
end



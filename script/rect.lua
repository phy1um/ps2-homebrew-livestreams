
local GIF = require("gif")
local P = require("ps2const")
local D2D = require("draw2d")

local gs = nil

function PS2PROG.start()
  DMA.init(DMA.GIF)
  gs = GS.newState(640, 448, GS.INTERLACED, GS.NTSC)
  local fb = gs:alloc(640, 448, GS.PSM24)
  local zb = gs:alloc(640, 448, GS.PSMZ24)
  gs:setBuffers(fb, zb)
  gs:clearColour(0x2b, 0x2b, 0x2b)
end

function PS2PROG.frame()
  D2D:newBuffer()
  local db = D2D:getBuffer()
  db:frameStart(gs)
  D2D:setColour(255,0,0,0x80)
  D2D:rect(-200, -200, 200, 200)
  db = D2D:getBuffer()
  db:frameEnd(gs)
  D2D:kick()
  print("tris/frame = " .. D2D.rawtri .. ", KC=" .. D2D.kc)
  D2D.rawtri = 0
  D2D.kc = 0
  --db:free()
end



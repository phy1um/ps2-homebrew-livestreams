
local GIF = require"gif"
local P = require"ps2const"
local D2D = require"draw2d"
local VRAM = require"vram"
local DMA = require"p2g.dma"
local GS = require"p2g.gs"
local RM = require"p2g.buffer"

local gs = nil

function PS2PROG.start()
  PS2PROG.logLevel(5)
  DMA.init(DMA.GIF)
  gs = GS.setOutput(640, 448, GS.INTERLACED, GS.NTSC)
  local fb1 = VRAM.mem:framebuffer(640, 448, GS.PSM24, 2048)
  local fb2 = VRAM.mem:framebuffer(640, 448, GS.PSM24, 2048)
  local zb = VRAM.mem:framebuffer(640, 448, GS.PSMZ24, 2048)
  GS.setBuffers(fb1, fb2, zb)
  D2D:screenDimensions(640, 448)
  D2D:clearColour(0x2b, 0x2b, 0x2b)
  local db = RM.alloc(200 * 1024)
  D2D:bindBuffer(db)

end

function PS2PROG.frame()
  D2D:frameStart(gs)
  D2D:setColour(255,0,0,0x80)
  D2D:rect(20, 20, 220, 220)
  D2D:frameEnd(gs)
end



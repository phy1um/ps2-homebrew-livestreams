
local PAD = require"p2g.pad"
local LOG = require"p2g.log"
local GIF = require "p2g.gif"
local D2D = require "p2g.draw2d"
local VRAM = require "p2g.vram"
local DMA = require"p2g.dma"
local GS = require"p2g.gs"
local RM = require"p2g.buffer"

local hotreloadDraw = require "eg.hotreload_draw"
local H = hotreloadDraw.new(40, 40)

local db = 0

function PS2PROG.start()
  PS2PROG.logLevel(LOG.infoLevel)
  DMA.init(DMA.GIF)
  GS.setOutput(640, 448, GS.INTERLACED, GS.NTSC)
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
  D2D:frameStart()

  H:draw()

  D2D:frameEnd()

  if PAD.held(PAD.CIRCLE) and db < 0 then
    PS2PROG.logLevel(LOG.traceLevel)
    reload"eg.hotreload_draw"
    PS2PROG.logLevel(LOG.infoLevel)
    db = 30
  end
  
  db = db - 1
end



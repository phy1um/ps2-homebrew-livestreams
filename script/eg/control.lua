
local GIF = require"p2g.gif"
local P = require"p2g.const"
local D2D = require"p2g.draw2d"
local VRAM = require"p2g.vram"
local LOG = require"p2g.log"
local DMA = require"p2g.dma"
local GS = require"p2g.gs"
local RM = require"p2g.buffer"
local PAD = require "p2g.pad"

function PS2PROG.start()
  PS2PROG.logLevel(LOG.debugLevel)
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

local pp = {
  xx = 100,
  yy = 100,
}

function PS2PROG.frame()

  local dx = PAD.axis(PAD.axisLeftX)
  local dy = PAD.axis(PAD.axisLeftY)
  LOG.debug("dx = " .. dx .. ", dy = " .. dy)

  pp.xx = pp.xx + 5.2*dx
  pp.yy = pp.yy + 5.2*dy

  D2D:frameStart()
  D2D:setColour(255,0,0,0x80)
  D2D:rect(pp.xx, pp.yy, 24, 24)
  D2D:frameEnd()
end



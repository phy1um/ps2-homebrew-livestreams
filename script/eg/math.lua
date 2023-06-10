
local GIF = require"p2g.gif"
local P = require"p2g.const"
local D2D = require"p2g.draw2d"
local VRAM = require"p2g.vram"
local M = require"p2g.math"
local LOG = require"p2g.log"
local DMA = require"p2g.dma"
local GS = require"p2g.gs"
local RM = require"p2g.buffer"
local PAD = require"p2g.pad"

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

local pos = M.vec2(100, 100)
local angle = 0
local fwd = M.vec2(1, 0)
local rv = 0.07
local v = 1.6

local TAU = 2 * math.pi


function stepPlayer()
  if PAD.held(PAD.LEFT) then
    angle = angle - rv
  elseif PAD.held(PAD.RIGHT) then
    angle = angle + rv
  end
  local velocity = 0
  if PAD.held(PAD.UP) then velocity = v end
  if angle > TAU then angle = angle - TAU end
  if angle < 0 then angle = angle + TAU end
  local work = M.vec2From(fwd)
  work:rotate(angle)
  work:scale(velocity)
  pos:add(work)
end

function PS2PROG.frame()
  D2D:frameStart()
  stepPlayer()
  D2D:setColour(255,0,0,0x80)
  D2D:rect(pos.x, pos.y, 20, 20)
  local w2 = M.vec2From(fwd)
  w2:rotate(angle)
  w2:scale(60)
  D2D:setColour(0, 255, 255, 0x80)
  D2D:rect(pos.x + w2.x + 10, pos.y + w2.y + 10, 4, 4)
  D2D:frameEnd()
end



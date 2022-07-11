local GIF = require"gif"
local P = require"ps2const"
local D2D = require"draw2d"
local VRAM = require"vram"
local LOG = require"p2g.log"
local DMA = require"p2g.dma"
local GS = require"p2g.gs"

local emt = {
  x=0,y=0,w=1,h=1,
}
function emt:draw()
    --D2D:rect(self.x, self.y, self.w, self.h)
    D2D:triangle(self.x, self.y, self.x+self.w, self.y, self.x, self.y+self.h)
end

function emt.new(x,y,w,h)
  return setmetatable({x=x,y=y,w=w,h=h}, {__index = emt})
end

local scene = {}

function PS2PROG.start()
  DMA.init(DMA.GIF)
  GS.setOutput(640, 448, GS.INTERLACED, GS.NTSC)
  local fb1 = VRAM.mem:framebuffer(640, 448, GS.PSM24, 256)
  local fb2 = VRAM.mem:framebuffer(640, 448, GS.PSM24, 256)
  local zb = VRAM.mem:framebuffer(640, 448, GS.PSM24, 256)
  GS.setBuffers(fb1, fb2, zb)
  D2D:clearColour(0x2b, 0x2b, 0x2b)
  D2D:screenDimensions(640, 448)

  local vr = VRAM:slice(VRAM.mem.head)

  local dd = 100
  local dx = math.floor(640/dd)
  local dy = math.floor(448/dd)
  for x=0,620,dx do
    for y=0,420,dy do
      tt = emt.new(x,y,dx,dy)
      table.insert(scene, tt)
    end
  end

  local db = RM.alloc(200*1024)
  D2D:bindBuffer(db)
end

local r = 0xff
local g = 0xff
local ffps = 0
local fpsr = 10
local cc = 0

function PS2PROG.frame()
  D2D:frameStart()
  D2D:setColour(r,g,0,0x80)
  for i,s in ipairs(scene) do
    s:draw()
  end
  D2D:frameEnd()
  if r > 0 then
    g = 0xff
    r = 0xff
  else 
    g = 0
    r = 0xff
  end
  LOG.info("FPS: " .. FPS)
end



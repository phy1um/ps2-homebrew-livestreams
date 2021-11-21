--local GIF = dofile("host:script/gif.lua")
local GIF = require("gif")
local P = require("ps2const")
local D2D = require("draw2d")
local VRAM = require("vram")


local gs = nil

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

  local vr = VRAM:slice(VRAM.mem.head)

--  local len = math.floor((VRAM.max - VRAM.mem.head) / 2)
--  local abuf = VRAM:slice(VRAM.mem.head, len)
--  local bbuf = VRAM:slice(VRAM.mem.head + len, len)

  local dd = 100
  local dx = math.floor(640/dd)
  local dy = math.floor(448/dd)
  for x=-320,320,dx do
    for y=-224,224,dy do
      tt = emt.new(x,y,dx,dy)
      table.insert(scene, tt)
    end
  end
end

local r = 0xff
local g = 0xff
local ffps = 0
local fpsr = 10
local cc = 0

function PS2PROG.frame()
  D2D:frameStart(gs)
  D2D:setColour(r,g,0,0x80)
  for i,s in ipairs(scene) do
    s:draw()
  end
  D2D:frameEnd(gs)
  print("tris/frame = " .. D2D.prev.rawtri .. ", KC=" .. D2D.prev.kc)
  if r > 0 then
    g = 0xff
    r = 0
  else 
    g = 0
    r = 0xff
  end
  if FPS ~= nil then
    ffps = ffps + FPS
    cc = cc + 1
    if cc > fpsr then
      print("FPS: " .. (ffps/fpsr))
      cc = 0
    end
  end
  --db:free()
end



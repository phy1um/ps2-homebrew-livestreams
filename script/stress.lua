--local GIF = dofile("host:script/gif.lua")
local GIF = require("gif")
local P = require("ps2const")
local D2D = require("draw2d")


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
  gs = GS.newState(640, 448, GS.INTERLACED, GS.NTSC)
  local fb = gs:alloc(640, 448, GS.PSM24)
  local zb = gs:alloc(640, 448, GS.PSMZ24)
  gs:setBuffers(fb, zb)
  gs:clearColour(0x2b, 0x2b, 0x2b)

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

function PS2PROG.frame()
  D2D:newBuffer()
  local db = D2D:getBuffer()
  db:frameStart(gs)
  D2D:setColour(255,0,0,0x80)
  for i,s in ipairs(scene) do
    s:draw()
  end
  db = D2D:getBuffer()
  db:frameEnd(gs)
  D2D:kick()
  print("tris/frame = " .. D2D.rawtri .. ", KC=" .. D2D.kc)
  D2D.rawtri = 0
  D2D.kc = 0
  --db:free()
end



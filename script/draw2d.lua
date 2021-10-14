local GIF = require("gif")
local P = require("ps2const")

local DRAW_NONE = 0
local DRAW_GEOM = 1
local DRAW_SPRITE = 2

local DRAW_FMT_GEOM = {1,5,5,5}
local DRAW_FMT_SPRITE = {2,1,5,2,1,5}
local DB_SIZE = 5000

local draw = {
  col = {r=255, g=255, b=255, a=0x80},
  state = DRAW_NONE,
  loopCount = 0,
  tagLoopPtr = 0,
  currentTexPtr = 0,
  buf = nil,
  kc = 0,
  rawtri = 0,

}

function draw:newBuffer()
  self.state = DRAW_NONE
  self.loopCount = 0
  self.tagLoopPtr = 0
  self.buf = RM.getDrawBuffer(DB_SIZE)
  end

function draw:getBuffer()
  return self.buf
end

function draw:setColour(r,g,b,a)
  self.col.r = r
  self.col.g = g
  self.col.b = b
  self.col.a = a
end

function draw:rect(x, y, w, h)
  draw:triangle(x, y, x+w, y, x, y+h)
  draw:triangle(x, y+h, x+w, y+h, x+w, y)
end

function draw:sprite(tex, x, y, w, h, u1, v1, u2, v2)
  if self.loopCount > 10000 then self:kick() end
  if self.buf.size - self.buf.head < 80 then self:kick() end
  if self.state ~= DRAW_SPRITE or self.currentTexPtr ~= tex.basePtr then
    if self.state ~= DRAW_NONE then self:kick() end
    local pb = math.floor(tex.basePtr/64)
    local pw = math.floor(tex.width/64)
    GIF.tag(self.buf, GIF.PACKED, 4, false, {0xe})
    GIF.texA(self.buf, 0x80, 0x80)
    GIF.tex1(self.buf, true, 0, true, 0, 0)
    self.buf:settex(0, pb, pw, tex.format, math.floor(log2(tex.width)), math.floor(log2(tex.height)), 0, 1, 0, 0, 0)
    -- GIF.mipTbp1(self.buf, 0, pb, pw, pb, pw, pb, pw)
    -- GIF.mipTbp2(self.buf, 0, pb, pw, pb, pw, pb, pw)
    GIF.primAd(self.buf, P.PRIM.SPRITE, false, true, false)
    --GIF.packedRGBAQ(self.buf, self.col.r, self.col.g, self.col.b, self.col.a)
    self.tagLoopPtr = GIF.tag(self.buf, 0, 1, false, DRAW_FMT_SPRITE)
    self.loopCount = 0
    self.state = DRAW_SPRITE
  end
  GIF.packedST(self.buf, u1, v1)
  GIF.packedRGBAQ(self.buf, self.col.r, self.col.g, self.col.b, self.col.a)
  GIF.packedXYZ2(self.buf, 0x8000 + (x*16), 0x8000 + (y*16), 0)
  GIF.packedST(self.buf, u2, v2)
  GIF.packedRGBAQ(self.buf, self.col.r, self.col.g, self.col.b, self.col.a)
  GIF.packedXYZ2(self.buf, 0x8000 + ((x+w)*16), 0x8000 + ((y+h)*16), 0)
  self.loopCount = self.loopCount + 1
end

function draw:triangle(x1, y1, x2, y2, x3, y3)
  if self.loopCount > 10000 then self:kick() end
  if self.buf.size - self.buf.head < 80 then self:kick() end
  if self.state ~= DRAW_GEOM then
    if self.state ~= DRAW_NONE then self:kick() end
    GIF.tag(self.buf, 0, 1, false, {0xe})
    GIF.primAd(self.buf, P.PRIM.TRI, false, false, false)
    self.tagLoopPtr = GIF.tag(self.buf, 0, 1, false, DRAW_FMT_GEOM)
    self.loopCount = 0
    self.state = DRAW_GEOM
  end
  GIF.packedRGBAQ(self.buf, self.col.r, self.col.g, self.col.b, self.col.a)
  GIF.packedXYZ2(self.buf, 0x8000 + (x1*16), 0x8000 + (y1*16), 0)
  GIF.packedXYZ2(self.buf, 0x8000 + (x2*16), 0x8000 + (y2*16), 0)
  GIF.packedXYZ2(self.buf, 0x8000 + (x3*16), 0x8000 + (y3*16), 0)
  self.loopCount = self.loopCount + 1
  self.rawtri = self.rawtri + 1
end

function draw:kick()
  local nloop = self.buf:read(self.tagLoopPtr) 
  if nloop - 0x8000 > 0 then
    print("kick EOP")
    self.buf:write(self.tagLoopPtr, 0x8000 + self.loopCount)
  else
    self.buf:write(self.tagLoopPtr, self.loopCount)
  end
  DMA.send(self.buf, DMA.GIF)
  self:newBuffer()
  self.kc = self.kc + 1
end

return draw



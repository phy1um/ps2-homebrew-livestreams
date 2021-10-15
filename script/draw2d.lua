local GIF = require("gif")
local P = require("ps2const")
local VRAM = require("vram")

local DRAW_NONE = 0
local DRAW_GEOM = 1
local DRAW_SPRITE = 2

local DRAW_FMT_GEOM = {1,5,5,5}
local DRAW_FMT_SPRITE = {2,1,5,2,1,5}
local DB_SIZE = 20000

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
  self.col.r = math.floor(r)
  self.col.g = math.floor(g)
  self.col.b = math.floor(b)
  self.col.a = math.floor(a)
end

function draw:rect(x, y, w, h)
  draw:triangle(x, y, x+w, y, x, y+h)
  draw:triangle(x, y+h, x+w, y+h, x+w, y)
end

function draw:sprite(tex, x, y, w, h, u1, v1, u2, v2)
  if self.loopCount > 10000 then self:kick() end
  if self.buf.size - self.buf.head < 80 then self:kick() end
  if self.state ~= DRAW_SPRITE or self.currentTexPtr ~= tex.basePtr then
    if self.state ~= DRAW_NONE then
      draw:updateLastTagLoops() 
    end
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
  local ix = math.floor(x)
  local iy = math.floor(y)
  local ix2 = math.floor(x+w)
  local iy2 = math.floor(y+h)
  GIF.packedST(self.buf, u1, v1)
  GIF.packedRGBAQ(self.buf, self.col.r, self.col.g, self.col.b, self.col.a)
  GIF.packedXYZ2(self.buf, 0x8000 + (ix*16), 0x8000 + (iy*16), 0)
  GIF.packedST(self.buf, u2, v2)
  GIF.packedRGBAQ(self.buf, self.col.r, self.col.g, self.col.b, self.col.a)
  GIF.packedXYZ2(self.buf, 0x8000 + (ix2*16), 0x8000 + (iy2*16), 0)
  self.loopCount = self.loopCount + 1
end

function draw:triangle(x1, y1, x2, y2, x3, y3)
  if self.loopCount > 10000 then self:kick() end
  if self.buf.size - self.buf.head < 80 then self:kick() end
  if self.state ~= DRAW_GEOM then
    if self.state ~= DRAW_NONE then
      draw:updateLastTagLoops() 
    end
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
  draw:updateLastTagLoops()
  DMA.send(self.buf, DMA.GIF)
  self:newBuffer()
  self.kc = self.kc + 1
end

function draw:updateLastTagLoops()
  local nloop = self.buf:read(self.tagLoopPtr) 
  if nloop - 0x8000 > 0 then
    print("kick EOP")
    self.buf:write(self.tagLoopPtr, 0x8000 + self.loopCount)
  else
    self.buf:write(self.tagLoopPtr, self.loopCount)
  end
end

function draw.loadTexture(fname, w, h)
  print("LOAD TEX: " .. fname .. " @ PSM32")
  local tt = {
    basePtr = 0,
    width = w,
    height = h,
    data = nil,
    format = GS.PSM32
  }

  tt.data = TGA.load(fname, w, h)

  -- only works for power of 2 textures @ psm32!!!!!
  local texVramSize = w*h*4
  tt.basePtr = VRAM.alloc(texVramSize, 256)
  print("LOAD TEX: got texture VRAM addr = " .. tt.basePtr)

  -- ib = RM.tmpBuffer(1000)
  ib = RM.getDrawBuffer(5000)

  GIF.tag(ib, GIF.PACKED, 4, false, {0xe})
  GIF.bitBltBuf(ib, math.floor(tt.basePtr/64), math.floor(tt.width/64), tt.format)
  GIF.trxPos(ib,0,0,0,0,0)
  GIF.trxReg(ib,tt.width,tt.height)
  GIF.trxDir(ib, 0)

  -- ASSUMPTION about format!
  local eeSize = tt.width*tt.height*4
  local qwc = math.floor(eeSize / 16)
  if qwc % 16 ~= 0 then qwc = qwc + 1 end
  local blocksize = math.floor(4496/16)
  local packets = math.floor(qwc / blocksize)
  local remain = qwc % blocksize
  print("LOAD TEX: transmitting in " .. packets .. " packets with " .. remain .. " left")

  local tb = 0
  while packets > 0 do
    GIF.tag(ib, GIF.IMAGE, blocksize, false, {0}) 
    print("LOAD TEX: copy from TT " .. tb*blocksize*16 .. " to IB " .. ib.head .. " -- " .. blocksize*16)
    tt.data:copy(ib, ib.head, tb*blocksize*16, blocksize*16)
    ib.head = ib.head + blocksize*16
    DMA.send(ib, DMA.GIF)
    ib = RM.getDrawBuffer(5000)
    packets = packets - 1
    tb = tb + 1
  end

  if remain > 0 then
    local base = tb*blocksize*16
    GIF.tag(ib, GIF.IMAGE, remain, false, {1})
    print("copy from TT " .. base .. " to IB " .. ib.head .. " -- " .. remain*16)
    tt.data:copy(ib, ib.head, base, remain*16)
    ib.head = ib.head + remain*16
  end

  GIF.texflush(ib)
  DMA.send(ib, DMA.GIF)
  print("LOAD TEX: DMA send")

  return tt
end


return draw



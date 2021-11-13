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
  tagLoopPtr = -1,
  currentTexPtr = 0,
  buf = {},
  kc = 0,
  rawtri = 0,
  dmaTagQws = 0,
  dmaTagQwPtr = 0,
  prev = {
    kc = 0,
    rawtri = 0,
  }
}

function draw:newCnt()
  self.dmaTagQwPtr = self.buf.head
  self.buf:pushint(DMA.CNT)
  self.buf:pushint(0)
  self.buf:pushint(0)
  self.buf:pushint(0)
end

function draw:dmaEnd()
  self.buf:pushint(DMA.END)
  self.buf:pushint(0)
  self.buf:pushint(0)
  self.buf:pushint(0)
end

function draw:newBuffer()
  self.state = DRAW_NONE
  self.loopCount = 0
  self.tagLoopPtr = -1
  self.buf = RM.getDrawBuffer(DB_SIZE)
  self:newCnt()
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
  x = math.floor(x)
  y = math.floor(y)
  draw:triangle(x, y, x+w, y, x, y+h)
  draw:triangle(x, y+h, x+w, y+h, x+w, y)
end

function toCoord(i)
  local ii = math.floor(i)
  local fi = math.floor((i%1)*0xf)
  return 0x8000 + (ii*16) + fi
end

function draw:sprite(tex, x, y, w, h, u1, v1, u2, v2)
  if self.loopCount > 10000 then self:kick() end
  if self.buf.size - self.buf.head < 80 then self:kick() end
  if self.state ~= DRAW_SPRITE or self.currentTexPtr ~= tex.basePtr then
    if self.state ~= DRAW_NONE then
      draw:updateLastTagLoops() 
    end
    self.currentTexPtr = tex.basePtr
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
  GIF.packedXYZ2(self.buf, toCoord(x-320), toCoord(y-224), 0)
  GIF.packedST(self.buf, u2, v2)
  GIF.packedRGBAQ(self.buf, self.col.r, self.col.g, self.col.b, self.col.a)
  GIF.packedXYZ2(self.buf, toCoord(x+w-320), toCoord(y+h-224), 0)
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
  GIF.packedXYZ2(self.buf, toCoord(x1-320), toCoord(y1-224), 0)
  GIF.packedXYZ2(self.buf, toCoord(x2-320), toCoord(y2-224), 0)
  GIF.packedXYZ2(self.buf, toCoord(x3-320), toCoord(y3-224), 0)
  self.loopCount = self.loopCount + 1
  self.rawtri = self.rawtri + 1
end

function draw:kick()
  draw:updateLastTagLoops()
  local lw = self.buf:read(self.dmaTagQwPtr)
  local qwc = math.floor(self.buf.head / 16)
  if self.buf.head % 16 ~= 0 then
    qwc = qwc + 1
  end
  self.buf:write(self.dmaTagQwPtr, lw + qwc - 1)
  draw:dmaEnd()
  DMA.sendChain(self.buf, DMA.GIF)
  self:newBuffer()
  self.kc = self.kc + 1
end

function draw:updateLastTagLoops()
  if self.tagLoopPtr >= 0 then
    local nloop = self.buf:read(self.tagLoopPtr) 
    if nloop - 0x8000 > 0 then
      print("kick EOP")
      self.buf:write(self.tagLoopPtr, 0x8000 + self.loopCount)
    else
      self.buf:write(self.tagLoopPtr, self.loopCount)
    end
  end
end

function draw.loadTexture(fname, w, h)
  local tt = {
    width = w,
    height = h,
    data = nil,
    format = GS.PSM32,
    fname = fname,
  }
  tt.data = TGA.load(fname, w, h)
  return tt 
end

function draw.vramAllocTexture(tt)
  local texVramSize = tt.width*tt.height*4
  tt.basePtr = VRAM.alloc(texVramSize, 256)
  print("LOAD TEX: got texture VRAM addr = " .. tt.basePtr)
end

function draw:uploadTexture(tt)
  if tt.basePtr == nil then
    error("cannot upload texture that has not been allocated")
  end
  -- only works for power of 2 textures @ psm32!!!!!
  ib = self.buf

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
    self:kick()
    ib = self.buf
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
  self:kick()
  print("LOAD TEX: DMA send")

  return tt
end

function draw:frameStart(gs)
  self.kc = 0
  self.rawtri = 0
  self:newBuffer() 
  self.buf:frameStart(640, 448, self.clearR, self.clearG, self.clearB)
end

function draw:frameEnd(gs)
  self.buf:frameEnd(gs)
  self:kick()
  self.prev.kc = self.kc
  self.prev.rawtri = self.rawtri
end

function draw:clearColour(r, g, b)
  self.clearR = r
  self.clearG = g
  self.clearB = b
end

return draw



local GIF = require("gif")
local P = require("ps2const")
local VRAM = require("vram")

-- enum for how this script tracks current
-- drawing mode to minimize GIFTag swapping
local DRAW_NONE = 0
local DRAW_GEOM = 1
local DRAW_SPRITE = 2

-- GS Registers to set when drawing geometry
local DRAW_FMT_GEOM = {1,5,5,5}
-- GS Registers to set when drawing SPRITE primitives
local DRAW_FMT_SPRITE = {2,1,5,2,1,5}
-- Size of each drawbuffer before we have to split it
local DB_SIZE = 20000

-- Local draw state
local draw = {
  -- current colour
  col = {r=255, g=255, b=255, a=0x80},
  -- current state
  state = DRAW_NONE,
  -- number of loops in current GIFTag
  loopCount = 0,
  -- pointer to start of current GIFTag in active buffer
  tagLoopPtr = -1,
  -- VRAM addr of texture we are currently drawing
  currentTexPtr = 0,
  -- current drawbuffer (initialized each frame)
  buf = nil,
  -- how many drawbuffer "kick"s this frame
  kc = 0,
  -- how many raw triangles we pushed this frame
  rawtri = 0,
  -- pointer to start of current DMATag in active buffer
  dmaTagQwPtr = 0,
  -- are we currently in a CNT DMATag?
  isInCnt = false,
  -- metrics from previous frame
  prev = {
    kc = 0,
    rawtri = 0,
  }
}

-- start new CNT DMATag for standard GIFTag registers data
function draw:newCnt()
  self.dmaTagQwPtr = self.buf.head
  self:dmaTagRaw(DMA.CNT, 0, 0)
  self.isInCnt = true
end

-- end current CNT and calculate number of QWs
function draw:endCnt()
  if self.isInCnt then
    local lw = self.buf:read(self.dmaTagQwPtr)
    local bytes = self.buf.head - self.dmaTagQwPtr
    local qwc = math.floor(bytes / 16)
    if self.buf.head % 16 ~= 0 then
      qwc = qwc + 1
    end
    -- update the DMATag with the number of QWs
    self.buf:write(self.dmaTagQwPtr, lw + qwc - 1)
  end
end

-- put a DMATag in the current draw buffer
--  tt   - DMA type (eg DMA.CNT, DMA.REF)
--  qwc  - number of quadwords
--  addr - addr field (meaning depends on type, 0 for CNT)
function draw:dmaTagRaw(tt, qwc, addr)
  self.buf:pushint(qwc + tt)
  self.buf:pushint(addr)
  self.buf:pushint(0)
  self.buf:pushint(0)
end

-- put a DMATag with type END in the draw buffer
function draw:dmaEnd()
  self:dmaTagRaw(DMA.END, 0, 0)
end

-- get a new drawbuffer, called at frame start or after "kick"
function draw:newBuffer()
  self.state = DRAW_NONE
  self.loopCount = 0
  self.tagLoopPtr = -1
  self.buf = RM.getDrawBuffer(DB_SIZE)
  -- start a CNT by default, if we end the CNT without pushing anything 
  -- into it this is has no effect
  self:newCnt()
end

-- set current draw colour
function draw:setColour(r,g,b,a)
  self.col.r = math.floor(r)
  self.col.g = math.floor(g)
  self.col.b = math.floor(b)
  self.col.a = math.floor(a)
end

-- draw a rectangle
function draw:rect(x, y, w, h)
  x = math.floor(x)
  y = math.floor(y)
  -- TODO: optimize with a SPRITE primitive
  draw:triangle(x, y, x+w, y, x, y+h)
  draw:triangle(x, y+h, x+w, y+h, x+w, y)
end

-- convert a 32bit floating point number to a fixed point coordinate + offset
function toCoord(i)
  local ii = math.floor(i)
  local fi = math.floor((i%1)*0xf)
  return 0x8000 + (ii*16) + fi
end

-- draw a sprite
function draw:sprite(tex, x, y, w, h, u1, v1, u2, v2)
  if self.loopCount > 10000 then self:kick() end
  if self.buf.size - self.buf.head < 80 then self:kick() end
  -- if we aren't in a GIFTag drawing sprites for the current texture then...
  if self.state ~= DRAW_SPRITE or self.currentTexPtr ~= tex.basePtr then
    -- cleanup previous tag
    if self.state ~= DRAW_NONE then
      draw:updateLastTagLoops() 
    end
    -- setup texture drawing registers
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
  -- push geometry, colour and ST to drawbuffer
  GIF.packedST(self.buf, u1, v1)
  GIF.packedRGBAQ(self.buf, self.col.r, self.col.g, self.col.b, self.col.a)
  GIF.packedXYZ2(self.buf, toCoord(x-320), toCoord(y-224), 0)
  GIF.packedST(self.buf, u2, v2)
  GIF.packedRGBAQ(self.buf, self.col.r, self.col.g, self.col.b, self.col.a)
  GIF.packedXYZ2(self.buf, toCoord(x+w-320), toCoord(y+h-224), 0)
  self.loopCount = self.loopCount + 1
end

-- draw triangle
function draw:triangle(x1, y1, x2, y2, x3, y3)
  if self.loopCount > 10000 then self:kick() end
  if self.buf.size - self.buf.head < 80 then self:kick() end
  -- if we are not currently drawing raw geometry then...
  if self.state ~= DRAW_GEOM then
    -- cleanup previous GIFTag
    if self.state ~= DRAW_NONE then
      draw:updateLastTagLoops() 
    end
    -- setup GIFTag for drawing triangles
    GIF.tag(self.buf, 0, 1, false, {0xe})
    GIF.primAd(self.buf, P.PRIM.TRI, false, false, false)
    self.tagLoopPtr = GIF.tag(self.buf, 0, 1, false, DRAW_FMT_GEOM)
    self.loopCount = 0
    self.state = DRAW_GEOM
  end
  -- push PACKED triangle data into draw buffer
  GIF.packedRGBAQ(self.buf, self.col.r, self.col.g, self.col.b, self.col.a)
  GIF.packedXYZ2(self.buf, toCoord(x1-320), toCoord(y1-224), 0)
  GIF.packedXYZ2(self.buf, toCoord(x2-320), toCoord(y2-224), 0)
  GIF.packedXYZ2(self.buf, toCoord(x3-320), toCoord(y3-224), 0)
  self.loopCount = self.loopCount + 1
  self.rawtri = self.rawtri + 1
end

-- submit the current drawbuffer for rendering
function draw:kick()
  -- cleanup current GIFTag
  self:updateLastTagLoops()
  -- end any active CNT (does nothing if no CNT DMATag active)
  self:endCnt()
  -- add DMA END tag
  self:dmaEnd()
  DMA.sendChain(self.buf, DMA.GIF)
  -- refresh our drawbuffer, basically free so no concerns doing this at the
  --  end of the frame
  self:newBuffer()
  self.kc = self.kc + 1
end

-- cleanup current GIFTag with all relevant data
function draw:updateLastTagLoops()
  if self.tagLoopPtr >= 0 then
    local nloop = self.buf:read(self.tagLoopPtr) 
    -- if this GIFTag has EOP flag set
    if nloop - 0x8000 > 0 then
      -- write the number of loops back with EOP flag set 
      self.buf:write(self.tagLoopPtr, 0x8000 + self.loopCount)
    else
      -- otherwise just write the number of loops
      self.buf:write(self.tagLoopPtr, self.loopCount)
    end
  end
end

-- load a texture into EE memory
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

-- get VRAM address for texture
function draw.vramAllocTexture(tt)
  -- only works for power of 2 textures @ psm32!!!!!
  local texVramSize = tt.width*tt.height*4
  tt.basePtr = VRAM.alloc(texVramSize, 256)
  print("TEXTURE: got texture VRAM addr = " .. tt.basePtr)
end

-- upload a texture that has been VRAM allocated into GS memory
function draw:uploadTexture(tt)
  if tt.basePtr == nil then
    error("cannot upload texture that has not been allocated")
  end
  if self.buf.size - self.buf.head < 7 then self:kick() end
  -- ASSUME: DMATag CNT is active

  -- setup GS texture transfer registers for upload
  GIF.tag(self.buf, GIF.PACKED, 4, false, {0xe})
  GIF.bitBltBuf(self.buf, math.floor(tt.basePtr/64), math.floor(tt.width/64), tt.format)
  GIF.trxPos(self.buf,0,0,0,0,0)
  GIF.trxReg(self.buf,tt.width,tt.height)
  GIF.trxDir(self.buf, 0)

  self:endCnt()

  -- ASSUME: format is PSM32!
  local eeSize = tt.width*tt.height*4
  local qwc = math.floor(eeSize / 16)
  if qwc % 16 ~= 0 then qwc = qwc + 1 end
  local blocksize = math.floor(4496/16)
  -- number of whole GIFTags this transfer will take
  local packets = math.floor(qwc / blocksize)
  -- plus a possible remainder
  local remain = qwc % blocksize

  local tb = 0
  local imgAddr = tt.data.addr
  while packets > 0 do
    if self.buf.size - self.buf.head < 4 then self:kick() end
    -- for each fullsized packet, add a CNT with the GIFTag describing IMAGE
    --  data, followed by DMATag REF pointing to EE memory
    self:dmaTagRaw(DMA.CNT, 1, 0) 
    GIF.tag(self.buf, GIF.IMAGE, blocksize, false, {0}) 
    self:dmaTagRaw(DMA.REF, blocksize, imgAddr)
    imgAddr = imgAddr + blocksize*16
    packets = packets - 1
    tb = tb + 1
  end

  -- if there was any remainder, upload seperately
  if remain > 0 then
    if self.buf.size - self.buf.head < 4 then self:kick() end
    local base = tb*blocksize*16
    self:dmaTagRaw(DMA.CNT, 1, 0)
    GIF.tag(self.buf, GIF.IMAGE, remain, false, {1})
    self:dmaTagRaw(DMA.REF, remain, imgAddr)
  end

  -- start a new CNT and add a texflush command
  self:newCnt()
  GIF.texflush(self.buf)

  return true
end

-- setup frame and setup drawbuffer
function draw:frameStart(gs)
  self.kc = 0
  self.rawtri = 0
  self:newBuffer() 
  self.buf:frameStart(640, 448, self.clearR, self.clearG, self.clearB)
end

-- setup frame end, kick drawbuffer
function draw:frameEnd(gs)
  self.buf:frameEnd(gs)
  self:kick()
  self.prev.kc = self.kc
  self.prev.rawtri = self.rawtri
end

-- set clear colour
function draw:clearColour(r, g, b)
  self.clearR = r
  self.clearG = g
  self.clearB = b
end

return draw



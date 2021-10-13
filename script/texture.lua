
local GIF = require("gif")
local P = require("ps2const")
local D2D = require("draw2d")
local VRAM = require("vram")

function makeTex(w, h, fill)
  local t = {}
  for x=0,w,1 do
    for y=0,h,1 do
      t[y*w + x] = fill
    end
  end
  return t
end


local gs = nil
local testTexBuf = nil
local tex = makeTex(64, 64, 0xff)
local testTexBuf = 0
local tt = {
  basePtr = 0,
  width = 64,
  height = 64,
  data = tex,
}

function PS2PROG.start()
  DMA.init(DMA.GIF)
  gs = GS.newState(640, 448, GS.INTERLACED, GS.NTSC)
  --local fb = gs:alloc(640, 448, GS.PSM24)
  --local zb = gs:alloc(640, 448, GS.PSMZ24)
  local fb = VRAM.buffer(640, 440, GS.PSM24, 256)
  local zb = VRAM.buffer(640, 440, GS.PSMZ24, 256)
  print("setting new buffers")
  gs:setBuffers(fb, zb)
  gs:clearColour(0x2b, 0x2b, 0x2b)

  print("detailing texture")
  local width = 64
  local height = 64
  for i=0,width,1 do
    for j=0,height,1 do
      if (i+j)%2 == 0 then
        tex[j*width + i] = 0xff00
      end
    end
  end
  local texVramSize = 1024
  testTexBuf = VRAM.alloc(texVramSize, 256)
  tt.basePtr = math.floor(testTexBuf/64)
  print("got texture VRAM addr = " .. testTexBuf)

  -- ib = RM.tmpBuffer(1000)
  local taddr = math.floor(testTexBuf/256)
  ib = RM.getDrawBuffer(5000)

  GIF.tag(ib, GIF.PACKED, 4, false, {0xe})
  GIF.bitBltBuf(ib, math.floor(testTexBuf/64), math.floor(width/64), GS.PSM24)
  GIF.trxPos(ib,0,0,0,0,0)
  GIF.trxReg(ib,width,height)
  GIF.trxDir(ib, 0)

  local eeSize = width*height*4
  local qwc = math.floor(eeSize / 16)
  print("image sent in " .. qwc .. " qwords")
  if qwc % 16 ~= 0 then qwc = qwc + 1 end
  local blocksize = math.floor(4496/16)
  local packets = math.floor(qwc / blocksize)
  local remain = qwc % blocksize
  -- print("transmitting")
  print("transmitting in " .. packets .. " packets with " .. remain .. " lefte")

  local tb = 0
  while packets > 0 do
    GIF.tag(ib, GIF.IMAGE, blocksize, false, {0}) 
    for i=1,blocksize,1 do
      ib:pushint(tex[tb*blocksize*4 + i])
      ib:pushint(tex[tb*blocksize*4 + i+1])
      ib:pushint(tex[tb*blocksize*4 + i+2])
      ib:pushint(tex[tb*blocksize*4 + i+3])
    end
    DMA.send(ib, DMA.GIF)
    ib = RM.getDrawBuffer(5000)
    packets = packets - 1
  end

  if remain > 0 then
    local base = math.floor(qwc/blocksize)*blocksize*4
    GIF.tag(ib, GIF.IMAGE, remain, false, {1})
    for i=1,remain,1 do
      ib:pushint(tex[base + i])
      ib:pushint(tex[base + i + 1])
      ib:pushint(tex[base + i + 2])
      ib:pushint(tex[base + i + 3])
    end
  end

  print("adding texflush")
  GIF.texflush(ib)

  print("dma sending")
  DMA.send(ib, DMA.GIF)

end

function PS2PROG.frame()
  D2D:newBuffer()
  local db = D2D:getBuffer()
  db:frameStart(gs)
  D2D:setColour(255,255,255,0x80)
  D2D:rectuv(tt, -200, -200, 200, 200, 0, 0, 2^14-1, 2^14 - 1)
  db = D2D:getBuffer()
  db:frameEnd(gs)
  D2D:kick()
  print("tris/frame = " .. D2D.rawtri .. ", KC=" .. D2D.kc)
  D2D.rawtri = 0
  D2D.kc = 0
  --db:free()
end



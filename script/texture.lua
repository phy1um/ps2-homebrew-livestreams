
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
--local tex = makeTex(64, 64, 0x800000ff)
local tt = {
  basePtr = 0,
  width = 64,
  height = 64,
  data = nil,
  format = GS.PSM32
}

function PS2PROG.start()
  tt.data = TGA.load("host:test.tga", 64, 64)
  print("loaded tga: size = " .. tt.data.size .. " head = " .. tt.data.head)

  DMA.init(DMA.GIF)
  gs = GS.newState(640, 448, GS.INTERLACED, GS.NTSC)
  local fb = VRAM.buffer(640, 440, GS.PSM24, 256)
  local zb = VRAM.buffer(640, 440, GS.PSMZ24, 256)
  print("setting new buffers")
  gs:setBuffers(fb, zb)
  gs:clearColour(0x2b, 0x2b, 0x2b)

  --[[
  print("detailing texture")
  local width = 64
  local height = 64
  for i=0,width,1 do
    for j=0,height,1 do
      local r = 0x0f
      local g = 50 + math.floor(200 * j/height)
      tex[j*width + i] = 0x80000000 + math.floor(r + (g*2^8))
    end
  end
  ]]

  local texVramSize = 1024
  tt.basePtr = VRAM.alloc(texVramSize, 256)
  print("got texture VRAM addr = " .. tt.basePtr)

  -- ib = RM.tmpBuffer(1000)
  ib = RM.getDrawBuffer(5000)

  GIF.tag(ib, GIF.PACKED, 4, false, {0xe})
  GIF.bitBltBuf(ib, math.floor(tt.basePtr/64), math.floor(tt.width/64), tt.format)
  GIF.trxPos(ib,0,0,0,0,0)
  GIF.trxReg(ib,tt.width,tt.height)
  GIF.trxDir(ib, 0)

  local eeSize = tt.width*tt.height*4
  local qwc = math.floor(eeSize / 16)
  print("image sent in " .. qwc .. " qwords")
  if qwc % 16 ~= 0 then qwc = qwc + 1 end
  local blocksize = math.floor(4496/16)
  local packets = math.floor(qwc / blocksize)
  local remain = qwc % blocksize
  -- print("transmitting")
  print("transmitting in " .. packets .. " packets with " .. remain .. " left")

  local tb = 0
  while packets > 0 do
    GIF.tag(ib, GIF.IMAGE, blocksize, false, {0}) 
    print("copy from TT " .. tb*blocksize*4 .. " to IB " .. ib.head .. " -- " .. blocksize*16)
    tt.data:copy(ib, ib.head, tb*blocksize*4, blocksize*16)
    ib.head = ib.head + blocksize*16
    DMA.send(ib, DMA.GIF)
    ib = RM.getDrawBuffer(5000)
    packets = packets - 1
    tb = tb + 1
  end

  if remain > 0 then
    local base = tb*blocksize*4
    GIF.tag(ib, GIF.IMAGE, remain, false, {1})
    print("copy from TT " .. base .. " to IB " .. ib.head .. " -- " .. remain*16)
    tt.data:copy(ib, ib.head, base, remain*16)
    ib.head = ib.head + remain*16
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
  D2D:rectuv(tt, -200, -200, 200, 200, 0, 0, 1, 1)
  db = D2D:getBuffer()
  db:frameEnd(gs)
  D2D:kick()
  print("tris/frame = " .. D2D.rawtri .. ", KC=" .. D2D.kc)
  D2D.rawtri = 0
  D2D.kc = 0
  --db:free()
end



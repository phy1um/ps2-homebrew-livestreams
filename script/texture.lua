
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
local testTex = {}
local fnt = nil


-- CURRENT TEXTURE LOADER CANNOT BE USED MID-FRAME!!!!!
function loadTexture(fname, w, h)
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

function PS2PROG.start()
  testTex = D2D.loadTexture("host:test.tga", 64, 64)
  fnt = loadTexture("host:bigfont.tga", 256, 64)
  DMA.init(DMA.GIF)
  gs = GS.setOutput(640, 448, GS.INTERLACED, GS.NTSC)
  local fb1 = VRAM.buffer(640, 448, GS.PSM24, 256)
  local fb2 = VRAM.buffer(640, 448, GS.PSM24, 256)
  local zb = VRAM.buffer(640, 448, GS.PSMZ24, 256)
  GS.setBuffers(fb1, fb2, zb)
  D2D:clearColour(0x2b, 0x2b, 0x2b)
end

xx = -200
local dt = 1/60
function PS2PROG.frame()
  D2D:frameStart(gs)
  D2D:setColour(0x80,0x80,0x80,0x80)
  D2D:sprite(testTex, xx, -200, 200, 200, 0, 0, 1, 1)
  D2D:sprite(fnt, 50, 100, 256, 64, 0, 0, 1, 1)
  D2D:frameEnd(gs)
  print("tris/frame = " .. D2D.prev.rawtri .. ", KC=" .. D2D.prev.kc)

  if PAD.held(PAD.LEFT) then xx = xx - 50*dt end
  if PAD.held(PAD.RIGHT) then xx = xx + 50*dt end
  --db:free()
end



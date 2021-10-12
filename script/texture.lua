
local GIF = require("gif")
local P = require("ps2const")
local D2D = require("draw2d")


function makeTex(w, h, fill)
  local t = {}
  for x=0,w,1 do
    for y=0,h,1 do
      t[y*w + x] = fill
    end
  end
end


local gs = nil
local testTexBuf = nil
local tex = makeTex(50, 50, 0xff)

function PS2PROG.start()
  DMA.init(DMA.GIF)
  gs = GS.newState(640, 448, GS.INTERLACED, GS.NTSC)
  local fb = gs:alloc(640, 448, GS.PSM24)
  local zb = gs:alloc(640, 448, GS.PSMZ24)
  gs:setBuffers(fb, zb)
  gs:clearColour(0x2b, 0x2b, 0x2b)

  testTexBuf = gs:alloc(50, 50, GS.PSM24)
  for i=0,50,1 do
    for j=0,50,1 do
      if (i+j)%2 == 0 then
        tex[j*50 + i] = 0xff00
      end
    end
  end

  ib = RM:tmpBuffer(1000)
  GIF.tag(ib, GIF.IMAGE, x, 1, 1)
end

function PS2PROG.frame()
  D2D:newBuffer()
  local db = D2D:getBuffer()
  db:frameStart(gs)
  D2D:setColour(255,0,0,0x80)
  D2D:rect(-200, -200, 200, 200)
  db = D2D:getBuffer()
  db:frameEnd(gs)
  D2D:kick()
  print("tris/frame = " .. D2D.rawtri .. ", KC=" .. D2D.kc)
  D2D.rawtri = 0
  D2D.kc = 0
  --db:free()
end



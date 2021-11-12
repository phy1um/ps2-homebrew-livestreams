
local GIF = require("gif")
local P = require("ps2const")
local D2D = require("draw2d")
local VRAM = require("vram")

local gs = nil
local testTex = {}
local fnt = nil


function PS2PROG.start()
  DMA.init(DMA.GIF)
  testTex = D2D:loadTexture("host:test.tga", 64, 64)
  fnt = D2D:loadTexture("host:bigfont.tga", 256, 64)
  print("loaded test=" .. testTex.basePtr .. ", font=" .. fnt.basePtr)
  gs = GS.setOutput(640, 448, GS.NONINTERLACED, GS.NTSC)
  local fb1 = VRAM.buffer(640, 448, GS.PSM24, 256)
  local fb2 = VRAM.buffer(640, 448, GS.PSM24, 256)
  local zb = VRAM.buffer(640, 448, GS.PSMZ24, 256)
  GS.setBuffers(fb1, fb2, zb)
  D2D:clearColour(0x2b, 0x2b, 0x2b)
end

xx = 200
local dt = 1/60
local tt = 2
function PS2PROG.frame()
  D2D:frameStart(gs)
  if tt < 0 then
    D2D:uploadTexture(testTex)
    D2D:uploadTexture(fnt)
    D2D:setColour(0x80,0x80,0x80,0x80)
    D2D:sprite(testTex, xx, 200, 200, 200, 0, 0, 1, 1)
    D2D:sprite(fnt, 50, 100, 256, 64, 0, 0, 1, 1)
  else
    print("skip ", tt)
    tt = tt - dt
  end
  D2D:frameEnd(gs)
  print("tris/frame = " .. D2D.prev.rawtri .. ", KC=" .. D2D.prev.kc .. ", FPS=" .. FPS)

  if PAD.held(PAD.LEFT) then xx = xx - 50*dt end
  if PAD.held(PAD.RIGHT) then xx = xx + 50*dt end
  --db:free()
end



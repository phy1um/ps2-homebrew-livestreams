PS2PROG.slow2d = true

local GIF = require("gif")
local P = require("ps2const")
local D2D = require("draw2d")
local VRAM = require("vram")

local gs = nil
local testTex = nil
local pal = nil
local img
local texturesInVram = false
local vr = nil

function build4Tex(t)
  for j = 0,7,1 do
    for i = 0,63,1 do
      local v = j
      t.data:pushint(math.floor(v*0x11111111))
    end
  end
end


function PS2PROG.start()
  PS2PROG.logLevel(15)
  --testTex = D2D.loadTexture("host:test.tga", 64, 64)
  testTex = D2D.loadTexture("host:pp.tga")
  pal = D2D.loadTexture("host:bigpal.tga")
  DMA.init(DMA.GIF)
  gs = GS.setOutput(640, 448, GS.INTERLACED, GS.NTSC)
  local fb1 = VRAM.mem:framebuffer(640, 448, GS.PSM32, 256)
  local fb2 = VRAM.mem:framebuffer(640, 448, GS.PSM32, 256)
  local zb = VRAM.mem:framebuffer(640, 448, GS.PSMZ24, 256)
  GS.setBuffers(fb1, fb2, zb)
  D2D:screenDimensions(640, 448)
  D2D:clearColour(0x2b, 0x2b, 0x2b)

  img = D2D.newTexture(64, 64, GS.PSM4)
  build4Tex(img)

  vr = VRAM.slice(VRAM.mem.head)
  vr:texture(pal)
  vr:texture(img)
end

function uploadTextures()
  if not texturesInVram then
    D2D:uploadTexture(pal)
    D2D:uploadTexture(testTex)
    D2D:uploadTexture(img)
    texturesInVram = true
  end
end

xx = 200
local dt = 1/60
local tt = false
local db = 0
function PS2PROG.frame()
  D2D:frameStart(gs)
  uploadTextures()
  D2D:setColour(0x80,0x80,0x80,0x80)
  D2D:setClut(pal)
  D2D:sprite(img, 100, 100, 256, 256, 0, 0, 1, 1)
  D2D:sprite(testTex, 200, 200, 64, 64, 0, 0, 1, 1)
  D2D:frameEnd(gs)

  if PAD.held(PAD.x) and db <= 0 then 
    tt = not tt 
    print("state " .. tostring(tt))
    db = 30
  end
  db = db - 1 
end



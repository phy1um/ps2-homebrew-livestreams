
local GIF = require("gif")
local P = require("ps2const")
local D2D = require("draw2d")
local VRAM = require("vram")

local gs = nil
local testTex = {}
local fnt = nil
local texturesInVram = false
local vr = nil


function PS2PROG.start()
  PS2PROG.logLevel(5)
  testTex = D2D.loadTexture("host:test.tga", 64, 64)
  fnt = D2D.loadTexture("host:bigfont.tga", 256, 64)
  DMA.init(DMA.GIF)
  gs = GS.setOutput(640, 448, GS.INTERLACED, GS.NTSC)
  local fb1 = VRAM.mem:framebuffer(640, 448, GS.PSM24, 256)
  local fb2 = VRAM.mem:framebuffer(640, 448, GS.PSM24, 256)
  local zb = VRAM.mem:framebuffer(640, 448, GS.PSMZ24, 256)
  GS.setBuffers(fb1, fb2, zb)
  D2D:screenDimensions(640, 448)
  D2D:clearColour(0x2b, 0x2b, 0x2b)

  vr = VRAM.slice(VRAM.mem.head)
  vr:texture(testTex)
  vr:texture(fnt)
end

function uploadTextures()
  if not texturesInVram then
    D2D:uploadTexture(testTex)
    D2D:uploadTexture(fnt)
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
  D2D:sprite(testTex, 20, 20, 64, 64, 0, 0, 1, 1)
  D2D:frameEnd(gs)

  if PAD.held(PAD.x) and db <= 0 then 
    tt = not tt 
    print("state " .. tostring(tt))
    db = 30
  end
  db = db - 1 
end



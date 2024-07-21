local GIF = require"p2g.gif"
local P = require"p2g.const"
local D2D = require"p2g.draw2d"
local VRAM = require"p2g.vram"
local LOG = require"p2g.log"
local DMA = require"p2g.dma"
local GS = require"p2g.gs"
local TGA = require"p2g.tga"
local PAD = require"p2g.pad"
local RM = require"p2g.buffer"
local IO = require"p2g.io"

local testTex1 = nil
local testTex2 = nil
local texturesInVram = false
local vr = nil

function PS2PROG.start()
  PS2PROG.logLevel(LOG.debugLevel)
  testTex1 = TGA.from_file("test.tga", RM.alloc)
  testTex2 = TGA.from_file("half.tga", RM.alloc)
  DMA.init(DMA.GIF)
  GS.setOutput(640, 448, GS.INTERLACED, GS.NTSC)
  local fb1 = VRAM.mem:framebuffer(640, 448, GS.PSM24, 256)
  local fb2 = VRAM.mem:framebuffer(640, 448, GS.PSM24, 256)
  local zb = VRAM.mem:framebuffer(640, 448, GS.PSMZ24, 256)
  GS.setBuffers(fb1, fb2, zb)
  D2D:screenDimensions(640, 448)
  D2D:clearColour(0x2b, 0x2b, 0x2b)

  vr = VRAM.slice(VRAM.mem.head)
  vr:texture(testTex1)
  vr:texture(testTex2)

  local db = RM.alloc(200 * 1024)
  D2D:bindBuffer(db)
end

function uploadTextures()
  if not texturesInVram then
    D2D:uploadTexture(testTex1)
    D2D:uploadTexture(testTex2)
    texturesInVram = true
  end
end

xx = 200
local dt = 1/60
local tt = false
local db = 0
function PS2PROG.frame()
  LOG.trace("ps2prog frame start")
  D2D:frameStart()
  uploadTextures()
  D2D:setColour(0x80,0x80,0x80,0x80)
  for i = 0, 5, 1 do
    D2D:sprite(testTex2, 20 + i*10, 20 + i*10, 64, 64, 0, 0, 1, 1)
  end
  D2D:textri(testTex1, 200, 200, 0, 0, 
    200, 300, 0, 1,
    300, 300, 1, 1)
  D2D:frameEnd()

  if PAD.held(PAD.x) and db <= 0 then 
    tt = not tt 
    LOG.info("state " .. tostring(tt))
    db = 30
  end
  db = db - 1 
end



local GIF = require"p2g.gif"
local P = require"p2g.const"
local D2D = require"p2g.draw2d"
local VRAM = require"p2g.vram"
local LOG = require"p2g.log"
local DMA = require"p2g.dma"
local GS = require"p2g.gs"
local RM = require"p2g.buffer"
local TGA = require"p2g.tga"

local testTex = nil
local pal = nil
local texturesInVram = false
local vr = nil

function PS2PROG.start()
  PS2PROG.logLevel(LOG.debugLevel)
  testTex = TGA.from_file("host:picotiles4.tga", RM.alloc)
  --testTex.data:print()
  pal = TGA.from_file("host:bigpal.tga", RM.alloc)
  DMA.init(DMA.GIF)
  GS.setOutput(640, 448, GS.INTERLACED, GS.NTSC)
  local fb1 = VRAM.mem:framebuffer(640, 448, GS.PSM32, 2048)
  local fb2 = VRAM.mem:framebuffer(640, 448, GS.PSM32, 2048)
  local zb = VRAM.mem:framebuffer(640, 448, GS.PSMZ24, 2048)
  GS.setBuffers(fb1, fb2, zb)
  D2D:screenDimensions(640, 448)
  D2D:clearColour(0x2b, 0x2b, 0x2b)

  img = D2D.newTexture(64, 64, GS.PSM4)

  vr = VRAM.slice(VRAM.mem.head)
  vr:texture(pal)
  vr:texture(testTex)

  local drawbuffer = RM.alloc(200 * 1024)
  D2D:bindBuffer(drawbuffer)
end

function uploadTextures()
  if not texturesInVram then
    D2D:uploadTexture(pal)
    D2D:uploadTexture(testTex)
    texturesInVram = true
  end
end

function drawTile(x, y, i)
  local ix = i % 4
  local iy = math.floor(i/4)
  D2D:sprite(testTex, x*32, y*32, 32, 32, 0.25*ix, 0.25*iy, 0.25*ix + 0.25, 0.25*iy + 0.25)
end

function PS2PROG.frame()
  D2D:frameStart()
  uploadTextures()
  D2D:setColour(0x80,0x80,0x80,0x80)
  D2D:setClut(pal)
  for i=0,19,1 do
    for j=0,15,1 do
      drawTile(i, j, 0)
    end
  end
  for i=0,10,1 do
    drawTile(i, 0, 4)
  end
  drawTile(4, 9, 8)
  drawTile(5, 9, 9)
  drawTile(6, 9, 9)
  drawTile(7, 9, 10)
  D2D:frameEnd()
end



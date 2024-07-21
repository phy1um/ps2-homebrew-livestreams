
local GIF = require"p2g.gif"
local P = require"p2g.const"
local D2D = require"p2g.draw2d"
local VRAM = require"p2g.vram"
local DMA = require"p2g.dma"
local GS = require"p2g.gs"
local RM = require"p2g.buffer"
local LOG = require"p2g.log"
local FONT = require"p2g.font"
local TGA = require"p2g.tga"

function PS2PROG.start()
  PS2PROG.logLevel(LOG.traceLevel)
  DMA.init(DMA.GIF)
  GS.setOutput(640, 448, GS.INTERLACED, GS.NTSC)
  local fb1 = VRAM.mem:framebuffer(640, 448, GS.PSM24, 2048)
  local fb2 = VRAM.mem:framebuffer(640, 448, GS.PSM24, 2048)
  local zb = VRAM.mem:framebuffer(640, 448, GS.PSMZ24, 2048)
  vram = VRAM.slice(VRAM.mem.head)
  GS.setBuffers(fb1, fb2, zb)
  D2D:screenDfontimensions(640, 448)

  D2D:clearColour(0x2b, 0x2b, 0x2b)
  local db = RM.alloc(200 * 1024)
  D2D:bindBuffer(db)

  local fontTexture = TGA.from_file("bigfont.tga", RM.alloc)
  font = FONT.new(fontTexture, 8, 16)
end

function PS2PROG.frame()
  D2D:frameStart()
  font:loadToVram(vram)
  D2D:setColour(255,0,0,0x80)
  font:printLines(10, 10,
    "Hello world. This is a simple font renderer.",
    "It is capable of printing text over many lines",
    "from a fixed-width font spritesheet.")
  D2D:setColour(0,255,255,0x80)
  font:printLines(10, 90,
    "The font colour can be changed by setting the 2D",
    "render colour. This is good.")
  D2D:setColour(255,255,255,0x80)
  font:centerPrint(320, 224, 0, "we can also put text in the middle of the screen")
  D2D:frameEnd()
end



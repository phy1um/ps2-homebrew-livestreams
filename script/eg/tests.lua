local GIF = require"p2g.gif"
local P = require"p2g.const"
local D2D = require"p2g.draw2d"
local VRAM = require"p2g.vram"
local DMA = require"p2g.dma"
local GS = require"p2g.gs"
local RM = require"p2g.buffer"
local LOG = require"p2g.log"
local FONT = require"p2g.font"
local PAD = require"p2g.pad"
local TGA = require"p2g.tga"

local camera = {
  x = 0,
  y = 0,
}

function PS2PROG.start()
  PS2PROG.logLevel(LOG.debugLevel)
  DMA.init(DMA.GIF)
  GS.setOutput(640, 448, GS.INTERLACED, GS.NTSC)
  local fb1 = VRAM.mem:framebuffer(640, 448, GS.PSM24, 2048)
  local fb2 = VRAM.mem:framebuffer(640, 448, GS.PSM24, 2048)
  local zb = VRAM.mem:framebuffer(640, 448, GS.PSMZ24, 2048)
  vram = VRAM.slice(VRAM.mem.head)
  GS.setBuffers(fb1, fb2, zb)
  D2D:screenDimensions(640, 448)
  D2D:clearColour(0x2b, 0x2b, 0x2b)
  local db = RM.alloc(200 * 1024)
  D2D:bindBuffer(db)

  local fontTexture = TGA.from_file("bigfont.tga", RM.alloc)
  font = FONT.new(fontTexture, 8, 16)
end

local function clip_print(x, y, ...)
  local screen_x = x - camera.x
  local screen_y = y - camera.y
  if screen_x < -16 or screen_y < -16 or screen_x > 640 or screen_y > 448 then
    return
  end
  font:printLines(screen_x, screen_y, ...)
end

local function printTestCase(offset_x, offset_y, suite_name, record)
  D2D:setColour(255,255,255,0x80)
  clip_print(offset_x, offset_y, "Tests for " .. suite_name)
  local offset_y = offset_y + 16
  for case, result in pairs(record) do
    local sx = offset_x + 10
    if result.ok then 
      D2D:setColour(0, 255, 0, 0x80)
      clip_print(sx, offset_y, "PASS")
      sx = sx + font.charWidth * 5
    else
      D2D:setColour(255, 0, 0, 0x80)
      clip_print(sx, offset_y, "FAIL")
      sx = sx + font.charWidth * 5
    end
    D2D:setColour(110,110,110,0x80)
    clip_print(sx, offset_y, "case " .. result.test_name)
    offset_y = offset_y + 16
  end
  return offset_y
end


function PS2PROG.frame()
  D2D:frameStart()
  font:loadToVram(vram)

  local yy = 10
  for name,record in pairs(test_record) do
    yy = printTestCase(10, yy, name, record)
  end

  if PAD.held(PAD.DOWN) then
    camera.y = camera.y + 2
  end
  if PAD.held(PAD.UP) then
    camera.y = camera.y - 2
  end


  D2D:frameEnd()
end



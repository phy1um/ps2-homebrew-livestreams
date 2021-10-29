
local GIF = require("gif")
local P = require("ps2const")
local D2D = require("draw2d")
local VRAM = require("vram")

print("running the thing")

local testTex = {}

function PS2PROG.start()
  DMA.init(DMA.GIF)
  GS.setOutput(640, 448, GS.INTERLACED, GS.NTSC)
  local fb1 = VRAM.buffer(640, 448, GS.PSM24, 256)
  local fb2 = VRAM.buffer(640, 448, GS.PSM24, 256)
  local zb = VRAM.buffer(640, 448, GS.PSMZ24, 256)
  testTex = D2D.loadTexture("host:test.tga", 64, 64)
  GS.setBuffers(fb1, fb2, zb)
  D2D:clearColour(0x2b, 0x2b, 0x2b)
end


local uvc = {0.5, 1, 2.0}
local uvi = 0
local nextPressed = false

function PS2PROG.frame()
  D2D:frameStart(gs)
  D2D:setColour(0x80,0x80,0x80,0x80)
  local uv2 = uvc[uvi+1]
  D2D:sprite(testTex, 200, 200, 200, 200, 0, 0, uv2, uv2)
  D2D:frameEnd(gs)
  -- print("tris/frame = " .. D2D.prev.rawtri .. ", KC=" .. D2D.prev.kc)

  if PAD.held(PAD.X) then
    if not nextPressed then
      uvi = (uvi + 1) % #uvc
      local nv = uvc[uvi+1]
      print("Tex Region = 0, 0, " .. nv .. ", " .. nv)
    end
    nextPressed = true
  else
    nextPressed = false
  end
end



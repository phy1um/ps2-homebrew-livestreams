
local SEARCH_PATH = PS2_SCRIPT_PATH .. "script/?.lua"

dbgPrint("search path = " .. SEARCH_PATH)
package.path = SEARCH_PATH

function PS2PROG.spinForever()
  while true do local c = 1+1 end
end

local d2d = require("draw2d")
FAST_DRAW2D.loadTexture = d2d.loadTexture
FAST_DRAW2D.newTexture = d2d.newTexture
FAST_DRAW2D.vramAllocTexture = d2d.vramAllocTexture

local trueRequire = require

function require(p)
  if p == "draw2d" then
    if not PS2PROG.slow2d then
      return FAST_DRAW2D
    end
  end
  return trueRequire(p)
end

-- TODO: make this a function
TGA.BPS_TO_PSM = {}
TGA.BPS_TO_PSM[4] = GS.PSM4
TGA.BPS_TO_PSM[8] = GS.PSM8
TGA.BPS_TO_PSM[16] = GS.PSM16
TGA.BPS_TO_PSM[24] = GS.PSM24
TGA.BPS_TO_PSM[32] = GS.PSM32

return function() end


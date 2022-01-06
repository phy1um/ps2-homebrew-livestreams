
local SEARCH_PATH = PS2_SCRIPT_PATH .. "script/?.lua"

dbgPrint("search path = " .. SEARCH_PATH)
package.path = SEARCH_PATH

function PS2PROG.spinForever()
  while true do local c = 1+1 end
end

local d2d = require("draw2d")
FAST_DRAW2D.loadTexture = d2d.loadTexture
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

return function() end


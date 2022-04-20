
local SEARCH_PATH = PS2_SCRIPT_PATH .. "script/?.lua"

dbgPrint("search path = " .. SEARCH_PATH)
package.path = SEARCH_PATH

function PS2PROG.spinForever()
  while true do local c = 1+1 end
end

LOG = P2GCORE.log
DMA = P2GCORE.dma
GS = P2GCORE.gs
TGA = P2GCORE.tga
RM = P2GCORE.buffer
PAD = P2GCORE.pad

local hr = require "hot"
require, reload = hr.init()

local trueRequire = require

function require(p)
  LOG.trace("-> require " .. p)
  local out = trueRequire(p)
  LOG.trace("<- require " .. p)
  return out
end

local d2d = require("draw2d")

LOG.trace("setup fast draw2d")
P2GCORE.draw2d.loadTexture = d2d.loadTexture
P2GCORE.draw2d.newTexture = d2d.newTexture
P2GCORE.draw2d.vramAllocTexture = d2d.vramAllocTexture

LOG.trace("setup hacked require")
function require(p)
  if p == "draw2d" then
    if not PS2PROG.slow2d then
      return P2GCORE.draw2d
    end
  end
  return trueRequire(p)
end

LOG.trace("setup TGA constants")
-- TODO: make this a function
TGA.BPS_TO_PSM = {}
TGA.BPS_TO_PSM[4] = GS.PSM4
TGA.BPS_TO_PSM[8] = GS.PSM8
TGA.BPS_TO_PSM[16] = GS.PSM16
TGA.BPS_TO_PSM[24] = GS.PSM24
TGA.BPS_TO_PSM[32] = GS.PSM32

function ps2Resource(path)
  return PS2_SCRIPT_PATH .. path
end

LOG.trace("ps2init end")

return function() end


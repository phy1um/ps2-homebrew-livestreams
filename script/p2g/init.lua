
local LOG = P2GCORE.log

dbgPrint("p2g.init begin")

-- Setup search path scripts in PS2 environment 
local SEARCH_PATH = P2G_ROOT .. "script/?.lua"
dbgPrint("search path = " .. SEARCH_PATH)
package.path = SEARCH_PATH

-- Define a function to block the EE Core, sometimes
-- useful for debugging. This will break ps2link reset
function PS2PROG.spinForever()
  while true do local c = 1+1 end
end

-- Setup hot-reload
dbgPrint("setup hot reloading")
local hotReload = require"p2g.hot"
require, reload = hotReload.init()

-- Make alias names for P2G core libs that can be required
P2GCORE_BINDS = {
  ["p2g.dma"] = P2GCORE.dma,
  ["p2g.gs"] = P2GCORE.gs,
  ["p2g.tga"] = P2GCORE.tga,
  ["p2g.buffer"] = P2GCORE.buffer,
  ["p2g.pad"] = P2GCORE.pad,
  ["p2g.draw2d"] = P2GCORE.draw2d,
}

local function coreLoad(as) 
  dbgPrint("hard loading P2G core: " .. as)
  package.loaded["p2g." .. as] = P2GCORE[as]
end

coreLoad("log")
coreLoad("dma")
coreLoad("gs")
coreLoad("tga")
coreLoad("buffer")
coreLoad("pad")

-- Get a reference to the Lua scripted draw2d renderer before we override require
local d2d = require("p2g.draw2d")

-- Create hacked logging require with P2G library hooks
local trueRequire = require

function require(p)
  LOG.trace("-> require " .. p)
  local out = trueRequire(p)
  LOG.trace("<- require " .. p)
  return out
end

local wrappedRequire = require

dbgPrint("setup hacked require")
function require(p)
  if p == "p2g.draw2d" then
    return P2GCORE.draw2d
  end
  return wrappedRequire(p)
end

-- Do some initial draw2d setup
dbgPrint("setup fast draw2d")
P2GCORE.draw2d.loadTexture = d2d.loadTexture
P2GCORE.draw2d.newTexture = d2d.newTexture
P2GCORE.draw2d.vramAllocTexture = d2d.vramAllocTexture

-- Initialize TGA library constants from GS constants
dbgPrint("setup TGA constants")
local TGA = P2GCORE.tga
local GS = P2GCORE.gs
TGA.BPS_TO_PSM = {}
TGA.BPS_TO_PSM[4] = GS.PSM4
TGA.BPS_TO_PSM[8] = GS.PSM8
TGA.BPS_TO_PSM[16] = GS.PSM16
TGA.BPS_TO_PSM[24] = GS.PSM24
TGA.BPS_TO_PSM[32] = GS.PSM32

dbgPrint("p2g.init end")

return function() end



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

-- Allow for requiring core C code
local function coreLoad(as) 
  dbgPrint("hard loading P2G core: " .. as)
  package.loaded["p2g." .. as] = P2GCORE[as]
end

coreLoad("log")
coreLoad("dma")
coreLoad("gs")
coreLoad("io")
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
  -- Return scripted draw2d if the flag is set
  if p == "p2g.draw2d" then
    if PS2PROG.slow2d == true then 
      return d2d
    else
      return P2GCORE.draw2d
    end
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


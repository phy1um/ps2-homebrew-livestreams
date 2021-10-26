
local libs = {}
local SEARCH_PATH = PS2_SCRIPT_PATH .. "?.lua"

--[[
local myreq = function(name)
  print("require " .. name)
  local l = libs[name]
  if l ~= nil then
    return l
  else
    l = dofile(SEARCH_PATH .. name .. ".lua")
    libs[name] = l
    return l
  end
end

require = myreq

function loadFnl(name)
  local f = myreq("fennel")
  print("loaded fennel")
  local l = libs[name]
  if l ~= nil then
    return l
  else
    l = f.dofile(SEARCH_PATH .. "foo" .. ".fnl")
    libs[name] = l
    return l
  end
end
]]

print("setting up package searchers")
--table.insert(package.serchers or package.loaders, function
package.path = SEARCH_PATH
local fennel = require ("fennel")
--fennel.path = "host:script/?.fnl"
fennel.path = PS2_SCRIPT_PATH .. "?.fnl"
print("seting fennel path = " .. fennel.path)
table.insert(package.loaders or package.searchers, fennel.searcher)

return function() end


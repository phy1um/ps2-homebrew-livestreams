
local libs = {}
local SEARCH_PATH = "host:script/"

function require(name)
  local l = libs[name]
  if l ~= nil then
    return l
  else
    l = dofile(SEARCH_PATH .. name .. ".lua")
    libs[name] = l
    return l
  end
end

return function() end


local coreRequire = require


function reload(p, ...)
  if package.loaded[p] ~= nil then
    package.loaded[p] = nil
  end
  return require(p, ...)
end

print("go go go")
if love ~= nil then
  local fennel = require("script/fennel")
  require = function(p, ...)
    if p == "draw2d" then p = "script/lovedraw"
    elseif p == "text" then p = "script/lovedraw"
    else p = "script/" .. p
    end
    return coreRequire(p, ...)
  end
  table.unpack = unpack
  print("loading love2d main")
  print("inserting fennel searchers")
  table.insert(package.loaders or package.searchers, fennel.make_searcher{correlate=true})
  fennel.dofile("script/lovemain.fnl")
else
  require = function(p, ...)
    print("REQ+" .. p)
    local rr = coreRequire(p, ...)
    print("REQ-" .. p)
    return rr
  end
  local fennel = require("fennel")
  fennel.dofile("host:script/main.fnl")
end
-- dofile("host:script/bundle.lua")

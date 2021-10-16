
local coreRequire = require

local fennel = require("fennel")
print("go go go")
if love ~= nil then
  require = function(p, ...)
    if p == "draw2d" then p = "lovedraw" end
    if p == "text" then p = "lovedraw" end
    return coreRequire(p, ...)
  end
  print("loading love2d main")
  print("inserting fennel searchers")
  table.insert(package.loaders or package.searchers, fennel.make_searcher{correlate=true})
  fennel.dofile("lovemain.fnl")
else
  fennel.dofile("host:script/main.fnl")
end
-- dofile("host:script/bundle.lua")

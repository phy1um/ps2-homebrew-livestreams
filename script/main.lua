
local fennel = require("fennel")
print("go go go")
if love ~= nil then
  print("loading love2d main")
  print("inserting fennel searchers")
  table.insert(package.loaders or package.searchers, fennel.make_searcher{correlate=true})
  fennel.dofile("lovemain.fnl")
else
  fennel.dofile("host:script/main.fnl")
end
-- dofile("host:script/bundle.lua")

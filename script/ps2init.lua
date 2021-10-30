
local SEARCH_PATH = PS2_SCRIPT_PATH .. "script/?.lua"

dbgPrint("search path = " .. SEARCH_PATH)
package.path = SEARCH_PATH

function PS2PROG.spinForever()
  while true do local c = 1+1 end
end

return function() end



local D2D = require "draw2d"

local hld = {
  x = 0,
  y = 0,
  w = 40,
  h = 40,
}

function hld.new(x, y)
  return setmetatable({
    x = x,
    y = y,
  }, { __index = hld })
end

function hld:draw()
  D2D:setColour(0xff, 0xff, 0, 0x80)
  D2D:rect(self.x, self.y, self.w, self.h)
end

return hld

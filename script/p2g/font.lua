
local D2D = require"p2g.draw2d"

local font = {
  texture = nil,
  charWidth = 8,
  charHeight = 14,
  charS = 0.03125,
  charT = 0.25
}

local function getCharacterIndex(i)
  if i >= 0 and i <= 32 then
    return i+96
  elseif i >= 33 and i <= 126 then
    return i-32
  else
    error("bad character index in string " .. i)
  end
end

function font:drawString(line, x, y)
  if line == nil then return end
  for i=1,#line,1 do
    local ci = getCharacterIndex(string.byte(line, i))
    local ts = (ci % self.charsPerLine) * self.charS
    local tt = math.floor(ci/self.charsPerLine) * self.charT
    D2D:sprite(self.texture, x+(self.charWidth*i), y, self.charWidth, self.charHeight, ts, tt, 
      ts + self.charS, tt + self.charT)
  end
end

function font:printLines(x, y, ...)
  for i, l in ipairs({...}) do
    self:drawString(l, x, y + ((i-1)*self.charHeight))
  end
end

function font:centerPrint(x, y, width, str)
  local strWidth = string.len(str) * self.charWidth
  local sx = math.floor((width - strWidth) / 2)
  self:drawString(str, x + sx, y)
end

function font:loadToVram(vr)
  if self.resident == false then
    vr:texture(self.texture)
    D2D:uploadTexture(self.texture)
    self.resident = true
  end
end

function font:markUnloaded()
  self.resident = false
end


function font.new(texture, charWidth, charHeight)
  return setmetatable({
    texture = texture,
    charWidth = charWidth,
    charHeight = charHeight,
    charS = charWidth / texture.width,
    charT = charHeight / texture.height,
    charsPerLine = 32,
    resident = false,
  }, { __index = font })
end

return font


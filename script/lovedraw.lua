
local draw = {
  kc = 0,
  rawtri = 0,
  events = {},
  font = nil,
  x = "X",
  square = "[]",
  circle = "O",
  triange = "/\\",
  prev = {
    kc = 0,
    rawtri = 0,
  },
}

function draw:newBuffer()
  return {}
end

function draw:getBuffer()
  return {
    frameStart = function() end,
    frameEnd = function() end,
  }
end

function draw:setColour(r,g,b,a)
  table.insert(self.events, function()
    love.graphics.setColor(r/255,g/255,b/255,a/0x80)
  end)
end

function draw:rect(x, y, w, h)
  table.insert(self.events, function()
    love.graphics.rectangle("fill", x, y, w, h)
  end)
end

function draw:sprite(tex, x, y, w, h, u1, v1, u2, v2)
  table.insert(self.events, function()
    local tox = u1 * tex.width
    local toy = v1 * tex.height
    local ttx = u2 * tex.width
    local tty = v2 * tex.height
    local tw = ttx - tox
    local th = tty - toy
    local q = love.graphics.newQuad(tox, toy, tw, th, tex.width, tex.height)
    love.graphics.draw(tex.data, q, x, y, 0, w/tw, h/th)
  end)
end

function draw:triangle(x1, y1, x2, y2, x3, y3)
  table.insert(self.events, function()
    love.graphics.print("tri", x1, y1)
  end)
end

function draw:kick()
end

function draw:updateLastTagLoops()
end

function draw.loadTexture(fname, w, h)
  local nn = "dist/" .. string.sub(fname, 6, -1)
  print("hooking load " .. fname .. " -> " .. nn)
  local tt = {
    basePtr = 0,
    width = w,
    height = h,
    data = love.graphics.newImage(nn),
    format = GS.PSM32
  }
  return tt
end

function draw:doLoveDraw()
  for i,e in ipairs(self.events) do
    e()
  end
  self.events = {}
end

function draw.printLines(x, y, ...)
  local narg = select("#", ...)
  for i=1,narg,1 do
    local e = select(i, ...)
    table.insert(draw.events, function()
      love.graphics.print(e, x, (y + (i * 16)))
    end)
  end
end

function draw.clearColour(r, g, b)
end

function draw.frameStart() end
function draw.frameEnd() end


return draw



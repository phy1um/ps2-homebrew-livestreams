
local draw = {
  kc = 0,
  rawtri = 0,
}

function draw:newBuffer()
  return {}
end

function draw:getBuffer()
  return {}
end

function draw:setColour(r,g,b,a)
  love.graphics.setColor(r,g,b,255)
end

function draw:rect(x, y, w, h)
  love.graphics.rectangle("fill", x+320, y+240, w, h)
end

function draw:sprite(tex, x, y, w, h, u1, v1, u2, v2)
  love.graphics.rectangle("fill", x+320, y+240, w, h)
end

function draw:triangle(x1, y1, x2, y2, x3, y3)
  love.graphics.print("tri", x1+320, y1+240)
end

function draw:kick()
end

function draw:updateLastTagLoops()
end

function draw.loadTexture(fname, w, h)
end


return draw



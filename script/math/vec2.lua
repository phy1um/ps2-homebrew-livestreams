local M = MATH_C_LIB

local vec2 = {}

function vec2.__index(o, k)
  if k == "x" then
    local buf = rawget(o, "buf")
    return buf:getFloat(0)
  elseif k == "y" then
    local buf = rawget(o, "buf")
    return buf:getFloat(1)
  else
    return vec2[k]
  end
end

function vec2.__newindex(o, k, v)
  if k == "x" then
    local b = rawget(o, "buf")
    b:setFloat(0, v)
  elseif k == "y" then
    local b = rawget(o, "buf")
    b:setFloat(1, v)
  else
    rawset(o, k, v)
  end
end

function vec2:__tostring()
  return "[" .. self.x .. "," .. self.y .. "]"
end

function vec2.__add(a, b)
  local n = vec2.from(a)
  n:add(b)
  return n
end

function vec2.__sub(a, b)
  local n = vec2.from(a)
  n:sub(b)
  return n
end

function vec2.__mul(a, s)
  local n = vec2.from(a)
  n:scale(s)
  return n
end

function vec2.new(x,y)
  local buf = RM.alloc(4 * 2)  
  local o = {buf = buf}
  setmetatable(o, vec2)
  o.x = x
  o.y = y
  return o
end

function vec2.from(other)
  local n = vec2.new(0, 0)
  n:copy(other)
  return n
end

function vec2:add(other)
  M.addVec2(self.buf, other.buf)
end

function vec2:sub(other)
  M.subVec2(self.buf, other.buf)
end

function vec2:copy(from)
  M.copyVec2(self.buf, from.buf)
end

function vec2:length()
  return M.lenVec2(self.buf)
end

function vec2:dot(other)
  return M.dotVec2(self.buf, other.buf)
end

function vec2:rotate(theta)
  M.rotateVec2(self.buf, theta)
end

function vec2:copyRotate(theta)
  local n = vec2.from(self)
  n:rotate(theta)
  return n
end

function vec2:scale(s)
  M.scaleVec2(self.buf, s)
end

function feq(a, b)
  return math.abs(a-b) < 0.00001
end

function vec2:equal(other)
  return feq(self.x, other.x) and feq(self.y, other.y) 
end

return vec2

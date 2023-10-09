local vec4 = {}
local RM = require"p2g.buffer"
local M = P2GCORE.math_vec4
local floatCmp = P2GCORE.math_misc.floatCmp


function vec4.__index(o, k)
  if k == "x" then
    local buf = rawget(o, "buf")
    return buf:getFloat(0)
  elseif k == "y" then
    local buf = rawget(o, "buf")
    return buf:getFloat(1)
  elseif k == "z" then
    local buf = rawget(o, "buf")
    return buf:getFloat(2)
  elseif k == "w" then
    local buf = rawget(o, "buf")
    return buf:getFloat(3)
  else
    return vec4[k]
  end
end

function vec4.__newindex(o, k, v)
  if k == "x" then
    local b = rawget(o, "buf")
    b:setFloat(0, v)
  elseif k == "y" then
    local b = rawget(o, "buf")
    b:setFloat(1, v)
  elseif k == "z" then
    local b = rawget(o, "buf")
    b:setFloat(2, v)
  elseif k == "w" then
    local b = rawget(o, "buf")
    b:setFloat(3, v)
  else
    rawset(o, k, v)
  end
end

function vec4:__eq(other)
  return floatCmp(self.x, other.x) and floatCmp(self.y, other.y) and floatCmp(self.z, other.z) and floatCmp(self.w, other.w)
end


function vec4:__tostring()
  return "[" .. self.x .. "," .. self.y .. "," .. self.z .. "," .. self.w .. "]"
end

function vec4.__add(a, b)
  local n = vec4.from(a)
  n:add(b)
  return n
end

function vec4.__sub(a, b)
  local n = vec4.from(a)
  n:sub(b)
  return n
end

function vec4.__mul(a, s)
  local n = vec4.from(a)
  n:scale(s)
  return n
end

function vec4.new(x,y,z,w)
  local buf = RM.gcAlloc(4 * 4)  
  local o = {buf = buf}
  setmetatable(o, vec4)
  o.x = x
  o.y = y
  o.z = z
  o.w = w
  return o
end

function vec4.from(other)
  local n = vec4.new(0,0,0,0)
  n:copy(other)
  return n
end

function vec4:add(other)
  M.addVec4(self.buf, other.buf)
end

function vec4:sub(other)
  M.subVec4(self.buf, other.buf)
end

function vec4:copy(from)
  M.copyVec4(self.buf, from.buf)
end

function vec4:length()
  return M.lenVec4(self.buf)
end

function vec4:dot(other)
  return M.dotVec4(self.buf, other.buf)
end

function vec4:scale(s)
  M.scaleVec4(self.buf, s)
end

function vec4:normalize()
  M.normalizeVec4(self.buf)
end

return vec4


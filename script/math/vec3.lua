local vec3 = {}
local RM = require"p2g.buffer"
local M = P2GCORE.math_vec3
local floatCmp = P2GCORE.math_misc.floatCmp


function vec3.__index(o, k)
  if k == "x" then
    local buf = rawget(o, "buf")
    return buf:getFloat(0)
  elseif k == "y" then
    local buf = rawget(o, "buf")
    return buf:getFloat(1)
  elseif k == "z" then
    local buf = rawget(o, "buf")
    return buf:getFloat(2)
  else
    return vec3[k]
  end
end

function vec3.__newindex(o, k, v)
  if k == "x" then
    local b = rawget(o, "buf")
    b:setFloat(0, v)
  elseif k == "y" then
    local b = rawget(o, "buf")
    b:setFloat(1, v)
  elseif k == "z" then
    local b = rawget(o, "buf")
    b:setFloat(2, v)
  else
    rawset(o, k, v)
  end
end

function vec3:__eq(other)
  return floatCmp(self.x, other.x) and floatCmp(self.y, other.y) and floatCmp(self.z, other.z)
end


function vec3:__tostring()
  return "[" .. self.x .. "," .. self.y .. "," .. self.z .. "]"
end

function vec3.__add(a, b)
  local n = vec3.from(a)
  n:add(b)
  return n
end

function vec3.__sub(a, b)
  local n = vec3.from(a)
  n:sub(b)
  return n
end

function vec3.__mul(a, s)
  local n = vec3.from(a)
  n:scale(s)
  return n
end

function vec3.new(x,y, z)
  local buf = RM.gcAlloc(4 * 3)  
  local o = {buf = buf}
  setmetatable(o, vec3)
  o.x = x
  o.y = y
  o.z = z
  return o
end

function vec3.from(other)
  local n = vec3.new(0, 0)
  n:copy(other)
  return n
end

function vec3:add(other)
  M.addVec3(self.buf, other.buf)
end

function vec3:sub(other)
  M.subVec3(self.buf, other.buf)
end

function vec3:copy(from)
  M.copyVec3(self.buf, from.buf)
end

function vec3:length()
  return M.lenVec3(self.buf)
end

function vec3:dot(other)
  return M.dotVec3(self.buf, other.buf)
end

function vec3:scale(s)
  M.scaleVec3(self.buf, s)
end

function vec3:normalize()
  M.normalizeVec3(self.buf)
end

return vec3

local M = P2GCORE.math_mat4
local floatCmp = P2GCORE.math_misc.floatCmp
local RM = require"p2g.buffer"
local LOG = require"p2g.log"

local mat4 = {}

function mat4.__index(o, k) 
  local n = tonumber(k)
  if n ~= nil and n >= 0 and n < 16 then
    local buf = rawget(o, "buf")
    return buf:getFloat(n) 
  else return mat4[k]
  end
end

function mat4.__newindex(o, k, v)
  local n = tonumber(k)
  if n ~= nil and n >= 0 and n < 16 then
    local buf = rawget(o, "buf")
    return buf:setFloat(n, v) 
  else return rawset(o, k, v)
  end
end

function mat4:__tostring()
  base = "[" 
  for i=0,15,1 do
    if i > 0 and i % 4 == 0 
      then base = base .. "|" .. self[i] .. " "
      else base = base .. self[i] .. " "
    end
  end
  return base .. "]"
end

function mat4:__eq(other)
  for i=0,15,1 do
    if floatCmp(self[i], other[i]) == false then 
      LOG.info("mismatch @ (" .. i .. ") " .. self[i] .. " -> " .. other[i])
      return false 
    end
  end
  return true
end

function mat4.new(x)
  local buf = RM.gcAlloc(4 * 16) 
  if x == nil then 
    -- if no args initialize to identity matrix
    M.identityMat4(buf)
  else
    -- otherwise copy from nested lists, ew
    for row=1,4,1 do
      for col=1,4,1 do
        buf:setFloat((row-1)*4 + (col-1), x[row][col])
      end
    end
  end
  return setmetatable({buf = buf}, mat4)
end

function mat4:set(x,y,v)
  self.buf:setFloat(x + (4*y), v)
end

function mat4:add(other)
  M.addMat4(self.buf, other.buf)
end

function mat4:mul(other)
  M.mulMat4(self.buf, other.buf)
end

function mat4:apply(v4)
  M.applyMat4(self.buf, v4.buf)
end

function mat4:get(col, row) 
  return self[(row * 4) + col]
end

function mat4:copy(from)
  M.copyMat4(self.buf, from.buf)
end

function mat4:identity()
  M.identityMat4(self.buf)
end

return mat4


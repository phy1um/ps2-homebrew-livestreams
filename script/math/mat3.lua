local M = P2GCORE.math_mat3
local floatCmp = P2GCORE.math_misc.floatCmp



local mat3 = {}

function mat3.__index(o, k) 
  local n = tonumber(k)
  if n ~= nil and n >= 0 and n < 9 then
    local buf = rawget(o, "buf")
    return buf:getFloat(n) 
  else return mat3[k]
  end
end

function mat3.__newindex(o, k, v)
  local n = tonumber(k)
  if n ~= nil and n >= 0 and n < 9 then
    local buf = rawget(o, "buf")
    return buf:setFloat(n, v) 
  else return rawset(o, k, v)
  end
end

function mat3:__tostring()
  base = "[" 
  for i=0,8,1 do
    if i > 0 and i % 3 == 0 
      then base = base .. "|" .. self[i] .. " "
      else base = base .. self[i] .. " "
    end
  end
  return base .. "]"
end

function mat3:__eq(other)
  for i=0,8,1 do
    if floatCmp(self[i], other[i]) == false then 
      LOG.info("mismatch @ (" .. i .. ") " .. self[i] .. " -> " .. other[i])
      return false 
    end
  end
  return true
end

function mat3.new(x)
  local buf = RM.gcAlloc(4 * 9) 
  if x == nil then 
    -- if no args initialize to identity matrix
    M.identityMat3(buf)
  else
    -- otherwise copy from nested lists, ew
    for row=1,3,1 do
      for col=1,3,1 do
        buf:setFloat((row-1)*3 + (col-1), x[row][col])
      end
    end
  end
  return setmetatable({buf = buf}, mat3)
end

function mat3:add(other)
  M.addMat3(self.buf, other.buf)
end

function mat3:mul(other)
  M.mulMat3(self.buf, other.buf)
end

function mat3:apply(v3)
  M.applyMat3(self.buf, v3.buf)
end

function mat3:get(col, row) 
  return self[(row * 3) + col]
end

function mat3:copy(from)
  M.copyMat3(self.buf, from.buf)
end

return mat3

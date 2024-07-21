
local M = require"p2g.math"
local test = require"p2g.test"

local suite = {name = "Vector 3D"}

function suite.vec3_init()
  local v = M.vec3(5, 17, 9)
  test.equal(v.buf:getFloat(0), 5)
  test.equal(v.buf:getFloat(1), 17)
  test.equal(v.buf:getFloat(2), 9)
  test.equal(v.x, 5)
  test.equal(v.y, 17)
  test.equal(v.z, 9)
end

function suite.vec3_copy()
  local v = M.vec3(111, 222, 333)
  local other = M.vec3From(v)
  test.equal(v.x, other.x)
  test.equal(v.y, other.y)
  test.equal(v.z, other.z)
end

function suite.vec3_add()
  local v1 = M.vec3(0, 77, 100)
  local v2 = M.vec3(10, 20, 30)
  local v3 = v1 + v2
  test.equal(v3, M.vec3(10, 97, 130))
end

function suite.vec3_sub()
  local v1 = M.vec3(10, 32, 11)
  local v2 = M.vec3(7, 12, 19)
  local v3 = v1 - v2
  test.equal(v3, M.vec3(3, 20, -8))
end

function suite.vec3_length()
  test.equal(M.vec3(0,0, 0):length(), 0)
  test.equal(M.vec3(1, 0, 0):length(), 1)
  test.equal(M.vec3(3, 4, 8):length(), math.sqrt(89))
end

function suite.vec3_dot()
  local d1 = M.vec3(0,0,0):dot(M.vec3(7, 19, 99))
  test.equal(d1, 0)
end

function suite.vec3_scale()
  local v = M.vec3(4, 7, 11)
  local n = v * 5
  test.equal(n, M.vec3(20, 35, 55))
end

function suite.vec3_normalize()
  local v = M.vec3(10, 7, 1)
  v:normalize()
  test.equal(v, M.vec3(10 / math.sqrt(150), 7 / math.sqrt(150), 1 / math.sqrt(150)))
end

return suite

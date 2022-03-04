
local M = require("ps2math")
local test = require("test")
local suite = {name = "Vector 2D"}

function suite.vec2_init()
  local v = M.vec2(5, 17)
  test.equal(v.buf:getFloat(0), 5)
  test.equal(v.buf:getFloat(1), 17)
  test.equal(v.x, 5)
  test.equal(v.y, 17)
end

function suite.vec2_copy()
  local v = M.vec2(111, 222)
  local other = M.vec2From(v)
  test.equal(v.x, other.x)
  test.equal(v.y, other.y)
end

function suite.vec2_add()
  local v1 = M.vec2(0, 77)
  local v2 = M.vec2(10, 20)
  local v3 = v1 + v2
  test.equal(v3, M.vec2(10, 97))
end

function suite.vec2_sub()
  local v1 = M.vec2(10, 32)
  local v2 = M.vec2(7, 12)
  local v3 = v1 - v2
  test.equal(v3, M.vec2(3, 20))
end

function suite.vec2_length()
  test.equal(M.vec2(0,0):length(), 0)
  test.equal(M.vec2(1, 0):length(), 1)
  test.equal(M.vec2(3, 4):length(), 5)
end

function suite.vec2_dot()
  local d1 = M.vec2(0,0):dot(M.vec2(7, 19))
  test.equal(d1, 0)
end

function suite.vec2_scale()
  local v = M.vec2(4, 7)
  local n = v * 5
  test.equal(n, M.vec2(20, 35))
end

function suite.vec2_normalize()
  local v = M.vec2(10, 7)
  v:normalize()
  test.equal(v, M.vec2(0.819232, 0.5734623))
end

return function() return test.run_suite(suite.name, suite) end

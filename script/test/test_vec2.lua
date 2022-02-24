
local M = require("ps2math")
local test = require("test")
local suite = {name = "Vector 2D"}

function suite.vec2_init()
  local v = M.vec2(5, 17)
  assert(v.buf:getFloat(0) == 5)
  assert(v.buf:getFloat(1) == 17)
  assert(v.x == 5)
  assert(v.y == 17)
end

function suite.vec2_copy()
  local v = M.vec2(111, 222)
  local other = M.vec2From(v)
  assert(v.x == other.x)
  assert(v.y == other.y)
end

function suite.vec2_add()
  local v1 = M.vec2(0, 77)
  local v2 = M.vec2(10, 20)
  local v3 = v1 + v2
  assert(v3.x == 10)
  assert(v3.y == 97)
end

function suite.vec2_sub()
  local v1 = M.vec2(10, 32)
  local v2 = M.vec2(7, 12)
  local v3 = v1 - v2
  assert(v3.x == 3)
  assert(v3.y == 20)
end

function suite.vec2_length()
  assert(M.vec2(0,0):length() == 0)
  assert(M.vec2(1, 0):length() == 1)
  assert(M.vec2(3, 4):length() == 5)
end

function suite.vec2_dot()
  local d1 = M.vec2(0,0):dot(M.vec2(7, 19))
  assert(d1 == 0)
end

function suite.vec2_scale()
  local v = M.vec2(4, 7)
  local n = v * 5
  assert(n.x == 20)
  assert(n.y == 35)
end


return function() return test.run_suite(suite.name, suite) end

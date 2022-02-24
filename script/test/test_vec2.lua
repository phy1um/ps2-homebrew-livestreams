
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


return function() test.run_suite(suite.name, suite) end


local M = require("ps2math")
local test = require("test")

local t = {}

function t.mat3_init()
  local a = M.mat3()
  assert(a[0] == 1)
  assert(a[1] == 0)
  assert(a[3] == 0)
  assert(a[4] == 1)
end

function t.mat3_add()
  local a = M.mat3()
  local b = M.mat3({ {1,2,3},{0,0,0},{0,0,0} })
  M.mat3_mt.add(a, b)
  local c = M.mat3({ {2,2,3},{0,1,0},{0,0,1} })
  assert(a:equal(c))
end

function t.mat3_multiply_identity()
  local ii = M.mat3()
  local a = M.mat3({ {1,2,3},{0,0,0},{0,0,0} })
  local x = M.mat3()
  x:copy(a)
  x:mul(ii)
  assert(a:equal(x))
end

function t.test_mat3_multiply()
  local ta = M.mat3({ {5,12,9},{2,4.2,1},{11,8,0.2} })
  local tb = M.mat3({ {7,12,122},{0.1,17,8},{5,100,-2} })
  local tres = M.mat3({ {81.2,1164,688},{19.42,195.4,275.6},{78.8,288,1405.6} })
  ta:mul(tb)
  assert(ta:equal(tres))
end

function t.test_mat3_apply_identity()
  local ii = M.mat3()
  local va = M.vec3(5,12,7.4)
  local vb = M.vec3From(va)
  ii:apply(va)
  assert(va:equal(vb))
end

function t.test_mat3_apply_rotate()
  local ra = M.vec3(12, 8, 1)
  local rx = M.vec3(10.17144 + 10, 10.22456 + 4, 1)
  local rm = M.mat3({ {math.cos(0.2), -1 * math.sin(0.2), 10}, {math.sin(0.2), math.cos(0.2), 4}, {0,0,1} })
  rm:apply(ra)
  assert(ra:equal(rx))
end

return function() test.run_suite("3x3 Matrix", t) end



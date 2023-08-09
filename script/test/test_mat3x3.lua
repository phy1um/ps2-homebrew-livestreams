
local M = require"p2g.math"
local test = require"p2g.test"

local t = {name = "Matrix 3x3"}

function t.mat3_init()
  local a = M.mat3()
  test.equal(a[0], 1)
  test.equal(a[1], 0)
  test.equal(a[3], 0)
  test.equal(a[4], 1)
end

function t.mat3_add()
  local a = M.mat3()
  local b = M.mat3({ {1,2,3},{0,0,0},{0,0,0} })
  a:add(b)
  local c = M.mat3({ {2,2,3},{0,1,0},{0,0,1} })
  test.equal(a, c)
end

function t.mat3_multiply_identity()
  local ii = M.mat3()
  local a = M.mat3({ {1,2,3},{0,0,0},{0,0,0} })
  local x = M.mat3()
  x:copy(a)
  x:mul(ii)
  test.equal(a, x)
end

function t.test_mat3_multiply()
  local ta = M.mat3({ {5,12,9},{2,4.2,1},{11,8,0.2} })
  local tb = M.mat3({ {7,12,122},{0.1,17,8},{5,100,-2} })
  local tres = M.mat3({ {81.2,1164,688},{19.419998,195.4,275.599976},{78.799995,288,1405.6} })
  ta:mul(tb)
  test.equal(ta, tres)
end

function t.test_mat3_apply_identity()
  local ii = M.mat3()
  local va = M.vec3(5,12,7.4)
  local vb = M.vec3From(va)
  ii:apply(va)
  test.equal(va, vb)
end

function t.test_mat3_apply_rotate()
  local ra = M.vec3(12, 8, 1)
  local rx = M.vec3(10.17144429 + 10, 10.2245646 + 4, 1)
  local rm = M.mat3({ {math.cos(0.2), -1 * math.sin(0.2), 10}, {math.sin(0.2), math.cos(0.2), 4}, {0,0,1} })
  rm:apply(ra)
  test.equal(ra, rx)
end

return t



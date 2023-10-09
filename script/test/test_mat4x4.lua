
local LOG = require"p2g.log"
local M = require"p2g.math"
local test = require"p2g.test"

local t = {name = "Matrix 4x4"}

function t.mat4_init()
  local a = M.mat4()
  for i=0,3 do
    for j=0,3 do
      v = a[j*4 + i]
      if i == j then
        if v ~= 1 then
          test.fail(string.format("expected 1 @ [%d,%d], got %d", i, j, v))
        end
      elseif v ~= 0 then
        test.fail(string.format("expected 0 @ [%d,%d], got %d", i, j, v))
      end
    end
  end
end

function t.mat4_add()
  local a = M.mat4()
  local b = M.mat4({ {1,2,3,4},{0,0,0,0},{0,0,0,0},{0,0,0,0} })
  a:add(b)
  local c = M.mat4({ {2,2,3,4},{0,1,0,0},{0,0,1,0},{0,0,0,1} })
  test.equal(a, c)
end

function t.mat4_multiply_identity()
  local ii = M.mat4()
  local a = M.mat4({ {1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16} })
  local x = M.mat4()
  x:copy(a)
  x:mul(ii)
  test.equal(a, x)
end

return t



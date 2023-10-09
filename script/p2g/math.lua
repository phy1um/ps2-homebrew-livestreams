
local types = {
  vec2_mt = require("math.vec2"),
  vec3_mt = require("math.vec3"),
  vec4_mt = require("math.vec4"),
  mat3_mt = require("math.mat3"),
  mat4_mt = require("math.mat4"),
}

types.vec2 = types.vec2_mt.new
types.vec2From = types.vec2_mt.from
types.vec3 = types.vec3_mt.new
types.vec3From = types.vec3_mt.from
types.vec4 = types.vec4_mt.new
types.vec4From = types.vec4_mt.from

types.mat3 = types.mat3_mt.new
types.mat4 = types.mat4_mt.new

local EPSILON = 0.0001

function types.floatCmp(a, b)
  return math.abs(a-b) < EPSILON
end

return types


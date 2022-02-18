
local types = {
  vec2_mt = require("math.vec2"),
  vec3_mt = require("math.vec3"),
  mat3_mt = require("math.mat3"),
}

types.vec2 = types.vec2_mt.new
types.vec2From = types.vec2_mt.from
types.vec3 = types.vec3_mt.new
types.vec3From = types.vec3_mt.from
types.mat3 = types.mat3_mt.new

return types


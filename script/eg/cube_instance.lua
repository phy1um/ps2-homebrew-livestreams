local LOG = require"p2g.log"
local M = require"p2g.math"
local DRAW = require"p2g.draw2d"

local function scale(m, n) 
  m:set(0,0, n)
  m:set(1,1, n)
  m:set(2,2, n)
  m:set(3,3, 1)
  return m
end

local function translate(m, ox, oy, oz)
  m:set(3,0,ox*10)
  m:set(3,1,oy*10)
  m:set(3,2,oz*10)
  return m
end

local function rotate_x(m, rx)
  m:set(1,1, math.cos(-rx))
  m:set(1,2, -math.sin(-rx))
  m:set(2,1, math.sin(-rx))
  m:set(2,2, math.cos(-rx))
  return m
end

local function rotate_y(m, ry)
  m:set(0,0, math.cos(-ry))
  m:set(0,2, math.sin(-ry))
  m:set(2,0, -math.sin(-ry))
  m:set(2,2, math.cos(-ry))
  return m
end

local Instance = {
  pos = nil,
  rot_x = nil,
  rot_y = nil,
  scale = 1,
  mesh = nil,
  verts = 0,
  vert_size = 0,
}

function Instance.new(mesh, verts, vert_size)
  return setmetatable({
    pos = M.vec3(0,0,0),
    rot_x = 0,
    rot_y = 0,
    scale = 1,
    mesh = mesh,
    verts = verts,
    vert_size = vert_size
  }, {__index=Instance})
end

function Instance:move_to(x,y,z)
  self.pos:set(x,y,z)
end

function Instance:step(dx,dy,dz)
  self.pos.x = self.pos.x + dx
  self.pos.y = self.pos.y + dy
  self.pos.z = self.pos.z + dz
end

function Instance:rotate(rx, ry)
  self.rot_x = self.rot_x + rx
  self.rot_y = self.rot_y + ry
end

function Instance:set_scale(s)
  self.scale = s
end

function Instance:model_matrix()
  local m = M.mat4() 
  m:mul(translate(M.mat4(), self.pos.x, self.pos.y, self.pos.z))
  m:mul(rotate_x(M.mat4(), self.rot_x))
  m:mul(rotate_y(M.mat4(), self.rot_y))
  m:mul(scale(M.mat4(), self.scale))
  return m
end

function Instance.draw_all(instances, cam)
  for i, inst in ipairs(instances) do
    local mvp = M.mat4()
    mvp:mul(cam:projection_matrix())
    mvp:mul(cam:view_matrix())
    mvp:mul(inst:model_matrix())
    LOG.trace(":: " .. tostring(mvp))
    --LOG.trace("@ " .. tostring(cam.pos))
    local mo = DRAW:mesh_cnt(inst.mesh, inst.verts, inst.vert_size)
    DRAW:ee_transform(mvp.buf, mo, inst.verts, inst.vert_size, 4)
  end 
end

return Instance


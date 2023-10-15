local LOG = require"p2g.log"
local M = require"p2g.math"

local C = {
  pos = M.vec3(),
  facing = M.vec3(),
  aspect = 1,
  near = 0.1,
  far = 100,
  fov = 0.2,
  _view_matrix = M.mat4(),
  _proj_matrix = M.mat4(),
  view_dirty = false,
  proj_dirty = false,
}

function C.new(x,y,z,opts) 
  local camera = {
    pos = M.vec3(x,y,z),  
    facing = M.vec3(0,0,-1),
    aspect = opts.aspect or 4/3,
    fov = opts.fov or 0.7,
    near = opts.near or 0.1,
    far = opts.far or 100,
    _view_matrix = M.mat4(),
    _proj_matrix = M.mat4(),
    -- true when these matricies require recalculation
    view_dirty = true,
    proj_dirty = true,
  }
  return setmetatable(camera, { __index = C })
end

function C:set_fov(fov) 
  self.fov = fov
  self.proj_dirty = true
end

function C:set_aspect(ar)
  self.aspect = ar
  self.proj_dirty = true
end

function C:move_to(x,y,z)
  self.pos:set(x,y,z) 
  self.view_dirty = true
end

function C:step(dx,dy,dz)
  self.pos.x = self.pos.x + dx
  self.pos.y = self.pos.y + dy
  self.pos.z = self.pos.z + dz
  self.view_dirty = true
end

function C:look_at(x,y,z)
  self.facing:set(x - self.pos.x, y-self.pos.y, z-self.pos.z)
  self.facing:normalize()
  self.view_dirty = true
end

function C:view_matrix()
  if self.view_dirty == true then
    -- TODO: lookat
    self._view_matrix:identity()
    self._view_matrix:set(3,0,-self.pos.x)
    self._view_matrix:set(3,1,-self.pos.y)
    self._view_matrix:set(3,2,-self.pos.z)
    self.view_dirty = false
  end
  return self._view_matrix
end

function C:projection_matrix()
  if self.proj_dirty then
    self._proj_matrix:identity()
    local near = self.near
    local far = self.far
    local top = near*math.tan(self.fov/2)
    local right = top*self.aspect
    self._proj_matrix:set(0, 0, near/right)
    self._proj_matrix:set(1, 1, near/top)
    self._proj_matrix:set(2, 2, (far+near)/(far-near))
    self._proj_matrix:set(2, 3, (-2*far*near)/(far - near))
    self._proj_matrix:set(3, 2, 1)
  end
  return self._proj_matrix
end

return C


local GIF = require"p2g.gif"
local P = require"p2g.const"
local DRAW = require"p2g.draw2d"
local VRAM = require"p2g.vram"
local DMA = require"p2g.dma"
local GS = require"p2g.gs"
local RM = require"p2g.buffer"
local LOG = require"p2g.log"
local M = require"p2g.math"
local IO = require"p2g.io"
local PAD = require"p2g.pad"
local CAMERA = require"p2g.camera"
local I = require"eg.cube_instance"

local rx = 0
local ry = 0
local cam = CAMERA.new(0,0,18, {
  fov = math.pi/4,
  aspect = 3/4,
  near = 0.1,
  far = 10,
})

local instances = {}


function PS2PROG.start()
  PS2PROG.logLevel(LOG.infoLevel)
  DMA.init(DMA.GIF)
  GS.setOutput(640, 448, GS.INTERLACED, GS.NTSC)
  local fb1 = VRAM.mem:framebuffer(640, 448, GS.PSM32, 2048)
  local fb2 = VRAM.mem:framebuffer(640, 448, GS.PSM32, 2048)
  local zb = VRAM.mem:framebuffer(640, 448, GS.PSMZ16, 2048)
  GS.setBuffers(fb1, fb2, zb)
  DRAW:screenDimensions(640, 448)
  DRAW:clearColour(0x2b, 0x2b, 0x2b)
  local db = RM.alloc(200 * 1024)
  DRAW:bindBuffer(db)
  
  local sz = IO.file_size("cube.bin")
  cube_model = RM.alloc(sz)
  IO.read_file("cube.bin", 0, sz, cube_model)

  local putcube = function(x,y,z,s,rx,ry)
    local i = I.new(cube_model, 12*3, 8*4)
    i:move_to(x,y,z)
    i:rotate(rx, ry)
    i:set_scale(s)
    table.insert(instances, i)
  end

  putcube(0, 0, -15, 30, 0, 0)
  --[[
  putcube(0, 0, -3, 20, 0.1, 0.2)
  putcube(-14, 0, -15, 3, 0, 0)
  putcube(0, 20, -13, 18, 0.4, 0.4)
  ]]

end

local reload_debounce = 0

function PS2PROG.frame()
  local dt = 1/30
  local cam_dx = PAD.axis(PAD.axisLeftX)
  local cam_dy = PAD.axis(PAD.axisLeftY)
  if PAD.held(PAD.L1) then
    instances[1]:step(cam_dx*30*dt, cam_dy*30*dt, 0)
  else
    cam:step(cam_dx*10*dt, 0, cam_dy*10*dt)
  end

  if reload_debounce > 0 then reload_debounce = reload_debounce - 1 end

  if PAD.held(PAD.SELECT) and reload_debounce <= 0 then
    local lv = PS2PROG.get_log_level()
    PS2PROG.logLevel(LOG.traceLevel)
    reload("p2g.camera")
    reload("eg.cube_instance")
    reload_debounce = 6
    PS2PROG.logLevel(lv)
  end

  if PAD.held(PAD.X) and reload_debounce <= 0 then
    LOG.info(string.format("camera @ {%f, %f, %f}", cam.pos.x, cam.pos.y, cam.pos.z))
    LOG.info(string.format("cube R {%f, %f, 0}", instances[1].rot_x, instances[1].rot_y))
    PS2PROG.logLevel(LOG.traceLevel)
    reload_rebounce = 6
  end

  local drot_x = 0
  local drot_y = 0
  if PAD.held(PAD.LEFT) then
    drot_y = 0.09
  elseif PAD.held(PAD.RIGHT) then
    drot_y = -0.09
  end
  if PAD.held(PAD.UP) then
    drot_x = 0.09
  elseif PAD.held(PAD.DOWN) then
    drot_x = -0.09
  end

  instances[1]:rotate(drot_x, drot_y)

  DRAW:frameStart()
  I.draw_all(instances, cam)
  DRAW:frameEnd()
end



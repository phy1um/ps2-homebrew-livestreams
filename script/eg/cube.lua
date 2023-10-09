
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

local geom_buffer = RM.alloc(1024)
local mvp = M.mat4()
local working_m4 = {M.mat4(), M.mat4(), M.mat4()}

local rx = 0
local ry = 0

local function scale(m, n) 
  m[0] = n
  m[5] = n
  m[10] = n
  m[15] = n
end

local function translate(m, ox, oy)
  m[3] = ox
  m[7] = oy
end

local function rotate_x(m, rx)
  m[5] = math.cos(-rx)
  m[6] = -math.sin(-rx)
  m[9] = math.sin(-rx)
  m[10]= math.cos(-rx)
end

local function rotate_y(m, ry)
  m[0] = math.cos(-ry)
  m[2] = math.sin(-ry)
  m[8] = -math.sin(-ry)
  m[10] = math.cos(-ry)
end

function PS2PROG.start()
  PS2PROG.logLevel(LOG.infoLevel)
  DMA.init(DMA.GIF)
  GS.setOutput(640, 448, GS.INTERLACED, GS.NTSC)
  local fb1 = VRAM.mem:framebuffer(640, 448, GS.PSM24, 2048)
  local fb2 = VRAM.mem:framebuffer(640, 448, GS.PSM24, 2048)
  local zb = VRAM.mem:framebuffer(640, 448, GS.PSMZ24, 2048)
  GS.setBuffers(fb1, fb2, zb)
  DRAW:screenDimensions(640, 448)
  DRAW:clearColour(0x2b, 0x2b, 0x2b)
  local db = RM.alloc(200 * 1024)
  DRAW:bindBuffer(db)
  
  local sz = IO.file_size("cube.bin")
  cube_model = RM.alloc(sz)
  IO.read_file("cube.bin", 0, sz, cube_model)

end

function PS2PROG.frame()
  working_m4[1]:identity()
  working_m4[2]:identity()
  working_m4[3]:identity()
  rotate_x(working_m4[1], rx)
  rotate_y(working_m4[2], ry)
  scale(working_m4[3], 100)
  mvp:identity()
  mvp:mul(working_m4[1])
  mvp:mul(working_m4[2])
  mvp:mul(working_m4[3])

  if PAD.held(PAD.LEFT) then 
    ry = ry+0.01
  elseif PAD.held(PAD.RIGHT) then
    ry = ry-0.01
  end

  if PAD.held(PAD.UP) then 
    rx = rx+0.01
  elseif PAD.held(PAD.DOWN) then
    rx = rx-0.01
  end

  LOG.trace("matrix: " .. tostring(mvp))
  LOG.trace("matrix buf: " .. tostring(mvp.buf))
  DRAW:frameStart()
  local mo = DRAW:mesh_cnt(cube_model, 12*3, 8*4)
  DRAW:ee_transform(mvp.buf, mo, 12*3, 8*4, 4)
  DRAW:frameEnd()
end




local GIF = require("gif")
local P = require("ps2const")
local D2D = require("draw2d")
local VRAM = require("vram")
local M = require("ps2math")

local gs = nil

function tests()
  local a = M.mat3()
  assert(a[0] == 1)
  assert(a[1] == 0)
  assert(a[3] == 0)
  assert(a[4] == 1)
  local b = M.mat3({ {1,2,3},{0,0,0},{0,0,0} })
  M.mat3_mt.add(a, b)
  local c = M.mat3({ {2,2,3},{0,1,0},{0,0,1} })
  assert(a:equal(c))
  assert(a[2] == a:get(2, 0))
  assert(a[8] == a:get(2,2))
  local ii = M.mat3()
  local x = M.mat3()
  x:copy(a)
  x:mul(ii)
  assert(a:equal(x))

  local ta = M.mat3({ {5,12,9},{2,4.2,1},{11,8,0.2} })
  local tb = M.mat3({ {7,12,122},{0.1,17,8},{5,100,-2} })
  local tres = M.mat3({ {81.2,1164,688},{19.42,195.4,275.6},{78.8,288,1405.6} })
  ta:mul(tb)
  assert(ta:equal(tres))

  local va = M.vec3(5,12,7.4)
  local vb = M.vec3From(va)
  ii:apply(va)
  assert(va:equal(vb))

  local ra = M.vec3(12, 8, 1)

  local rx = M.vec3(10.17144 + 10, 10.22456 + 4, 1)
  local rm = M.mat3({ {math.cos(0.2), -1 * math.sin(0.2), 10}, {math.sin(0.2), math.cos(0.2), 4}, {0,0,1} })
  -- local rm = M.mat3({ {math.cos(0.2), math.sin(0.2), 0}, {-1 * math.sin(0.2), math.cos(0.2), 0}, {10, 4, 1} })
  rm:apply(ra)
  print(ra:__string() .. " -> " .. rx:__string())
  assert(ra:equal(rx))
end

function PS2PROG.start()
  PS2PROG.logLevel(5)
  DMA.init(DMA.GIF)
  gs = GS.setOutput(640, 448, GS.INTERLACED, GS.NTSC)
  local fb1 = VRAM.mem:framebuffer(640, 448, GS.PSM24, 2048)
  local fb2 = VRAM.mem:framebuffer(640, 448, GS.PSM24, 2048)
  local zb = VRAM.mem:framebuffer(640, 448, GS.PSMZ24, 2048)
  GS.setBuffers(fb1, fb2, zb)
  D2D:screenDimensions(640, 448)
  D2D:clearColour(0x2b, 0x2b, 0x2b)
  local db = RM.alloc(200 * 1024)
  D2D:bindBuffer(db)
end

local done = false
local pos = M.vec2(100, 100)
local angle = 0
local fwd = M.vec2(1, 0)
local work = M.vec2(0, 0)
local w2 = M.vec2(0,0)
local rv = 0.07
local v = 1.6

local TAU = 2 * math.pi


function stepPlayer()
  if PAD.held(PAD.LEFT) then
    angle = angle - rv
  elseif PAD.held(PAD.RIGHT) then
    angle = angle + rv
  end
  local velocity = 0
  if PAD.held(PAD.UP) then velocity = v end
  if angle > TAU then angle = angle - TAU end
  if angle < 0 then angle = angle + TAU end
  MATH_C_LIB.copyVec2(work.buf, fwd.buf)
  work:rotate(angle)
  work:scale(velocity)
  pos:add(work)
end

function PS2PROG.frame()
  if not done then
    done = true

    local v1 = M.vec2(5, 4)
    local v2 = M.vec2(7, 7)
    local perp = M.vec2(-11, 12)
    v1:add(v2)
    local l = v1:length()
    local d = v1:dot(v2)
    LOG.info("vector add result: " .. tostring(v1))
    LOG.info("len = " .. l .. ", dot = " .. d)
    LOG.info("zero = " .. v1:dot(perp))
    v1:copyRotate(1)
    tests()
  end
  D2D:frameStart(gs)
  stepPlayer()
  D2D:setColour(255,0,0,0x80)
  D2D:rect(pos.x, pos.y, 20, 20)
  MATH_C_LIB.copyVec2(w2.buf, fwd.buf)
  w2:rotate(angle)
  w2:scale(60)
  D2D:setColour(0, 255, 255, 0x80)
  D2D:rect(pos.x + w2.x + 10, pos.y + w2.y + 10, 4, 4)
  D2D:frameEnd(gs)
end



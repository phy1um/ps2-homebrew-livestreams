-- hello world

local GIF = require"p2g.gif"
local P = require"p2g.const"
local D2D = require"p2g.draw2d"
local VRAM = require"p2g.vram"
local DMA = require"p2g.dma"
local GS = require"p2g.gs"
local RM = require"p2g.buffer"
local LOG = require"p2g.log"
local FONT = require"p2g.font"
local IO = require"p2g.io"

-- https://stackoverflow.com/questions/1426954/split-string-in-lua
local function mysplit (inputstr, sep)
        if sep == nil then
                sep = "%s"
        end
        local t={}
        for str in string.gmatch(inputstr, "([^"..sep.."]+)") do
                table.insert(t, str)
        end
        return t
end


function read_chars(buf, offset)
  if offset > buf.size then
    error("out of bounds: " .. offset)
  end
  local v = buf:read(offset) 
  local b0 = v & 0xff
  local b1 = (v >> 8)&0xff
  local b2 = (v >> 16)&0xff
  local b3 = (v >> 24)&0xff
  return string.char(b0,b1,b2,b3)
end

function read_n(buf, offset, n)
  local b = {}
  for i=0,n do
    local c4 = read_chars(buf, (i*4)+offset) 
    table.insert(b, c4)
  end
  return table.concat(b, "")
end

function PS2PROG.start()
  PS2PROG.logLevel(LOG.debugLevel)
  DMA.init(DMA.GIF)
  GS.setOutput(640, 448, GS.INTERLACED, GS.NTSC)
  local fb1 = VRAM.mem:framebuffer(640, 448, GS.PSM24, 2048)
  local fb2 = VRAM.mem:framebuffer(640, 448, GS.PSM24, 2048)
  local zb = VRAM.mem:framebuffer(640, 448, GS.PSMZ24, 2048)
  GS.setBuffers(fb1, fb2, zb)
  D2D:screenDimensions(640, 448)
  D2D:clearColour(0x2b, 0x2b, 0x2b)
  local db = RM.alloc(200 * 1024)
  D2D:bindBuffer(db)

  local fontTexture = D2D.loadTexture("bigfont.tga", 256, 64)
  font = FONT.new(fontTexture, 8, 16)

  vram = VRAM.slice(VRAM.mem.head)

  size = IO.file_size("script/eg/io.lua")
  LOG.info("file size = " .. size)
  local file_contents = RM.alloc(size)
  LOG.info("allocated buffer, reading file")
  IO.read_file("script/eg/io.lua", 0, size, file_contents)
  result = mysplit(read_n(file_contents, 0, math.floor(size/4)), "\n")
end

function PS2PROG.frame()
  D2D:frameStart()
  font:loadToVram(vram)
  D2D:setColour(255,255,255,0x80)
  font:printLines(10, 10, table.unpack(result))
  D2D:frameEnd()
end



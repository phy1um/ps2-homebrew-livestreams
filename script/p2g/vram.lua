local LOG = require"p2g.log"
local GS = require"p2g.gs"

local vram = {}
local basePtr = 0
-- maximum is 4mb
local max = math.floor(4 * 1024 * 1024)
vram.max = max

local vramslice = {
  start= 0,
  head = 0,
  tail = max,
}

function vramslice:alloc(b, align)
  local out = self.head 
  --out = out + align - (out%align)
  while out % align ~= 0 do out = out + 1 end
  if out + b >= max then
    LOG.error("vram buffer overflow")
    error("vram buffer overflow")
  end
  self.head = out + b
  LOG.trace("vram alloc: " .. out .. " base -> " .. basePtr)
  return out
end

function vramslice:framebuffer(w, h, psm, align)
  -- NOTE: this size has to be in words rather than bytes for some reason
  local sz = vram.size(w, h, psm, align)/4
  local fb = {
    address = self:alloc(sz, align),
    width = w,
    height = h,
    format = psm,
  }
  LOG.debug("VRAM framebuffer -- @ " .. fb.address .. ", size = " .. sz)
  return fb
end

function vramslice:texture(tex)
  --local size = vram.size(tex.width, tex.height, tex.psm, 256)
  local size = tex.width*tex.height*4
  tex.basePtr = self:alloc(size, 256)
  LOG.debug("allocated texture in VRAM: " .. tex.fname .. " @ " .. tex.basePtr)
  return tex
end

function vram.slice(start, tail)
  if tail == nil then tail = max end
  return setmetatable({
      start=start,
      head=start,
      tail=tail
    }, 
    { __index=vramslice }
  )
end

function vram.size(w, h, psm, align)
  local size = w*h
  if psm == GS.PSM32 or psm == GS.PSM24 then
    size = w*h*4
  elseif psm == GS.PSM16 or psm == GS.PSM16S or psm == GS.PSMZ16 or psm == GS.PSMZ16S then
    size = w*h*2
  elseif psm == GS.PSM4 then 
    size = math.floor(w*h*0.5) 
  end

  LOG.trace("VRAM sizeof " .. w .. ", " .. h .. " @" .. psm .. " :: " .. size)

  return size
end

vram.mem = vram.slice(0, max)

return vram

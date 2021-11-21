
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
  out = out + align - (out%align)
  if out + b >= max then
    print("vram buffer overflow")
    error("vram buffer overflow")
  end
  self.head = out + b
  -- print("vram alloc: " .. out .. " base -> " .. basePtr)
  return out
end

function vramslice:framebuffer(w, h, psm, align)
  local sz = vram.size(w, h, psm, align)
  return {
    address = self:alloc(sz, align),
    width = w,
    height = h,
    format = psm,
  }
end

function vramslice:texture(tex)
  --local size = vram.size(tex.width, tex.height, tex.psm, 256)
  local size = tex.width*tex.height*4
  tex.basePtr = self:alloc(size, 256)
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
--  if w%align ~= 0 then
--    w = vpa(w, 64) 
--  end
  local size = w*h
  if psm == GS.PSM16 or psm == GS.PSM16S or psm == GS.PSMZ16 or psm == GS.PSMZ16S then
    math.floor(w*h*0.5)
  elseif psm == GS.PSM8 then size = math.floor(w*h*2^-2)
  elseif psm == GS.PSM4 then size = math.floor(w*h*2^-3) 
  end

  return size + align - (size%align)
end

vram.mem = vram.slice(0, max)

return vram

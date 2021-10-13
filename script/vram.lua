
local vram = {}
local basePtr = 0

function vpa(v, a)
  return v + a - (v%a)
end

function vram.alloc(b, align)
  local out = basePtr
  out = out + align - (out%align)
  basePtr = out + b
  print("vram alloc: " .. out .. " base -> " .. basePtr)
  return out
end

function vram.size(w, h, psm, align)
  w = vpa(w, 64) 
  local size = w*h
  if psm == GS.PSM16 or psm == GS.PSM16S or psm == GS.PSMZ16 or psm == GS.PSMZ16S then
    math.floor(width*height*0.5)
  elseif psm == GS.PSM8 then size = math.floor(width*height*2^-2)
  elseif psm == GS.PSM4 then size = math.floor(width*height*2^-3) 
  end

  return vpa(size, align)
end

function vram.buffer(w, h, psm, align)
  local sz = vram.size(w, h, psm, align)
  return {
    address = vram.alloc(sz, align),
    width = w,
    height = h,
    format = psm,
  }
end

function vram.textureSize(w,h,psm)
  error("not implemented")
end

return vram

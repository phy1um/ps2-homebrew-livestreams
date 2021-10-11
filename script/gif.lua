
local gif = {
  PACKED = 0x0,
  REGLIST = 0x01 * 2^26,
  IMAGE  = 0x10 * 2^26,
}

function gif.tag(b, flg, nloop, eop, regs) 
  local lpp = b.head
  if #regs == 0 then return end
  -- print("GIFTAG: pushing " .. string.format("0x%x", nloop))
  if eop then
    b:pushint(nloop + 0x8000)
  else
    b:pushint(nloop)
  end
  local nreg = #regs
  if nreg > 16 then error("invalid gif tag: nreg = " .. #regs) end
  if nreg == 16 then nreg = 0 end
  local w2 = (nreg * 2^28) + flg
  -- print("GIFTAG: pushing " .. string.format("0x%x", w2))
  b:pushint(w2)
  local reg = 0
  local regc = 0
  local max = 1

  -- write the registers
  for i=1,#regs,1 do
    reg = reg + regs[i] * 2^(4*regc)
    regc = regc + 1
    if regc >=8 then
      b:pushint(reg)
      -- print("GIFTAG: pushing regword " .. string.format("0x%x", reg))
      regc = 0
      max = max - 1
    end
  end

  -- pad out the rest
  for i=max,0,-1 do
    b:pushint(reg)
    -- print("GIFTAG: pushing regword " .. string.format("0x%x", reg))
    reg = 0
  end

  return lpp
end

function gif.setAd(b, reg, v1, v2)
  b:pushint(v1)
  b:pushint(v2)
  b:pushint(reg)
  b:pushint(0)
end

function gif.primAd(b, primType, shaded, textured, aa)
  local bits = primType
  if shaded then bits = bits + 0x4 end
  if textured then bits = bits + 0x8 end
  if aa then bits = bits + 0x40 end
  gif.setAd(b, 0x00, bits, 0)
end

function gif.packedRGBAQ(bu, r, g, b, a)
  bu:pushint(r)
  bu:pushint(g)
  bu:pushint(b)
  bu:pushint(a)
end

function gif.packedXYZ2(b, x, y, z)
  b:pushint(x)
  b:pushint(y)
  b:pushint(z)
  b:pushint(0)
end

return gif

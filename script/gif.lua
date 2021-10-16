local P = require("ps2const")

local gif = {
  PACKED = 0x0,
  REGLIST = 0x01 * 2^26,
  IMAGE  = math.floor(2^27),
  BLOCKSIZE = 0x7FF,
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
  local w2 = (math.floor(nreg * 2^28) + flg)
  --print("GIFTAG: pushing " .. string.format("0x%x", w2) .. " :: " .. w2)
  --print("GIFTAG: flag = " .. flg)
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

function gif.bitBltBuf(b, dba, dbw, psm)
  gif.setAd(b, P.REG.BITBLTBUF, 0, dba + (dbw*2^16) + (psm*2^24))
end

function gif.trxPos(b, sx, sy, dx, dy, dir)
  gif.setAd(b, P.REG.TRXPOS, sx + (sy*2^16), dx + (dy*2^16) + (dir*2^27))
end

function gif.trxReg(b, w, h)
  gif.setAd(b, P.REG.TRXREG, w, h)
end

function gif.trxDir(b, dir)
  gif.setAd(b, P.REG.TRXDIR, dir, 0)
end

function gif.tex1(b, lcm, mxl, mtba, l, k)
  local v1 = mxl * 4
  if lcm then v1 = v1 + 1 end
  if mtba then v1 = v1 + 2^8 end
  v1 = v1 + math.floor(l * (2^18))
  gif.setAd(b, P.REG.TEX1, v1, k)
end

function gif.texA(b, a0, a1)
  gif.setAd(b, P.REG.TEXA, a0, a1)
end

function gif.tex2(b, v)
  gif.setAd(b, P.REG.TEX2, v, 0)
end

function gif.mipTbp1(b, ctx, p1, w1, p2, w2, p3, w3)
  b:setMipTbp(p1, w1, p2, w2, p3, w3)
  b:pushint(P.REG.MIPTBP1 + ctx) 
  b:pushint(0)
end

function gif.mipTbp2(b, ctx, p1, w1, p2, w2, p3, w3)
  b:setMipTbp(p1, w1, p2, w2, p3, w3)
  b:pushint(P.REG.MIPTBP2 + ctx) 
  b:pushint(0)
end

function gif.primAd(b, primType, shaded, textured, aa)
  local bits = primType
  if shaded then bits = bits + 0x8 end
  if textured then bits = bits + 0x10 end
  if aa then bits = bits + 0x80 end
  gif.setAd(b, P.REG.PRIM, bits, 0)
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

function gif.packedUV(b, u, v)
  local vv = u + math.floor(v * 2^16)
  print("u,v -> " .. vv)
  b:pushint(vv)
  b:pushint(0)
  b:pushint(0)
  b:pushint(0)
end

function gif.packedST(b, s, t)
  b:pushfloat(s)
  b:pushfloat(t)
  b:pushfloat(1.0)
  b:pushint(0)
end

function gif.texflush(b)
  gif.tag(b, gif.PACKED, 1, true, {0xe})
  gif.setAd(b, P.REG.TEXFLUSH, 0, 0)
end

return gif

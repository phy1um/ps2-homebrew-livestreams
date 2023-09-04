
local IO = require"p2g.io"
local LOG = require"p2g.log"
local TGA = P2GCORE["tga"]

function TGA.from_file(file_name, alloc)
  LOG.trace("loading texture " .. file_name)
  local tga_header = alloc(TGA.HEADER_SIZE)
  IO.read_file(file_name, 0, TGA.HEADER_SIZE, tga_header)
  local w = TGA.get_header_field(tga_header, "width")
  local h = TGA.get_header_field(tga_header, "height")
  local bps = TGA.get_header_field(tga_header, "bps")
  local bpp = math.floor(bps/8)
  local size = w*h*bpp
  if bps == 4 then size = w*h*0.5 end
  LOG.debug(string.format("TGA header: %d x %d @ %d (%d bytes)", w, h, bps, size))
  local tga_body = alloc(size)
  IO.read_file(file_name, TGA.HEADER_SIZE, size, tga_body)
  local texture = {
    width = w,
    height = h,
    data = tga_body,
    format = TGA.BPS_TO_PSM[bps],
    fname = file_name,
  }
  if bps == 32 then
    TGA.swizzle32(texture)
  elseif bps == 24 then
    TGA.swizzle24(texture)
  elseif bps == 16 then
    TGA.swizzle16(texture)
  end
  return texture, tga_header
end

return TGA

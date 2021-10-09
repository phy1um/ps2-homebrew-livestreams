
local gs = nil

function PS2PROG.start()
  DMA.init(DMA.GIF)
  gs = GS.newState(640, 448, GS.INTERLACED, GS.NTSC)
  local fb = gs:alloc(640, 448, GS.PSM24)
  local zb = gs:alloc(640, 448, GS.PSMZ24)
  gs:setBuffers(fb, zb)
end

function PS2PROG.onframe()
  local db = RM.getDrawBuffer(10000)
  db:frameStart(gs)
  db:frameEnd(gs)
  DMA.sendBufferNormal(db, DMA.GIF)
  db:free()
end



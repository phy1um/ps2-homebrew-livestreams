
local gs = nil

function PS2PROG.start()
  DMA.init(DMA.GIF)
  gs = GS.newState(640, 448, GS.INTERLACED, GS.NTSC)
  local fb = gs:alloc(640, 448, GS.PSM24)
  local zb = gs:alloc(640, 448, GS.PSMZ24)
  gs:setBuffers(fb, zb)
  gs:clearColour(255, 0, 0)
end

function PS2PROG.frame()
  local db = RM.getDrawBuffer(10000)
  db:frameStart(gs)
  db:frameEnd(gs)
  DMA.send(db, DMA.GIF)
  db:free()
end



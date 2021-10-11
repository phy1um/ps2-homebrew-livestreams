local gif = dofile("host:script/gif.lua")

local gs = nil

function writeToBuffer(b, ints)
  print("writing " .. #ints .. " ints")
  for i=1,#ints,1 do
    b:pushint(ints[i])
  end
end

function PS2PROG.start()
  DMA.init(DMA.GIF)
  gs = GS.newState(640, 448, GS.INTERLACED, GS.NTSC)
  local fb = gs:alloc(640, 448, GS.PSM24)
  local zb = gs:alloc(640, 448, GS.PSMZ24)
  gs:setBuffers(fb, zb)
  gs:clearColour(0, 255, 0)
end

function PS2PROG.frame()
  local db = RM.getDrawBuffer(10000)
  db:frameStart(gs)
  -- writeToBuffer(db, {0x8001, 0x10000000, 0xe, 0x0, 0x1, 0x0, 0x61, 0x0})
  db:frameEnd(gs)
  DMA.send(db, DMA.GIF)
  db:free()
end



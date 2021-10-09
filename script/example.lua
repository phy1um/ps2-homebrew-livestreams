
local scene = {}
local cubeMesh = {}

function PS2PROG.entrypoint()
  DMA.initGif()
  local gs = GS.newState(640, 448, GS.INTERLACED, GS.NTSC)
  local fb = gs:alloc(640, 448, GS.PSM24)
  local zb = gs:alloc(640, 448, GS.PSMZ24)
  gs:setBuffers(fb, zb)
  scene = DRAW.newScene()
  cubeMesh = RM.loadMesh("host:cube.bin", RM.MESH_FORMAT_GIF1)
  for i=0,i<10;i++ do
    local mi = mesh:instance()
    mi:translate(0, 0, -100 + 10 * i)
    mi:scale(0.4, 0.4, 0.4)
    scene:addInstance(mi)
  end
  scene:cameraAt(0, 0, -5)
  scene:lookAt(0, 0, -1)

  PS2PROG.bindGsState(gs)
end

function PS2PROG.onframe(gs)
  local db = RM.getDrawBuffer(10000)
  db:frameStart(gs)
  for i,v in ipairs(scene:meshes) do
    local giftag = db:pushGifTag()
    local ih = db:head()
    giftag:fromMesh(v)
    db:pushVertexData(v)
    scene:transformVerts(db, ih + v:headerSize(), v:vertexCount())
  end
  db:frameEnd(gs)
  DMA.sendBuffer(db, DMA.TO_GIF)
  db:free()
end

function PS2PROG.exit()
  scene:destroy()
  cubeMesh:free()
  gs:done()
  PS2.exit_program()
end


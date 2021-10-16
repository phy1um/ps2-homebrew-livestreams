
(print "running main fennel file")

(local D2D (require "draw2d"))
(local VRAM (require "vram"))

(local game (require "game"))

(print "loading utils")
(local T (require "text"))
(local E (require "events"))
(local PLAYER (require "player"))

(var gs nil)
(var font nil)

(var state {})

(var counter 0)
(fn get-events [] 
  (let [evs []]
    (if (= true (PAD.held PAD.UP)) (table.insert evs E.up))
    (if (= true (PAD.held PAD.DOWN)) (table.insert evs E.down))
    (if (= true (PAD.held PAD.LEFT)) (table.insert evs E.left))
    (if (= true (PAD.held PAD.RIGHT)) (table.insert evs E.right))
    evs))


(print "overriding start function")
(fn PS2PROG.start []
  (set font (D2D.loadTexture "host:bigfont.tga" 256 64))
  (set T.font font)
  (DMA.init DMA.GIF)
  (set gs (GS.newState 640 448 GS.INTERLACED GS.NTSC))
  (let [fb (VRAM.buffer 640 448 GS.PSM24 256)
        zb (VRAM.buffer 640 448 GS.PSMZ24 256)]
    (gs:setBuffers fb zb)
    (gs:clearColour 0x2b 0x2b 0x2b))
  (set state (game.new-state))
  (print "creating entity")
  (PLAYER.new state -20 -50 255 0 0 E.right)
  (PLAYER.new state -200 -10 0 255 0 E.right)
  (PLAYER.new state 300 -10 100 40 100 E.left))

(global *dt* (/ 1 60))
(print "overriding on-frame handler")
(fn PS2PROG.frame []
  (local events (get-events))
  (set state (game.update *dt* state events))
  (D2D:newBuffer)
  (let [db (D2D:getBuffer)]
    (db:frameStart gs)
    (D2D:setColour 0xff 0xff 0xff 0x80)
    (game.draw state)
    (D2D:setColour 255 255 255 0x80)
    (T.printLines -300 -220 (.. "tris/frame=" D2D.rawtri ", KC=" (+ D2D.kc 1)))
    (let [db (D2D:getBuffer)]
      (db:frameEnd gs)
      (D2D:kick)))
  (set D2D.rawtri 0)
  (set D2D.kc 0)
  (set state.colliders [])
  (set counter (+ counter 1)))

(print "ended main fennel file")

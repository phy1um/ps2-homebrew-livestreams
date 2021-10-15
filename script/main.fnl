
(print "running main fennel file")

(local D2D (require "draw2d"))
(local VRAM (require "vram"))

(local game (require "game"))

(print "loading utils")
(local T (require "text"))
(local E (require "events"))

(var gs nil)
(var font nil)

(var state {})

(var counter 0)
(fn get-events []
  (if (> counter 60) [E.left] [E.up]))

(fn create-thing [state x y r g b d] 
  (state:new-entity  
    (fn [] { : x : y :w 24 :h 24 :col {: r : g : b} :dir d :v 50})
    (fn [me dt state events]
            (if (= me.dir E.up) (set me.y (- me.y (* dt me.v)))
                (= me.dir E.down) (set me.y (+ me.y (* dt me.v)))
                (= me.dir E.left) (set me.x (- me.x (* dt me.v)))
                (= me.dir E.right) (set me.x (+ me.x (* dt me.v)))
                nil))
    (fn [me]
      (D2D:setColour me.col.r me.col.g me.col.b 0x80)
      (D2D:rect me.x me.y me.w me.h))))


(print "overriding start function")
(fn PS2PROG.start []
  (set font (D2D.loadTexture "host:bigfont.tga" 256 64))
  (DMA.init DMA.GIF)
  (set gs (GS.newState 640 448 GS.INTERLACED GS.NTSC))
  (let [fb (VRAM.buffer 640 448 GS.PSM24 256)
        zb (VRAM.buffer 640 448 GS.PSMZ24 256)]
    (gs:setBuffers fb zb)
    (gs:clearColour 0x2b 0x2b 0x2b))
  (set state (game.new-state))
  (print "creating entity")
  (create-thing state -20 -50 255 0 0 E.right)
  (create-thing state -200 -10 0 255 0 E.down)
  (create-thing state 300 100 100 40 100 E.left))

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
    (let [db (D2D:getBuffer)]
      (db:frameEnd gs)
      (D2D:kick)))
  (print "tri/frame = " D2D.rawtri ", KC = " D2D.kc)
  (set D2D.rawtri 0)
  (set D2D.kc 0)
  (set counter (+ counter 1)))

(print "ended main fennel file")

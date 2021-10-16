
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

(fn point-in-rect [ax ay x y w h]
  (and
    (>= ax x)
    (<= ax (+ x w))
    (>= ay y)
    (<= ay (+ y h))))

(fn collider-rect [id x y w h onhit]
  (fn [other]
    (if (= other.id id) {:hit false}
        (or
          (point-in-rect other.x other.y x y w h)
          (point-in-rect (+ other.x other.w) other.y x y w h)
          (point-in-rect other.x (+ other.y other.h) x y w h)
          (point-in-rect (+ other.x other.w) (+ other.y other.h) x y w h))
      {:hit true :action onhit}
      {:hit false})))

(fn create-thing [state x y r g b d] 
  (state:new-entity  
    (fn [] 
      { : x : y :w 24 :h 24 
       :col {: r : g : b} :dir d 
       :v 50 :dx 0 :dy 0 })
    (fn [me dt state events]
      (let [px me.x py me.y]
        (if (= me.dir E.up) (set me.dy (* -1 dt me.v))
            (= me.dir E.down) (set me.dy (* dt me.v))
            (= me.dir E.left) (set me.dx (* -1 dt me.v))
            (= me.dir E.right) (set me.dx (* dt me.v))
            nil)
        (set me.x (+ me.x me.dx))
        (set me.y (+ me.y me.dy))
        (state:add-col (collider-rect 
                         me.id
                         me.x 
                         me.y 
                         me.w 
                         me.h 
                         (fn [other]
                           (let [cdx (- me.x other.x)
                                 cdy (- me.y other.y)]
                           (set me.x (+ py (* dt 2 cdx)))
                           (set me.y (+ py (* dt 2 cdy)))))))))
            (fn [me]
      (D2D:setColour me.col.r me.col.g me.col.b 0x80)
      (T.printLines me.x (- me.y 30) (.. me.dx ", " me.dy) )
      (D2D:rect me.x me.y me.w me.h))))


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
  (create-thing state -20 -50 255 0 0 E.right)
  (create-thing state -200 -10 0 255 0 E.right)
  (create-thing state 300 -10 100 40 100 E.left))

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

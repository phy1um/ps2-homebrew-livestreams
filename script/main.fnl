
(print "running main fennel file")

(local D2D (require "draw2d"))
(local VRAM (require "vram"))

(local S (require "state"))

(print "loading utils")
(local T (require "text"))
(local E (require "events"))
(local PLAYER (require "player"))
(local menu (require "mainmenu"))

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
    (if (= true (PAD.held PAD.X)) (table.insert evs E.a0))
    evs))

(fn eventToString [e]
  (if (= e E.right) "right"
      (= e E.left) "left"
      (= e E.up) "up"
      (= e E.down) "down"
      "?"))

(fn new-roamer [sx sy tx ty r g b d]
  (fn []
    { :x sx :y sy :w 20 :h 20 :dir d
      :col {: r : g : b} :v 61 :dx 0 :dy 0
      :type "bad"
      :solid true
      :update (fn [me dt state events]
                (let [nd 
                      (if 
                        (and (< me.x sx) (= me.dir E.left)) E.right
                        (and (> me.x tx) (= me.dir E.right)) E.left
                        (and (< me.y sy) (= me.dir E.down)) E.up
                        (and (> me.y ty) (= me.dirt  E.up)) E.down
                        me.dir)]
                  (let [dx (if 
                             (= nd E.left) (* -1 dt me.v)
                             (= nd E.right) (* dt me.v)
                             0)
                        dy (if
                              (= nd E.up) (* -1 dt me.v)
                             (= nd E.down) (* dt me.v)
                             0)]
                    (set me.x (+ me.x dx))
                    (set me.y (+ me.y dy))
                    (set me.dir nd)))
                me)
      :draw (fn [me]
        (D2D:setColour me.col.r me.col.g me.col.b 0x80)
        (let [ddx (- me.x sx)]
          (T.printLines me.x (- me.y 30) (.. ddx " - " (eventToString me.dir))))
        (D2D:rect me.x me.y me.w me.h))
      }))




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
  (set state (menu.new)))
  ;(set state (S.new-state))
  ;(print "creating entity")
  ;(state:spawn (PLAYER.new -20 -50 255 0 0 E.right))
  ;(state:spawn (new-roamer -120 0 300 20 30 40 50 E.right)))

(global *dt* (/ 1 60))
(print "overriding on-frame handler")
(fn PS2PROG.frame []
  (local events (get-events))
  (set state (S.update *dt* state events))
  (D2D:newBuffer)
  (let [db (D2D:getBuffer)]
    (db:frameStart gs)
    (D2D:setColour 0xff 0xff 0xff 0x80)
    (S.draw state)
    (D2D:setColour 255 255 255 0x80)
    (T.printLines -300 -220 (.. "tris/frame=" D2D.rawtri ", KC=" (+ D2D.kc 1)))
    (let [db (D2D:getBuffer)]
      (db:frameEnd gs)
      (D2D:kick)))
  (set D2D.rawtri 0)
  (set D2D.kc 0)
  (set counter (+ counter 1)))

(print "ended main fennel file")

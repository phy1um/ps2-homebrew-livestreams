
(print "running main fennel file")

(local D2D (require "draw2d"))
(local VRAM (require "vram"))

(local S (require "state"))

(print "loading utils")
(local T (require "text"))
(local E (require "events"))
(local PLAYER (require "player"))
(local menu (require "mainmenu"))
(local resources (require "resource"))

(var gs nil)
(var font nil)

(var state {})

(var *btn-state* {})

(var *binds* {})
(tset *binds* PAD.UP E.type.up)
(tset *binds* PAD.DOWN E.type.down)
(tset *binds* PAD.LEFT E.type.left)
(tset *binds* PAD.RIGHT E.type.right)
(tset *binds* PAD.X E.type.a0)
(tset *binds* PAD.CIRCLE E.type.a3)
(tset *binds* PAD.SQUARE E.type.a1)
(tset *binds* PAD.TRIANGLE E.type.a2)

(each [_ v (pairs E.type)]
  (tset *btn-state* v false))

(var *buttons* [PAD.UP PAD.DOWN PAD.LEFT PAD.RIGHT PAD.X])

(fn test-button [p e]
  (let [ps (PAD.held p)
        btn-state (. *btn-state* e)]
    (let [res (if (= true ps)
                ; if pad held 
                (if (= false btn-state) 
                  [(E.event e E.mod.hold) (E.event e E.mod.press)]
                  [(E.event e E.mod.hold)])
                ; else pad not held
                (if (= true btn-state) 
                  [(E.event e E.mod.release)]
                  []))]
      (tset *btn-state* e ps)
      res)))

(fn get-events []
  (let [evs []]
    (each [p e (pairs *binds*)]
      (let [xs (test-button p e)]
        (each [_ x (ipairs xs)]
          (table.insert evs x))))
    evs))

(var counter 0)

(fn eventToString [e]
  (if (= e E.type.right) "right"
      (= e E.type.left) "left"
      (= e E.type.up) "up"
      (= e E.type.down) "down"
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
  (resources:load)
  (print "loaded resources")
  (set T.font resources.font)
  (DMA.init DMA.GIF)
  (GS.setOutput 640 448 GS.INTERLACED GS.NTSC)
  (let [fb1 (VRAM.buffer 640 448 GS.PSM24 256)
        fb2 (VRAM.buffer 640 448 GS.PSM24 256)
        zb (VRAM.buffer 640 448 GS.PSMZ24 256)]
    (GS.setBuffers fb1 fb2 zb)
    (D2D:clearColour 0x2b 0x2b 0x2b))
  (set state (menu.new)))
  ;(set state (S.new-state))
  ;(print "creating entity")
  ;(state:spawn (PLAYER.new -20 -50 255 0 0 E.right))
  ;(state:spawn (new-roamer -120 0 300 20 30 40 50 E.right)))

(global *dt* (/ 1 60))
(print "overriding on-frame handler")
(fn PS2PROG.frame []
  (local events (get-events))
  (set state (state.update *dt* state events))
  (D2D:frameStart)
  (D2D:setColour 0xff 0xff 0xff 0x80)
  (state:draw)
  (D2D:setColour 255 255 255 0x80)
  ;(T.printLines -300 -220 (.. "tris/frame=" D2D.prev.rawtri ", KC=" D2D.prev.kc))
  (D2D:frameEnd)
  (set counter (+ counter 1)))

(print "ended main fennel file")

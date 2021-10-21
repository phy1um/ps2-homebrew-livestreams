(local state (require "state"))
(local player (require "player"))
(local C (require "col"))
(local T (require "text"))
(local R (require "resource"))
(local D2D (require "draw2d"))
(local es (require "enemy/walker"))
(local bat (require "enemy/bat"))
(local room (require "room"))

(local *GRID* 16)

(local tile0 (R.get-uv 16 4 3))

(fn tile-draw []
  {
    :update (fn [me] me)
    :draw (fn [me state]
            (each [i v (pairs state.m.active-room.tiles)]
              (let [gx (% i state.m.w)
                    gy (- (math.floor (/ i state.m.w)) 1)
                    wx (- (* gx *GRID*) 320)
                    wy (- (* gy *GRID*) 224) ]
                (if (> v 0)
                  (do
                    (D2D:setColour 100 100 100 0x80)
                    (D2D:sprite R.tiles wx wy *GRID* *GRID* tile0.u1 tile0.v1 tile0.u2 tile0.v2))))))
   })


(fn zone [x y w h draw action]
  (fn []
  {
   :x -100 :y -100 :w 20 :h 20
   :update (fn [me dt state]
      (state:add-col (C.collider-rect me.id me.x me.y me.w me.h action))
      me)
    : draw
  }))

(fn new [w h]
  (let [s (state.new-state)]
    (s:spawn (zone -100 -100 30 30 
                   (fn [me]
                     (D2D:setColour 180 100 180 0x80)
                     (T.printLines me.x (- me.y 30) "Menu")
                     (D2D:rect me.x me.y me.w me.h))
                   (fn [other]
                     (if (= other.type "player")
                       {:do-pop true}))))
    (s:spawn tile-draw)
    (s:spawn (es.new 10 10 19.4))
    (s:spawn (bat.new -100 100 24 2))
    ; create our player and remember him!
    (let [p (s:spawn (player.new -200 -200 0x93 0x80 0x93 nil))]
      (set s.m {
                : w 
                : h
                :active-room (room.new-room -320 -240 w h)
                :player p
                :tile-free (fn [m x y] (m.active-room:tile-free x y))
                :tile-rect-free (fn [m x y w h] (m.active-room:tile-rect-free x y w h))
                :tile-set (fn [m x y v] (m.active-room:tile-set x y v))
                :tile-get (fn [m x y] (m.active-room:tile-get x y))
                }))

    (s.m:tile-set 0 0 4)
    (for [i 0 32]
      (s.m:tile-set i 20 2))
    (for [i 0 12]
      (s.m:tile-set i 17 3))
    s))

{
 : new
 }

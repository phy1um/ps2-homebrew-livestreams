(local state (require "state"))
(local player (require "player"))
(local C (require "col"))
(local T (require "text"))
(local R (require "resource"))
(local D2D (require "camera"))
(local es (require "enemy/walker"))
(local bat (require "enemy/bat"))
(local room (require "room"))

(local *GRID* 16)

(local tile0 (R.get-uv 16 4 3))

(fn set-room [s room-id]
  (let [nr (. s.m.room-map room-id)]
    (set s.view-x nr.ox)
    (set s.view-y nr.oy)
    (print "new XO" s.view-x)
    (set s.m.active-room nr)
    (D2D:setCamera nr.ox nr.oy)))


(fn tile-draw []
  {
    :update (fn [me] me)
    :draw (fn [me state]
            (each [i v (pairs state.m.active-room.tiles)]
              (let [gx (% i state.m.w)
                    gy (- (math.floor (/ i state.m.w)) 1)
                    wx (* gx *GRID*) 
                    wy (+ (* gy *GRID*) 16) ]
                (if (> v 0)
                  (do
                    (D2D:setColour 100 100 100 0x80)
                    (D2D:sprite R.tiles wx wy *GRID* *GRID* tile0.u1 tile0.v1 tile0.u2 tile0.v2))))))
   })

(fn player-focus []
  {
    :update (fn [me dt state] 
              (let [next-room (if (< state.m.player.x state.m.active-room.ox) "left"
                                (> state.m.player.x (+ state.m.active-room.ox 640)) "right"
                                (< state.m.player.y state.m.active-room.oy) "up"
                                (> state.m.player.y (+ state.m.active-room.oy 448)) "down"
                                nil)]
                (if (~= next-room nil)
                  (do
                    (print "changing room" next-room (. state.m.active-room next-room))
                    (set-room state (. state.m.active-room next-room)))))
              me)
    :draw (fn [])
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
    (s:spawn player-focus)
    (s:spawn (es.new 10 10 19.4))
    (s:spawn (bat.new 600 100 24 2))
    (print "spawn entities")
    ; create our player and remember him!
    (let [p (s:spawn (player.new 320 220 0x93 0x80 0x93 nil))]
      (set s.m {
                : w 
                : h
                :active-room nil
                :room-map {}
                :player p
                :tile-free (fn [m x y] (m.active-room:tile-free x y))
                :tile-rect-free (fn [m x y w h] (m.active-room:tile-rect-free x y w h))
                :tile-set (fn [m x y v] (m.active-room:tile-set x y v))
                :tile-get (fn [m x y] (m.active-room:tile-get x y))
                :add-room (fn [m r]
                            (tset m.room-map r.id r))
                }))
    (print "spawn player")
    (let [r1 (room.new-room 0 0 w h)
          r2 (room.new-room 640 0 w h)]
      (r1:tile-set 0 0 4)
      (for [i 0 32]
        (r1:tile-set i 20 2))
      (for [i 0 12]
        (r1:tile-set i 17 3))
      (s.m:add-room r1)
      (s.m:add-room r2)
      (set r1.right r2.id)
      (set r2.left r1.id)
      (set-room s r1.id)
      (print "room init"))
    s))
      

{
 : new
 }

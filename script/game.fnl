(local state (require "state"))
(local player (require "player"))
(local C (require "col"))
(local T (require "text"))
(local R (require "resource"))
(local E (require "events"))
(local D2D (require "camera"))
(local es (require "enemy/walker"))
(local bat (require "enemy/bat"))
(local room (require "room"))

(local *GRID* 16)

(fn set-room [s room-id]
  ;(s.m.active-room:leave s)
  (print "setting room to " room-id)
  (let [nr (. s.m.room-map room-id)]
    (if (~= nr nil )
      (do
        (set s.view-x nr.ox)
        (set s.view-y nr.oy)
        (print "enter room @" nr.ox nr.oy)
        (print " - links = " nr.left nr.right nr.up nr.down)
        (set s.m.active-room nr)
        (D2D:setCamera nr.ox nr.oy)
        (nr:enter s))
      (do
        (print "NIL ROOM TARGET! UNKNOWN ID " room-id)))))

(fn focus-room [tgt state]
  (let [next-room (if (< tgt.x state.m.active-room.ox) "left"
                      (> tgt.x (+ state.m.active-room.ox 640)) "right"
                      (< tgt.y state.m.active-room.oy) "up"
                      (> tgt.y (+ state.m.active-room.oy 448)) "down"
                      nil)]
    (if (~= next-room nil)
      (do
        (print "changing room" next-room (. state.m.active-room next-room))
        (set-room state (. state.m.active-room next-room))))))


(fn player-focus []
  {
    :update (fn [me dt state] 
              (if (~= nil state.m.player)
                (focus-room state.m.player state))
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

(fn new [w h rooms active-id]
  (let [s (state.new-state)]
    (s:spawn (zone -100 -100 30 30 
                   (fn [me]
                     (D2D:setColour 180 100 180 0x80)
                     (T.printLines me.x (- me.y 30) "Menu")
                     (D2D:rect me.x me.y me.w me.h))
                   (fn [other]
                     (if (= other.type "player")
                       {:do-pop true}))))
    (s:spawn room.tile-draw)
    (s:spawn player-focus)
    (s:spawn (fn []
               {
                :update (fn [me dt state events]
                          (each [_ e (ipairs events)]
                            (if (E.is e E.type.menu E.mod.press) (set state.do-pop true)))
                          me)
                :draw (fn [])
                }))

    (set s.m {
              : w 
              : h
              :active-room nil
              :room-map rooms
              :player nil 
              :tile-free (fn [m x y] (m.active-room:tile-free x y))
              :tile-rect-free (fn [m x y w h] (m.active-room:tile-rect-free x y w h))
              :tile-set (fn [m x y v] (m.active-room:tile-set x y v))
              :tile-get (fn [m x y] (m.active-room:tile-get x y))
              :add-room (fn [m r]
                          (tset m.room-map r.id r))
              })
    (set-room s active-id)
    s))
      

{
 : new
 }

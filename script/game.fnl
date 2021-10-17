(local state (require "state"))
(local player (require "player"))
(local C (require "col"))
(local T (require "text"))
(local D2D (require "draw2d"))

(fn zone [x y w h draw action]
  (fn []
  {
   :x -100 :y -100 :w 20 :h 20
   :update (fn [me dt state]
      (state:add-col (C.collider-rect me.id me.x me.y me.w me.h action))
      me)
    : draw
  }))

(fn new []
  (let [s (state.new-state)]
    (s:spawn (zone -100 -100 30 30 
                   (fn [me]
                     (D2D:setColour 180 100 180 0x80)
                     (T.printLines me.x (- me.y 30) "POTATO")
                     (D2D:rect me.x me.y me.w me.h))
                   (fn [other]
                     (print "boop")
                     (if (= other.type "player")
                       {:do-pop true}))))
    (s:spawn (player.new 0 0 255 0 0 nil))
    s))

{
 : new
 }

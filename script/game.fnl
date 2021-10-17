(local state (require "state"))
(local player (require "player"))
(local C (require "col"))
(local T (require "text"))
(local D2D (require "draw2d"))

(local *GRID* 16)

(fn tile-draw []
  {
    :update (fn [me] me)
    :draw (fn [me state]
            (each [i v (pairs state.m.tiles)]
              (let [gx (% i state.m.w)
                    gy (- (math.floor (/ i state.m.w)) 1)
                    wx (- (* gx *GRID*) 320)
                    wy (- (* gy *GRID*) 224) ]
                (if (> v 0)
                  (do
                    (D2D:setColour 100 100 100 0x80)
                    (D2D:rect wx wy *GRID* *GRID*))))))
   })

(fn make-tile-map [w h]
  (let [tiles []]
    (for [i 0 w] 
      (for [j 0 h]
        (tset tiles (+ i (* j w)) 0)))
      tiles))


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
                     (T.printLines me.x (- me.y 30) "POTATO")
                     (D2D:rect me.x me.y me.w me.h))
                   (fn [other]
                     (print "boop")
                     (if (= other.type "player")
                       {:do-pop true}))))
    (s:spawn tile-draw)
    (s:spawn (player.new 0 0 255 0 0 nil))
    (set s.m {
              : w 
              : h
              :tiles (make-tile-map w h)
              :tile-free (fn [m x y]
                          (let [gx (math.floor (/ (+ x 320) *GRID*)) 
                                gy (math.floor (/ (+ y 240) *GRID*))]
                            (= 0 (m:tile-get gx gy))))
              :tile-rect-free (fn [m x y w h]
                                (and
                                  (m:tile-free x y)
                                  (m:tile-free (+ x w) y)
                                  (m:tile-free x (+ y h))
                                  (m:tile-free (+ x w) (+ y h))))
              :tile-set (fn [m x y v]
                          (let [index (+ x (* w y))]
                            (print "TM set " index v)
                            (tset m.tiles index v))
                          v)
              :tile-get (fn [m x y]
                          (let [index (+ x (* w y))]
                            (print "GET " x y index)
                            (let [res (. m.tiles index)]
                              (print " =" res)
                              res)))
              })

    (s.m:tile-set 0 0 4)
    (s.m:tile-set 10 10 4)
    (s.m:tile-set 10 11 5)
    (s.m:tile-set 10 12 6)
    (s.m:tile-set 10 13 6)
    (s.m:tile-set 10 14 6)
    (s.m:tile-set 10 15 6)
    (s.m:tile-set 10 16 6)
    (print "TILES = " s.m.tiles)
    s))

{
 : new
 }

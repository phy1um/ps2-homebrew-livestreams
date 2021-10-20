(local state (require "state"))
(local player (require "player"))
(local C (require "col"))
(local T (require "text"))
(local D2D (require "draw2d"))
(local es (require "enemy/walker"))
(local bat (require "enemy/bat"))

(local *GRID* 16)

(fn tile-draw []
  {
    :update (fn [me] me)
    :draw (fn [me state]
            (each [i v (pairs state.m.tiles)]
              (let [gx (% i state.m.w)
                    gy (- (math.floor (/ i state.m.w)) 1)
                    wx (- (* gx *GRID*) 320)
                    wy (- (* gy *GRID*) (- 224 16)) ]
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
                     (T.printLines me.x (- me.y 30) "Menu")
                     (D2D:rect me.x me.y me.w me.h))
                   (fn [other]
                     (if (= other.type "player")
                       {:do-pop true}))))
    (s:spawn tile-draw)
    (s:spawn (es.new 10 10 19.4))
    (s:spawn (bat.new -100 100 24 2))
    ; create our player and remember him!
    (let [p (s:spawn (player.new -200 -200 255 0 0 nil))]
      (set s.m {
                : w 
                : h
                :player p
                :tiles (make-tile-map w h)
                :tile-free (fn [m x y]
                             (let [wx (+ x 320) wy (+ y 224)
                                   gx (math.floor (/ wx *GRID*))
                                   gy (math.floor (/ wy *GRID*))]
                             ;(print "test tm point " wx wy)
                              (if 
                                ; if wx and wy OOB then false
                                (or 
                                  (or (< gx 0) (< gy 0))
                                  (or (>= gx w) (>= gy h)))
                                false
                                ; else test tile map
                                (= 0 (m:tile-get gx gy)))))
                :tile-rect-free (fn [m x y w h]
                                  (and
                                    (m:tile-free x y)
                                    (m:tile-free (+ x w) y)
                                    (m:tile-free x (+ y h))
                                    (m:tile-free (+ x w) (+ y h))))
                :tile-set (fn [m x y v]
                            (let [index (+ x (* w y))]
                              (tset m.tiles index v))
                            v)
                :tile-get (fn [m x y]
                            (let [index (+ x (* w y))]
                              (let [res (. m.tiles index)]
                                ;(print "tile get " x y res)
                                res)))
                }))

    (s.m:tile-set 0 0 4)
    (for [i 0 32]
      (s.m:tile-set i 20 2))
    (for [i 0 12]
      (s.m:tile-set i 17 3))
    (print "TILES = " s.m.tiles)
    s))

{
 : new
 }


(local D2D (require "camera"))
(local R (require "resource"))
(local entity (require "entity"))
(local fennel (require "fennel"))

(local *GRID* 16)
(local tile0 (R.get-uv 16 4 3))

(var *id-state* 177)
(fn universal-id []
  (let [i *id-state*]
    (set *id-state* (+ *id-state* 1))
    i))

(fn make-tile-map [w h]
  (let [tiles []]
    (for [i 0 w] 
      (for [j 0 h]
        (tset tiles (+ i (* j w)) 0)))
      tiles))

(local *GRID* 16)

(local copy-fields [
                    "up" "down" "left" "right"
                    "ox" "oy" "w" "h" "id"
                    "tiles" "entity-spawns"])

(fn stringify [self]
  (let [o (collect [_ k (ipairs copy-fields)]
                   (values k (. self k)))]
    (fennel.view o)))

(fn minify [self]
  (collect [_ k (ipairs copy-fields)]
             (values k (. self k))))




(fn new-room [ox oy w h]
  {
    : ox : oy : w : h
    :id (universal-id)
    :tiles (make-tile-map w h)
    :entity-spawns []
    :add-entity-spawn (fn [self f] (table.insert self.entity-spawns f))
    :tile-free (fn [m x y]
                 (let [wx (- x ox) wy (- y oy)
                       gx (math.floor (/ wx *GRID*))
                       gy (math.floor (/ wy *GRID*))]
                  (if 
                    ; if wx and wy OOB then false
                    (or 
                      (or (< gx 0) (< gy 0))
                      (or (>= gx w) (>= gy h)))
                    true
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
    :left nil
    :right nil
    :up nil
    :down nil
    :enter (fn [])
    :leave (fn [])
    : minify
   })



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


;; init room loaded from file

(fn gen-enter [r]
  (fn [self state]
    (print "ENTER ROOM" r.id)
    (each [_ sp (ipairs r.entity-spawns)]
      (let [f (entity.get-spawn-fn sp.class sp.x sp.y)]
        (if (= f nil) (print "failed to spawn " sp.class))
        (let [e (state:spawn f)]
          (if (= sp.class "player")
            (set state.m.player e)))))))

(fn from-loaded [o]
  (let [n (new-room 0 0 0 0)]
    (each [k v (pairs o)]
      (print "setting loaded room field: " k v)
      (tset n k v))
    (set n.enter (gen-enter n))
    n))

(fn map-from-file [fname]
  (let [loaded (fennel.dofile fname)]
    (collect [k v (pairs loaded)]
               (values k (from-loaded v)))))


(fn find-player [rm]
  (print "looking for player room start IN" rm)
  (var tgt -1)
  (each [k v (pairs rm)]
    (print "trying room" k v)
    (each [_ sp (ipairs v.entity-spawns)]
      (print "found entity" sp.class sp.x sp.y)
      (if (= sp.class "player")
        (set tgt k))))
  (print "found player @" tgt)
  tgt)

{
 : new-room
 : tile-draw
 : map-from-file
 : find-player
 }

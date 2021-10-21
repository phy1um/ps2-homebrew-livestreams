
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
   })

{
 : new-room
 }

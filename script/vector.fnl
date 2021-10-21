
(fn vec2 [x y]
  {: x : y})

(fn vec-length [v]
  (math.sqrt (+ (* v.x v.x) (* v.y v.y))))

(fn vec-normalize* [v]
  (let [l (vec-length v)]
    (set v.x (if (> l 0) (/ v.x l) 0))
    (set v.y (if (> l 0) (/ v.y l) 0))
    v))

(fn vec-normalize [v]
  (let [v* (vec2 v.x v.y)]
    (vec-normalize* v*)))

(fn vec-scale* [v n]
  (set v.x (* v.x n))
  (set v.y (* v.y n))
  v)

(fn vec-scale [v n]
  (let [v* (vec2 v.x v.y)]
    (vec-scale* v* n)))

{
 : vec2
 : vec-length
 : vec-normalize*
 : vec-normalize
 : vec-scale*
 : vec-scale
}

    

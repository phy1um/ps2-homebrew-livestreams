
(local c {})

(fn c.point-in-rect [ax ay x y w h]
  (and
    (>= ax x)
    (<= ax (+ x w))
    (>= ay y)
    (<= ay (+ y h))))

(fn c.collider-rect [id x y w h onhit]
  (fn [other]
    (if (= other.id id) {:hit false}
        (or
          (c.point-in-rect other.x other.y x y w h)
          (c.point-in-rect (+ other.x other.w) other.y x y w h)
          (c.point-in-rect other.x (+ other.y other.h) x y w h)
          (c.point-in-rect (+ other.x other.w) (+ other.y other.h) x y w h))
      {:hit true :action onhit}
      {:hit false})))

c

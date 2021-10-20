
(local c {})

(fn c.point-in-rect [ax ay x y w h]
  (and
    (>= ax x)
    (<= ax (+ x w))
    (>= ay y)
    (<= ay (+ y h))))

(fn c.collider-rect [id x y w h onhit]
  (fn [other]
    (if (and (~= other.id id)
             (or
               (or
                (c.point-in-rect other.x other.y x y w h)
                (c.point-in-rect (+ other.x other.w) other.y x y w h)
                (c.point-in-rect other.x (+ other.y other.h) x y w h)
                (c.point-in-rect (+ other.x other.w) (+ other.y other.h) x y w h))
               (or
                (c.point-in-rect x y other.x other.y other.w other.h)
                (c.point-in-rect (+ x w) y other.x other.y other.w other.h)
                (c.point-in-rect x (+ y h) other.x other.y other.w other.h)
                (c.point-in-rect (+ x w) (+ y h) other.x other.y other.w other.h))))
      {:hit true :action onhit}
      {:hit false})))


c

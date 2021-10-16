
(local C (require "col"))
(local T (require "text"))
(local E (require "events"))
(local D2D (require "draw2d"))

(fn fclamp [x min max]
  (if (> x max) max
    (< x min) min
    x))

(fn new [x y r g b d] 
    (fn [] 
      { : x : y :w 24 :h 24 
       :col {: r : g : b} :dir d 
       :v 50 :dx 0 :dy 0 
       :health 5
       :type "player"
       :solid true

      :update (fn [me dt state events]
        (set me.dx 0)
        (set me.dy 0)
        (let [px me.x py me.y]
          (each [i ev (ipairs events)]
            (if (= ev E.up) (set me.dy (* -1 dt me.v))
                (= ev E.down) (set me.dy (* dt me.v))
                (= ev E.left) (set me.dx (* -1 dt me.v))
                (= ev E.right) (set me.dx (* dt me.v))
                nil))
          (set me.x (+ me.x me.dx))
          (set me.y (+ me.y me.dy))
          (state:add-col (C.collider-rect 
                           me.id
                           me.x 
                           me.y 
                           me.w 
                           me.h 
                           (fn [other]
                             (print other.type)
                             (if (= other.type "bad") (set me.health (- me.health 1)))))))
        (if (>= me.health 0) me
          (do
            ; player dead logic here
            nil)))

      :draw (fn [me]
        (D2D:setColour me.col.r me.col.g me.col.b 0x80)
        ; (T.printLines me.x (- me.y 30) (.. me.dx ", " me.dy) )
        (T.printLines me.x (- me.y 30) (.. "HP: " me.health))
        (D2D:rect me.x me.y me.w me.h))
    }))

{
  : new
}

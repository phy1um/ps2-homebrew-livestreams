
(local C (require "col"))
(local T (require "text"))
(local E (require "events"))
(local D2D (require "draw2d"))


(fn new [x y r g b d] 
    (fn [] 
      { : x : y :w 24 :h 24 
       :col {: r : g : b} :dir d 
       :v 50 :dx 0 :dy 0 
       :type "player"

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
                             (let [cdx (- me.x other.x)
                                   cdy (- me.y other.y)]
                             (set me.x (+ py (* dt 2 cdx)))
                             (set me.y (+ py (* dt 2 cdy)))))))))
      :draw (fn [me]
        (D2D:setColour me.col.r me.col.g me.col.b 0x80)
        ; (T.printLines me.x (- me.y 30) (.. me.dx ", " me.dy) )
        (D2D:rect me.x me.y me.w me.h))
    }))

{
  : new
}

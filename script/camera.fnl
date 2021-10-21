(local D2D (require "draw2d"))

(local cam {
            :cx 0
            :cy 0})

(fn cam.setCamera [self x y]
  (set self.cx x)
  (set self.cy y))

(fn cam.sprite [c tex x y w h u1 v1 u2 v2]
  (D2D:sprite tex (- x c.cx) (- y c.cy) w h u1 v1 u2 v2))

(fn cam.rect [c x y w h]
  (D2D:rect (- x c.cx) (- y c.cy) w h))

(fn cam.setColour [_ r g b a]
  (D2D:setColour r g b a))

cam

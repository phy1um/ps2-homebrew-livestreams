
(local D2D (require "draw2d"))
(local V (require "vector"))
(local R (require "resource"))

(local frame (R.get-uv 16 4 20))

(fn draw [me]
  (D2D:setColour 0x80 0x80 0x80 0x80)
  (print frame.u1 frame.v1 frame.u2 frame.v2)
  (D2D:sprite R.chars me.x me.y me.w me.h frame.u1 frame.v1 frame.u2 frame.v2))

(fn update [me dt state]
  (if (> me.waiting-timer 0)
      ; we are waiting for something so spin our wheels
      (do
        (set me.waiting-timer (- me.waiting-timer dt))
        me)
      ; do the actual update part
      (do
        ; ...
        (let [tgt state.m.player]
          (if (~= tgt nil)
            (let [dv (V.vec2 (- me.x tgt.x) (- me.y tgt.y))
                  dv* (V.vec-scale (V.vec-normalize dv) (* me.speed dt))]
              (set me.x (- me.x dv*.x))
              (set me.y (- me.y dv*.y))))
          )
        (if (> me.health 0) me nil))))

(fn new [x y speed]
  (fn []
    {
      : x : y : speed
      :w 16 :h 20
      :tx 0 :ty 0
      :waiting-timer 0.4
      :type "enemy"
      :solid true
      :health 3
      :hurt (fn [me d] 
              (let [dmg (if (~= nil d.power) d.power 1)]
                (set me.health (- me.health dmg))))
      : draw
      : update
     }))

{
 : new
 }

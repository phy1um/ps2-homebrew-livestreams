
(local D2D (require "draw2d"))
(local V (require "vector"))

(local *seek* 0)
(local *strafe* 1)

(fn draw [me]
  (D2D:setColour 0xff 0x00 0xff 0x80)
  (D2D:rect me.x me.y me.w me.h))

(fn move-seek [me v speed]
  (let [dv (V.vec-scale (V.vec-normalize v) speed)]
    (set me.x (- me.x dv.x))
    (set me.y (- me.y dv.y))))

(fn move-strafe [me v speed]
  (if (> v.x v.y)
    (set me.y (+ me.y speed))
    (set me.x (+ me.x speed))))

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
                  speed (* dt me.speed) ]
              (if (= me.state *seek*)
                    (move-seek me dv (* 2.4 speed))
                  (= me.state *strafe*)
                    (move-strafe me dv speed)))))
        (if (> me.state-timer 0)
          ; if time left in state decrease time
          (set me.state-timer (- me.state-timer dt))
          ; otherwise advance to next state
          (do
            (set me.state (% (+ me.state 1) 2))
            (print "bat change state" me.state)
            (set me.state-timer 2.1)))
        (if (> me.health 0) me nil))))

(fn new [x y speed health]
  (fn []
    {
      : x : y : speed
      :w 14 :h 8
      :tx 0 :ty 0
      :waiting-timer 0.4
      :type "enemy"
      :solid true
      : health
      :state *seek*
      :state-timer 2.9
      :hurt (fn [me d] 
              (let [dmg (if (~= nil d.power) d.power 1)]
                (set me.health (- me.health dmg))))
      : draw
      : update
     }))

{
 : new
 }

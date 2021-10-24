
(local D2D (require "camera"))
(local V (require "vector"))
(local R (require "resource"))

(local *seek* 0)
(local *strafe* 1)
(local *flee* 2)

(fn draw [me]
  (D2D:setColour 0x80 0x80 0x80 0x80)
  (let [frame (. R.bat-frames.move (if (> me.anim-timer 0.1) 1 2))]
    (D2D:sprite R.chars me.x me.y me.w me.h frame.u1 frame.v1 frame.u2 frame.v2)))

(fn move-seek [me v speed]
  (let [dv (V.vec-scale (V.vec-normalize v) speed)]
    (set me.x (- me.x dv.x))
    (set me.y (- me.y dv.y))))

(fn move-strafe [me v speed tx ty]
  (let [dist (V.vec-length v)
        arcx (+ tx (- (* v.x (math.cos 0.01)) (* v.y (math.sin 0.01))))
        arcy (+ ty (* v.x (math.sin 0.01)) (* v.y (math.cos 0.01)))
        dx (- arcx me.x)
        dy (- arcy me.y)
        move (V.vec-scale (V.vec-normalize (V.vec2 dx dy)) speed)]
    (set me.x (+ me.x move.x))
    (set me.y (+ me.y move.y))))

(fn update [me dt state]
  (set me.anim-timer
       (if (> me.anim-timer 0.2) 0
         (+ me.anim-timer dt)))
  (if (> me.waiting-timer 0)
      ; we are waiting for something so spin our wheels
      (do
        (set me.waiting-timer (- me.waiting-timer dt))
        me)
      ; do the actual update part
      (do
        ; ...
        (if me.washurt
          (do
            (set me.state *flee*)
            (set me.state-timer (math.random 0.08 0.25))
            (set me.washurt false)))
        (let [tgt state.m.player]
          (if (~= tgt nil)
            (let [dv (V.vec2 (- me.x tgt.x) (- me.y tgt.y))
                  speed (* dt me.speed) ]
              (if (= me.state *seek*)
                    (move-seek me dv speed)
                  (= me.state *strafe*)
                    (move-strafe me dv speed tgt.x tgt.y)
                  (= me.state *flee*)
                    (move-seek me dv (* speed -1.8))))))
        (if (> me.state-timer 0)
          ; if time left in state decrease time
          (set me.state-timer (- me.state-timer dt))
          ; otherwise advance to next state
          (do
            (set me.state (% (+ me.state 1) 2))
            (set me.state-timer (math.random 1.1 4.1))))
        (if (> me.health 0) 
          ; still alive
          me 
          ; mort
          (do
           (if (~= me.ondeath nil)
             (me.ondeath))
           nil)))))

{
 : draw
 : update
 :seek *seek*
 }

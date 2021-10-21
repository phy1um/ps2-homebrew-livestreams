
(local C (require "col"))
(local T (require "text"))
(local E (require "events"))
(local D2D (require "camera"))
(local R (require "resource"))
(local vec (require "vector"))

(local *state-ground* 0)
(local *state-fall* 1)
(local *state-jump* 2)

(local *stand-height* 2)

(local *ground-speed* 3.1)
(local *ground-friction* 2.1)
(local *ground-accel* 2.4)
(local *air-accel* 1.8)
(local *gravity* 0.98)
(local *jump-pulse-speed* -8)

(local *up* 0)
(local *down* 1)
(local *left* 2)
(local *right* 3)

(fn bullet [x y dx dy]
  (fn []
  {
    : x : y : dx : dy
    :w 12 :h 12
    :life 0.08
    :type "enemy-hurt"
    :update (fn [me dt state] 
              (set me.x (+ me.x me.dx))
              (set me.y (+ me.y me.dy))
              (set me.life (- me.life dt))
              (if (> me.life 0)
                (do
                  (state:add-col (C.collider-rect 
                                   me.id
                                   me.x 
                                   me.y 
                                   me.w 
                                   me.h 
                                   (fn [other]
                                     (if (and (= other.type "enemy") (~= other.hurt nil))
                                       (other:hurt me))
                                     (if (~= other.type "player")
                                       (set me.life 0))
                                     nil)))
                  me)
                nil))
    :draw (fn [me]
            (D2D:setColour 100 100 100 0x80)
            (D2D:rect me.x me.y me.w me.h))
   }))



(fn friction [v f]
  (if 
    ; move toward 0 from +
    (> v 0)
    (math.max 0 (- v f))
    ; move toward 0 from -
    (< v 0)
    (math.min 0 (+ v f))
    0))

(fn accel-to [a d t]
  (if 
    ; target > 0, go +
    (> t 0)
    (math.min t (+ d a))
    ; target < 0, go -
    (< t 0)
    (math.max t (- d a))
    ; target == 0, do friction instead
    (friction d a)))

(fn vec-length [v]
  (math.sqrt (+ (* v.x v.x) (* v.y v.y))))

(fn update-stand [me dt state _]
  (let [wish {:x me.impulse-x :y me.impulse-y}
        wish-u (vec.vec-normalize wish)
        d {:x (* *ground-speed* wish-u.x) :y (* *ground-speed* wish-u.y)}]
    (set me.vx d.x)
    (set me.vy d.y)))

(fn new [x y r g b d] 
    (fn [] 
      { : x : y :w 16 :h 16
       :col {: r : g : b} :dir d 
       :v 90 :vx 0 :vy 0
       :health 5
       :type "player"
       :solid true
       :impulse-x 0 :impulse-y 0
       :action *state-ground*
       :last-dir *down*

      :update (fn [me dt state events]
                (set me.impulse-x 0)
                (set me.impulse-y 0)
                ; process inputs into impulse/wish direction
                (each [_ ev (ipairs events)]
                  (if (E.is ev E.type.left E.mod.hold) (set me.impulse-x (+ me.impulse-x -1))
                      (E.is ev E.type.right E.mod.hold) (set me.impulse-x (+ me.impulse-x 1))
                      (E.is ev E.type.up E.mod.hold) (set me.impulse-y (+ me.impulse-y -1))
                      (E.is ev E.type.down E.mod.hold) (set me.impulse-y (+ me.impulse-y 1))
                      (E.is ev E.type.a0 E.mod.press)
                        (do
                          (if (= me.last-dir *right*)
                                (state:spawn (bullet (+ me.x me.w) me.y 10 0))
                              (= me.last-dir *up*)
                                (state:spawn (bullet me.x me.y 0 -10))
                              (= me.last-dir *left*)
                                (state:spawn (bullet me.x me.y -10 0))
                              (= me.last-dir *down*)
                                (state:spawn (bullet me.x (+ me.y me.h) 0 10))))))

                ; run according to our state machine
                (if (= me.action *state-ground*)
                      (update-stand me dt state events))
                ; update position (if it is free!)
                (if (> me.vx 0) (set me.last-dir *right*)
                  (< me.vx 0) (set me.last-dir *left*))
                (if (> me.vy 0) (set me.last-dir *down*)
                  (< me.vy 0) (set me.last-dir *up*))
                (if (state.m:tile-rect-free (+ me.x me.vx) (+ me.y me.vy) me.w me.h)
                  (do
                    (set me.x (+ me.x me.vx))
                    (set me.y (+ me.y me.vy))))
                ; look for collisions
                (state:add-col (C.collider-rect 
                                             me.id
                                             me.x 
                                             me.y 
                                             me.w 
                                             me.h 
                                             (fn [other]
                                               (if (= other.type "bad") (set me.health (- me.health 1)))
                                               nil)))
                ; we are still alive
                me)

      :draw (fn [me]
        (D2D:setColour me.col.r me.col.g me.col.b 0x80)
        ; (T.printLines me.x (- me.y 30) (.. me.dx ", " me.dy) )
        ; (T.printLines me.x (- me.y 30) (.. me.impulse-x "-x " me.action " :: " me.vx))

        (let [frame (. R.player-frames.down 1)] 
          (D2D:sprite R.chars me.x me.y me.w me.h frame.u1 frame.v1 frame.u2 frame.v2)))
    }))

{
  : new
}

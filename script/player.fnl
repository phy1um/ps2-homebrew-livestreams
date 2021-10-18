
(local C (require "col"))
(local T (require "text"))
(local E (require "events"))
(local D2D (require "draw2d"))

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

(fn on-ground [me dx dy state]
  (let [f1 (state.m:tile-free (+ me.x dx) (+ me.y me.h dy *stand-height*))
        f2 (state.m:tile-free (+ me.x me.w dx) (+ me.y me.h dy *stand-height*))]
    (not (or f1 f2))))

(fn move-to-ground [me state yy]
  (var yy (math.floor yy))
  (while (and (> yy 0) (not (on-ground me 0 yy state)))
    (set yy (- yy 1)))
  (set me.y (+ me.y yy))
  (set me.vy 0))

(fn sgn-diff [a b]
  (or (and (> a 0) (< b 0))
      (and (< a 0) (> b 0))))

(fn update-stand [me dt state _]
  (let [dx (* me.impulse-x dt *ground-speed*)]
    (if (or (= dx 0) (sgn-diff dx me.vx))
      ; apply ground friction
      (set me.vx (friction me.vx *ground-friction*))
      ; otherwise accelerate
      (set me.vx (accel-to *ground-accel* me.vx (* me.impulse-x *ground-speed*)))))
  (if (not (on-ground me 0 0 state)) (set me.action *state-fall*))
  (if (> me.impulse-y 0)
    (do
      (set me.action *state-jump*)
      (set me.y (- me.y *stand-height* 1))
      (set me.vy *jump-pulse-speed*)
      (set me.impulse-y 0))
  (set me.vy 0)))

(fn update-misc [me]
  (print "unknown player state")
  (set me.action *state-ground*))

(fn update-fall [me dt state _]
  (let [dy 3]
    (let [newvy (accel-to *gravity* me.vy dy)]
      ;(print "falling from " me.vy " to " newvy)
      (set me.vy newvy)))
  (set me.vx (accel-to *air-accel* me.vx (* me.impulse-x *ground-speed*)))
  (if (on-ground me 0 me.vy state)
    (do
      (set me.action *state-ground*)
      (move-to-ground me state me.vy))))

(fn update-jump [me dt state ev]
  (each [_ e (ipairs ev)]
    (if (E.is e E.type.a0 E.mod.hold) (set me.vy *jump-pulse-speed*)))
  (if (>= me.vy 0) (set me.action *state-fall*))
  (update-fall me dt state ev))

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

      :update (fn [me dt state events]
                (set me.impulse-x 0)
                (set me.impulse-y (- me.impulse-y dt))
                ; process inputs into impulse/wish direction
                (each [_ ev (ipairs events)]
                  (if (E.is ev E.type.left E.mod.hold) (set me.impulse-x -1)
                      (E.is ev E.type.right E.mod.hold) (set me.impulse-x 1)
                      (E.is ev E.type.a0 E.mod.press) (set me.impulse-y 0.2)))
                ; run according to our state machine
                (if (= me.action *state-ground*)
                      (update-stand me dt state events)
                    (= me.action *state-fall*)
                      (update-fall me dt state events)
                    (= me.action *state-jump*)
                      (update-jump me dt state events)
                    (update-misc me dt state events))
                ; update position (if it is free!)
                ; (print "speed " me.vx me.vy)
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
                                               (print other.type)
                                               (if (= other.type "bad") (set me.health (- me.health 1)))
                                               nil)))
                ; we are still alive
                me)

      :draw (fn [me]
        (D2D:setColour me.col.r me.col.g me.col.b 0x80)
        ; (T.printLines me.x (- me.y 30) (.. me.dx ", " me.dy) )
        (T.printLines me.x (- me.y 30) (.. me.impulse-x "-x " me.action " :: " me.vx))
        (D2D:rect me.x me.y me.w me.h))
    }))

{
  : new
}

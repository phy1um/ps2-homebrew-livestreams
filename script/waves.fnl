(local EE (require "entity"))
(local T (require "text"))
(local D2D (require "draw2d"))

(fn bat [x y]
  (EE.get-spawn-fn "bat1" x y))

(fn walk [x y]
  (EE.get-spawn-fn "walker1" x y))

(fn msg [t]
  (fn [] { :life 3
           :ox (- 320 (/ (* 16 (length t)) 2))
          :update (fn [me dt]
                    (if (<= me.life 0)
                      nil
                      (do
                        (set me.life (- me.life dt))
                        me)))
          :draw (fn [me]
                  (D2D:setColour 255 255 255 0x80)
                  (T.printLines me.ox 100 t))
          }))

(fn wave-init [seq oncomplete]
  (fn []
    {
     :ec 0
     :time 0
     : seq
     :draw (fn [])
     :update (fn [me dt state]
               (set me.time (+ me.time dt))
               (let [ss (me.seq me state me.time)]
                 (if (~= ss nil)
                   (set me.seq ss)))
               me)
     :spawn (fn [self state f]
              (let [e (state:spawn f)]
                (set self.ec (+ self.ec 1))
                (set e.ondeath (fn []
                                (set self.ec (- self.ec 1)))))) 
     : oncomplete
     }))


(fn at [time f then]
  (fn [me state t]
    (if (> t time) (do
                     (f me state)
                     then))))
(fn after [time f then]
  (var is-done false)
  (fn [me state t]
    (if is-done
      ; test
      (if (> t me.after-counter)
        (do
          (f me state)
          then))
      ; store current time etc
      (do
        (set me.after-counter (+ t time))
        (set is-done true)))))

(fn block-clear [then]
  (fn [me state t]
    (if (<= me.ec 0)
      then)))

(fn pass [] )

(fn wait-for-end [then]
  (block-clear
    (after 2.2 pass then)))


(local wave-3
  (after 0 (fn [me state]
             (print "START WAVE 3")
             (state:spawn (msg "WAVE 3")))
  (after 3 (fn [me state]
             (me:spawn state (bat 0 100))
             (me:spawn state (bat 0 340)))
  (after 5 (fn [me state]
             (me:spawn state (bat 640 100))
             (me:spawn state (bat 640 340)))
  (after 2 (fn [me state]
             (me:spawn state (walk 100 0))
             (me:spawn state (walk 540 0)))
  (block-clear
  (after 0.1 (fn [me state]
               (me:spawn state (bat 0 0))
               (me:spawn state (bat 640 480))
               (me:spawn state (bat 0 448))
               (me:spawn state (bat 640 0)))
  (block-clear
  (after 0.8 (fn [me state]
               (me:spawn state (walk 0 200)))
  (after 0.2 (fn [me state]
               (me:spawn state (walk 0 300)))
  (after 0.2 (fn [me state]
                 (me:spawn state (walk 640 100)))
  (after 0.2 (fn [me state]
                 (me:spawn state (walk 640 20)))
  (after 0.2 (fn [me state]
                 (me:spawn state (walk 300 0)))
  (after 2 (fn [me state]
             (me:spawn state (bat 200 450))
             (me:spawn state (bat 400 450)))
  (wait-for-end (fn [me state]
                 (state:spawn (msg "YOU WIN :)")
                              nil)))))))))))))))))


(local wave-2
  (after 0 (fn [me state]
          (print "START WAVE 2")
          (state:spawn (msg "WAVE 2")))
  (after 3 (fn [me state]
          (me:spawn state (bat 320 0)))
  (after 4 (fn [me state]
          (me:spawn state (walk 280 450))
          (me:spawn state (walk 400 450)))
  (block-clear
  (after 0.5 (fn [me state]
               (me:spawn state (bat 0 200)))
  (after 0.5 (fn [me state]
               (me:spawn state (bat 0 300)))
  (wait-for-end wave-3))))))))


(local wave-1 
  (at 0 (fn [me state]
          (print "START WAVE 1")
          (state:spawn (msg "WAVE 1")))
  (at 3 (fn [me state]
          (me:spawn state (walk 600 200)))
  (block-clear
  (after 3 (fn [me state]
             (me:spawn state (walk 0 180)))
  (block-clear
  (after 1 (fn [me state]
             (me:spawn state (walk 20 20))
             (me:spawn state (walk 200 20))
             (me:spawn state (walk 400 20)))
  (wait-for-end wave-2))))))))

{
 : wave-init
 : wave-1
 : wave-2
 }

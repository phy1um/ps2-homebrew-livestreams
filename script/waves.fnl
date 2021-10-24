(local EE (require "entity"))

(fn bat [x y]
  (EE.get-spawn-fn "bat1" x y))


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

(fn wait-for-end [me state]
  (if (<= me.ec 0)
    (do
      (me.oncomplete)
      (set me.oncomplete (fn [])))))

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

(local wave-1 
  (at 0 (fn [me state]
          (me:spawn state (bat 20 200)))
  (at 6 (fn [me state]
          (me:spawn state (bat 20 10))
          (me:spawn state (bat 20 210)))
  (after 1 (fn [me state]
             (print "after 1?")
             (me:spawn state (bat 20 20))
             (me:spawn state (bat 200 20))
             (me:spawn state (bat 400 20)))
  wait-for-end))))



{
 : wave-init
 : wave-1
 }

(local bat (require "enemy/bat"))
(local walker (require "enemy/walker"))
(local player (require "player"))

(fn hurt [me d]
  (let [dmg (if (~= nil d.power) d.power 1)]
    (set me.health (- me.health dmg))))

; copy A to B
(fn cpy-> [a b]
  (each [k v (pairs a)]
    (tset b k v))
  b)

(local class-list [
                   "null" "player" "bat1" "walker1"])

(fn get-class [i]
  (if (and (>= i 0) (< i (length class-list)))
    (. class-list i)
    "???"))

(local classes {
                :player {
                        :w 16 :h 16 :v 90
                        :vx 0 :vy 0
                        :col {:r 100 :g 100 :b 100}
                        :health 6
                        :type "player"
                        :solid true 
                        :impulse-x 0 :impulse-y 0
                        :action 0
                        :last-dir 0
                        :update player.update
                        :draw player.draw
                        :hurtdebounce 0.01
                        :hurt player.hurt

                         }
                :bat1 {
                      :w 28 :h 14
                      :speed 20
                      :tx 0 :ty 0
                      :waiting-timer 0.4
                      :type "enemy"
                      :solid true
                      :health 3
                      :state bat.seek
                      :state-timer 2.9
                      :anim-timer 0
                      : hurt                       
                      :draw bat.draw
                      :update bat.update
                      }
                :walker1 {
                      :w 16 :h 24
                      :tx 0 :ty 0
                      :waiting-timer 0.4
                      :type "enemy"
                      :solid true
                      :health 3
                      : hurt 
                      :draw walker.draw
                      :update walker.update
                      }
                })

(fn get-spawn-fn [c x y]
  (let [class (. classes c)
        f (if
            (= class nil) (fn []
                            (print "SPAWN NIL!")
                            { :update (fn []) :draw (fn []) })
            (fn [] 
              (print "SPAWN!")
              (cpy-> class {: x : y})))]
    f))

{
 : get-spawn-fn
 : get-class
 : classes
 }

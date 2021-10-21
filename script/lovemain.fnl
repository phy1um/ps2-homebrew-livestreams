(local fennel (require "fennel"))

(local state (require "state"))
(local D2D (require "draw2d"))
(local T (require "text"))
(local E (require "events"))
(local PLAYER (require "player"))

(var events [])

(var key-map {
              :w false
              :s false
              :a false
              :d false
              :z false
              :x false
              :c false
              :v false
              :m false
              })

(fn love.keypressed [key]
  (if (= key "escape") (love.event.quit 0))
  (tset key-map key true))

(fn love.keyreleased [key]
  (tset key-map key false))

(global PS2PROG {})

(global DMA {
             :init (fn [] nil)
             :GIF 0
             })

(global GS {
            :setOutput (fn [w h]
                         (love.window.setMode w h))
            :setBuffers (fn [])
            })

(global PAD {
             :UP "up" :LEFT "left" :DOWN "down" :RIGHT "right" :X "x"
             :SQUARE "z" :TRIANGLE "c" :CIRCLE "v" :SELECT "m"
             :held (fn [i]
                     (. key-map i))
             })

(fennel.dofile "main.fnl")


(fn love.load []
  (PS2PROG.start))
  

(fn love.update [dt]
  (PS2PROG.frame))
  
(fn love.draw []
  (D2D:doLoveDraw))


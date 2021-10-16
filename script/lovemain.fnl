(local fennel (require "fennel"))

(local game (require "game"))
(local D2D (require "draw2d"))
(local T (require "text"))
(local E (require "events"))
(local PLAYER (require "player"))

(var events [])

(var key-map {
              :up false
              :down false
              :left false
              :right false
              })

(fn love.keypressed [key]
  (if (= key "w") (set key-map.up true))
  (if (= key "a") (set key-map.left true))
  (if (= key "d") (set key-map.right true))
  (if (= key "s") (set key-map.down true))
  (if (= key "escape") (love.event.quit 0)))

(fn love.keyreleased [key]
  ;;(print (.. "[UP] " key))
  (if (= key "w") (set key-map.up false))
  (if (= key "a") (set key-map.left false))
  (if (= key "d") (set key-map.right false))
  (if (= key "s") (set key-map.down false)))


(global PS2PROG {})

(global DMA {
             :init (fn [] nil)
             :GIF 0
             })

(global GS {
            :newState (fn [w h i mode] {
                                        :setBuffers (fn [fb zb] nil)
                                        :clearColour (fn [r g b a] nil)
                                        })
            })

(global PAD {
             :UP "up" :LEFT "left" :DOWN "down" :RIGHT "right"
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


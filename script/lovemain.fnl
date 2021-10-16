
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
  (print (.. "[DOWN] " key))
  (if (= key "w") (set key-map.up true)
    (= key "a") (set key-map.left true)
    (= key "d") (set key-map.right true)
    (= key "s") (set key-map.down true)))

(fn love.keyreleased [key]
  (print (.. "[UP] " key))
  (if (= key "w") (set key-map.up false)
    (= key "a") (set key-map.left false)
    (= key "d") (set key-map.right false)
    (= key "s") (set key-map.down false)))

(fn get-events []
  (each [k v (pairs key-map)]
    (print k v))
  (let [ev []]
    (if key-map.up (table.insert ev E.up)
        key-map.down (table.insert ev E.down)
        key-map.left (table.insert ev E.left)
        key-map.right (table.insert ev E.right))
    ev))

(var state {})

(fn love.load []
  (set state (game.new-state))
  (PLAYER.new state -20 -50 255 0 0 E.right)
  (PLAYER.new state -200 -10 0 255 0 E.right)
  (PLAYER.new state 300 -10 100 40 100 E.left))


(fn love.update [dt]
  (let [events (get-events)]
    (set state (game.update dt state events))
    (D2D:setColour 0xff 0xff 0xff 0x80)
    (set state.colliders [])))

(fn love.draw []
  (love.graphics.print "hello world" 100 100)
  (game.draw state))


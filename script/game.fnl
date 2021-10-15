
(local D2D (require "draw2d"))
(local E (require "events"))

(local game {})

(fn new-entity [state create update draw]
  (let [t (create)]
    (set t.update update)
    (set t.draw draw)
    (table.insert state.entities t)))

(fn game.new-state []
  { :entities [] 
    : new-entity })

(fn game.update [dt state events]
  (each [i ent (ipairs state.entities)]
    (ent:update dt state events))
  state)

(fn game.draw [state]
  (each [i ent (ipairs state.entities)]
    (ent:draw)))

game

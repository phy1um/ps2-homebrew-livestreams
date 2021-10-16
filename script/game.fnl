
(local D2D (require "draw2d"))
(local E (require "events"))

(local game {})

(fn new-entity [state create update draw]
  (let [t (create)]
    (set t.update update)
    (set t.draw draw)
    (set t.id (state:next-id))
    (table.insert state.entities t)))

(fn add-col [state col]
  (table.insert state.colliders col))

(fn game.new-state []
  { :entities [] 
    :colliders []
    :next-id (fn [state] 
               (let [ n state.id-state ]
                 (set state.id-state (+ state.id-state 1))
                 n))
    : add-col
    : new-entity
    :id-state 100 })

(fn game.update [dt state events]
  (each [i ent (ipairs state.entities)]
    (ent:update dt state events))
  (each [i col (ipairs state.colliders)]
    (each [i ent (ipairs state.entities)]
      (let [{:hit hit :action act} (col ent)]
        (if (= true hit) (act ent)))))
  state)

(fn game.draw [state]
  (each [i ent (ipairs state.entities)]
    (ent:draw)))

game


(local D2D (require "draw2d"))
(local E (require "events"))

(local game {})

(fn spawn [state spawn]
  (let [t (spawn)]
    (set t.id (state:next-id))
    (table.insert state.entities t)
    t))

(fn add-col [state col]
  (table.insert state.colliders col))

(fn push [this-state next-state]
  (set next-state.next-state this-state)
  next-state)

(fn pop [this-state]
  this-state.next-state)

(fn game.new-state []
  { :entities [] 
    :colliders []
    :id-state 100 
    :next-id (fn [state] 
               (let [ n state.id-state ]
                 (set state.id-state (+ state.id-state 1))
                 n))
    : add-col
    : spawn 
    : push
    : pop
    :next-state nil
  })

(fn game.update [dt state events]
  (let [new-entities 
        (icollect [_ ent (ipairs state.entities)]
          (ent:update dt state events))
        ]
    (each [i col (ipairs state.colliders)]
      (each [i ent (ipairs new-entities)]
        (if (= ent.solid true)
          (let [{:hit hit :action act} (col ent)]
            (if (= true hit) (act ent))))))
    {
      :entities new-entities
      :colliders []
      :next-id state.next-id
      :id-state state.id-state
      : add-col
      : spawn
      : push
      : pop
     }))

(fn game.draw [state]
  (each [i ent (ipairs state.entities)]
    (ent:draw)))

game

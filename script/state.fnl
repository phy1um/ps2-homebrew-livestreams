

(local state {})

(fn spawn [state spawn]
  (let [t (spawn)]
    (set t.id (state:next-id))
    (if (= nil t.x) (set t.x 0))
    (if (= nil t.y) (set t.y 0))
    (table.insert state.entities t)
    t))

(fn add-col [state col]
  (table.insert state.colliders col))

(fn push [this-state next-state]
  (print "setting next state to " next-state)
  (set next-state.next-state this-state)
  next-state)

(fn pop [this-state]
  this-state.next-state)

(fn merge [s1 s2]
  (if (~= s2 nil) 
    (each [k v (pairs s2)]
      (tset s1 k v))))


(fn draw [state]
  (let [min-x (- state.view-x 24) min-y (- state.view-y 24)
        max-x (+ state.view-x state.view-width 24) max-y (+ state.view-y state.view-height 24)]
    (each [i ent (ipairs state.entities)]
      (if (and
            (> ent.x min-x)
            (<= ent.x max-x)
            (> ent.y min-y)
            (<= ent.y max-y))
          (ent:draw state)))))


(fn update [dt state events]
  (let [new-entities 
        (icollect [_ ent (ipairs state.entities)]
          (ent:update dt state events))
        ]
    (each [i col (ipairs state.colliders)]
      (each [i ent (ipairs new-entities)]
        (if (= ent.solid true)
          (let [{:hit hit :action act} (col ent)]
            (if (= true hit) (merge state (act ent)))))))
    (if 
      ; return next state if we have flagged to pop
      (= true state.do-pop) (do
                              (print "popping state to " state.next-state)
                              (state:pop))
      ; otherwise return the successor to this state
      {
        :entities new-entities
        :colliders []
        :next-id state.next-id
        :id-state state.id-state
        :next-state state.next-state
        : add-col
        : spawn
        : push
        : pop
        :update state.update 
        :draw state.draw
        :m state.m
        :view-x state.view-x
        :view-y state.view-y
        :view-width state.view-width
        :view-height state.view-height
       })))



(fn state.new-state []
  { :entities [] 
    :colliders []
    :do-pop false
    :id-state 100 
    :next-id (fn [state] 
               (let [ n state.id-state ]
                 (set state.id-state (+ state.id-state 1))
                 n))
    : add-col
    : spawn 
    : push
    : pop
    : update
    : draw
    :view-x 0 
    :view-y 0 
    :view-width 640
    :view-height 448
    :next-state nil
    :m nil
  })


state

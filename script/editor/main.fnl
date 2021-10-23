(local state (require "state"))
(local D2D (require "camera"))
(local E (require "events"))
(local room (require "room"))
(local T (require "text"))
(local EE (require "entity"))
(local fennel (require "fennel"))

(fn draw [me state] 
  (T.printLines 5 5 (.. "Editing " state.m.name " @ " state.m.active-room.ox "," state.m.active-room.oy))
  (each [_ s (ipairs state.m.active-room.entity-spawns)]
    (D2D:rect s.x s.y 16 16)
    (T.printLines (- s.x D2D.cx) (- s.y D2D.cy) s.class)))

(fn update [me dt state events] 
  (each [_ e (ipairs events)]
    (if
      (E.is e E.type.menu E.mod.press)
      (do
        (print "go to game state")
        (let [menu (. (reload "editor/menu") "new")
              old-update state.update]
          (set state.update (fn [_ state]
                              (let [ns (state:push (menu state state.m.cursor me))]
                                (set state.update old-update)
                                ns)))))))

  me)

(fn add-room [m dir]
  (if (~= (. m.active-room dir) nil)
    (let [rm (. m.room-map (. m.active-room dir))]
      (if (~= rm nil)
        (set m.active-room (. m.room-map (. m.active-room dir)))
        (print "room" dir "was nil!")))
    (let [dx (if
               (= dir "left") -640
               (= dir "right") 640
               0)
          dy (if
               (= dir "up") -448
               (= dir "down") 448
               0)
          nx (+ m.active-room.ox dx)
          ny (+ m.active-room.oy dy)
          opposite (if
                     (= dir "left") "right"
                     (= dir "right") "left"
                     (= dir "up") "down"
                     (= dir "down") "up")
          r (room.new-room nx ny 40 30)]
      (tset m.active-room dir r.id)
      (tset m.room-map r.id r)
      (tset r opposite m.active-room.id)
      (set m.active-room r)
      (print "set room tgt " r.ox r.oy))))




(fn controller []
  { : update : draw 
   :add-room (fn [self dir] (add-room self.m dir))
   :save (fn [self] 
           (let [f (io.open "xx.fnl" "w")
                 rooms (collect [k r (pairs self.m.room-map)]
                                  (values k (r:minify)))]
             (f:write (fennel.view rooms))
             (f:close)))
   })

(fn area-fill [room fx fy tx ty v]
  (let [dx (if (> tx fx) 1 -1)
        dy (if (> ty fy) 1 -1)]
    (for [i fx tx dx]
      (for [j fy ty dy]
        (room:tile-set i j v)))))

(fn area-ol [room fx fy tx ty v]
  (let [dx (if (> tx fx) 1 -1)
        dy (if (> ty fy) 1 -1)]
    (for [i fx tx dx]
      (for [j fy ty dy]
        (if (or (= i fx) (= j fy) (= i tx) (= j ty))
          (room:tile-set i j v))))))



(local cursor-actions {
                       :tile (fn [me state] (print "add") (state.m.active-room:tile-set me.x me.y me.active))
                       :entity (fn [me state] 
                                 (let [r state.m.active-room
                                       cname (EE.get-class me.active)
                                       rx r.ox
                                       ry r.oy
                                       x (+ rx (* 16 me.x))
                                       y (+ ry (* 16 me.y))]
                                   (r:add-entity-spawn {: x : y :class cname})))

                       :area (fn [me state] 
                               (if (~= nil me.area-first)
                                 (print me.area-first.x me.area-first.y)
                                 (print "nil area - picking?"))
                               (if
                                (~= me.area-first nil)
                                (do
                                  (area-fill state.m.active-room me.area-first.x me.area-first.y me.x me.y me.active)
                                  (set me.area-first nil))
                                (set me.area-first {:x me.x :y me.y})))
                       })

(local cursor-alt-actions {
                       :tile (fn [me state] (print "remove") (state.m.active-room:tile-set me.x me.y 0))
                       :entity (fn [me state] )
                       :area (fn [me state] 
                               (if (~= nil me.area-first)
                                 (print me.area-first.x me.area-first.y)
                                 (print "nil area - picking?"))
                               (if
                                (~= me.area-first nil)
                                (do
                                  (area-ol state.m.active-room me.area-first.x me.area-first.y me.x me.y me.active)
                                  (set me.area-first nil))
                                (set me.area-first {:x me.x :y me.y})))
                       })


(fn cursor-update [me dt state events]
  (each [_ e (ipairs events)]
    (if 
      (E.is e E.type.left E.mod.press)
        (set me.x (- me.x 1))
      (E.is e E.type.right E.mod.press)
        (set me.x (+ me.x 1))
      (E.is e E.type.up E.mod.press)
        (set me.y (- me.y 1))
      (E.is e E.type.down E.mod.press)
        (set me.y (+ me.y 1))
      (E.is e E.type.a0 E.mod.press)
        (let [a (. cursor-actions me.mode)]
          (a me state))
      (E.is e E.type.a1 E.mod.press)
        (let [a (. cursor-alt-actions me.mode)]
          (a me state))
      (E.is e E.type.l1 E.mod.press)
        (set me.active (math.max 0 (- me.active 1)))
      (E.is e E.type.r1 E.mod.press)
        (set me.active (math.min 100 (+ me.active 1)))))
  me)

(fn cursor-draw [me]
  (D2D:setColour 255 0 255 0x20)
  (D2D:rect (* me.x 16) (* me.y 16) 16 16)
  (if (and (= me.mode "area") (~= nil me.area-first))
    (D2D:rect (* me.area-first.x 16) (* me.area-first.y 16) 16 16))
  (D2D:setColour 255 255 255 0x80)
  (T.printLines (* me.x 16) (- (* me.y 16) 28) 
                (.. me.mode " :: " me.active)))


(fn cursor []
  { :update cursor-update
   :draw cursor-draw 
   :mode "tile"
   :area-first nil
   :set-mode (fn [self x] 
               (set self.mode x)
               (set self.area-first nil))
   :x 5 :y 5 :active 1})


(fn new [name]
  (let [ed (state.new-state)]
    (ed:spawn room.tile-draw)
    (let [cursor (ed:spawn cursor)
          ctrl (ed:spawn controller)]
      (set ed.m {
        :w 40 :h 30
        :active-room nil
        :room-map {}
        :name (if (= name nil) "test-zone" name)
        : cursor
                 })
      (set ctrl.m ed.m)
      (let [r (room.new-room 0 0 40 30)]
        (set ed.m.active-room r)
        (tset ed.m.room-map r.id r))
      ed)))

{
 : new
 }

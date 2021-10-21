(local state (require "state"))
(local D2D (require "camera"))
(local E (require "events"))
(local room (require "room"))

(fn draw [me state])
(fn update [me dt state events] me)

(fn controller []
  { : update : draw })


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
        (state.m.active-room:tile-set me.x me.y me.active)
      (E.is e E.type.a1 E.mod.press)
        (state.m.active-room:tile-set me.x me.y 0)))
  me)

(fn cursor-draw [me]
  (D2D:setColour 255 0 255 0x20)
  (D2D:rect (* me.x 16) (* me.y 16) 16 16))


(fn cursor []
  { :update cursor-update
   :draw cursor-draw 
   :x 5 :y 5 :active 6})


(fn new []
  (let [ed (state.new-state)]
    (ed:spawn room.tile-draw)
    (ed:spawn controller)
    (ed:spawn cursor)
    (set ed.m {
      :w 30 :h 40
      :active-room nil
      :room-map []
               })
    (let [r (room.new-room 0 0 30 40)]
      (set ed.m.active-room r)
      (tset ed.m.room-map r.id r))
    ed))

{
 : new
 }

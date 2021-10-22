(local T (require "text"))
(local D2D (require "draw2d"))
(local E (require "events"))
(local state (require "state"))

(var *debounce-time* 0.07)

(fn ed-menu [ed cursor controller]
  (fn []
  {
    :options [
              "Go left"
              "Go right"
              "Go up"
              "Go down"
              "Mode tile"
              "Mode entity"
              "Mode player"
              "Mode area"
              "Save"
              "Test"
              "Back"
            ]
    :actions [
              ; move room direction
              (fn [state] (controller:add-room "left"))
              (fn [state] (controller:add-room "right"))
              (fn [state] (controller:add-room "up"))
              (fn [state] (controller:add-room "down"))
              ; change cursor mode
              (fn [state] (cursor:set-mode "tile"))
              (fn [state] (cursor:set-mode "entity"))
              (fn [state] (cursor:set-mode "player"))
              (fn [state] (cursor:set-mode "area"))
              ; save room (?)
              (fn [state] (controller:save))
              ; test
              (fn [state] 
                (print "go to game state")
                (let [game (. (reload "game") "new")
                      old-update state.update]
                  (set state.update (fn [_ state]
                                      (print "moving to game, loading rooms:")
                                      (each [id _ (pairs ed.m.room-map)]
                                        (print "got one " id))
                                      (let [ns (state:push (game 40 30 ed.m.room-map ed.m.active-room.id))]
                                        (set state.update old-update)
                                        ns)))))
              ; go back to prev state
              (fn [state] (set state.do-pop true))
          ]
    :cursor 1
    :debounce 0
    :update (fn [me dt state events] 
              (if (> me.debounce 0) 
                (do
                  (set me.debounce (- me.debounce dt))
                  me)
                (let [actions (icollect [_ ev (ipairs events)]
                                        (if 
                                          ; on cursor up
                                          (E.is ev E.type.up E.mod.press)
                                          (fn [] 
                                            ;(print "move cursor " me.cursor (- me.cursor 1))
                                            (set me.cursor (math.max 1 (- me.cursor 1))))
                                          ; on cursor down
                                          (E.is ev E.type.down E.mod.press)
                                          (fn [] 
                                            (set me.cursor (math.min 
                                                             (length me.options) 
                                                             (+ me.cursor 1))))
                                          ; on action
                                          (E.is ev E.type.a0 E.mod.press)
                                            (. me.actions me.cursor)
                                          (E.is ev E.type.menu E.mod.press)
                                            (set state.do-pop true)))]
                      (each [i a (ipairs actions)]
                        ;(print "acting?")
                        (if (~= a nil) 
                          (do
                            (set me.debounce *debounce-time*)
                            (a state))))
                      me)))

    :draw (fn [me]
            (D2D:setColour 255 255 255 0x80)
            (T.printLines 120 124 (table.unpack me.options))
            (T.printLines 120 400 (.. "D-Pad to select, " T.x " to confirm"))
            (D2D:setColour 255 255 0 0x80)
            (D2D:rect 105 (+ 126 2 (* 16 me.cursor)) 8 8))
   }))

(fn menu-bg []
  {
   :update (fn [me dt state events] me)
   :draw (fn [me])
           ;(D2D:setColour 30 80 80 0x80)
           ;(D2D:rect -320 -240 640 480))
   })

(fn new [ed cursor controller]
  (let [menu (state.new-state)]
    (menu:spawn menu-bg)
    (menu:spawn (ed-menu ed cursor controller))
    menu))

{
 : new
 }

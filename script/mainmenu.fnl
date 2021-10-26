(local T (require "text"))
(local D2D (require "draw2d"))
(local E (require "events"))
(local state (require "state"))
(local F (require "flags"))
(local fennel (require "fennel"))
(local room (require "room"))
(local credits (require "credits"))
(local tutorial (require "tutorial"))
(local ed (require "editor/main"))
(local game (require "game2"))

(var *debounce-time* 0.07)

(fn menu-controller []
  {
    :options [
              "New Game"
              "How to Play"
              "Credits"
            ]
    :actions [
              (fn [state] 
                (print "go to game state")
                (let [game game.new 
                      old-update state.update]
                  (set state.update (fn [_ state]
                                      (let [ns (state:push (game 40 30))]
                                        (set state.update old-update)
                                        ns)))))
              
              (fn [state] 
                (print "show tutorial")
                (let [tut tutorial.new
                      old-update state.update]
                  (set state.update (fn [_ state]
                                      (let [ns (state:push (tut))]
                                        (set state.update old-update)
                                        ns)))))


              (fn [state]                 
                (print "show credits")
                (let [credits credits.new
                      old-update state.update]
                  (set state.update (fn [_ state]
                                      (let [ns (state:push (credits))]
                                        (set state.update old-update)
                                        ns)))))

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
                                            (print "move cursor " me.cursor (- me.cursor 1))
                                            (set me.cursor (math.max 1 (- me.cursor 1))))
                                          ; on cursor down
                                          (E.is ev E.type.down E.mod.press)
                                          (fn [] 
                                            (set me.cursor (math.min 
                                                             (length me.options) 
                                                             (+ me.cursor 1))))
                                          ; on action
                                          (E.is ev E.type.a0 E.mod.press)
                                            (. me.actions me.cursor)))]
                      (each [i a (ipairs actions)]
                        (print "acting?")
                        (if (~= a nil) 
                          (do
                            (set me.debounce *debounce-time*)
                            (a state))))
                      me)))

    :draw (fn [me]
            (D2D:setColour 255 255 255 0x80)
            (T.printLines 120 124 (table.unpack me.options))
            (T.printLines 120 220 (.. "D-Pad to select, " T.x " to confirm"))
            (D2D:setColour 255 255 0 0x80)
            (D2D:rect 105 (+ 126 2 (* 16 me.cursor)) 8 8))
   })

(fn menu-bg []
  {
   :update (fn [me dt state events] me)
   :draw (fn [me])
           ;(D2D:setColour 30 80 80 0x80)
           ;(D2D:rect -320 -240 640 480))
   })

(fn fallthrough-message []
  {
    :update (fn [me] me)
    :draw (fn []
            (D2D:setColour 255 255 255 0x80)
            (T.printLines 20 44 
                               "ERROR: somehow exited from the menu"
                               " this message should not be visible"
                               " please restart the program"
                               ""
                               " :)"))
   })

(fn new []
  (let [fallthrough (state.new-state) 
        menu (state.new-state)]

    (menu:spawn menu-bg)
    (let [mc (menu:spawn menu-controller)]
      (if (= true F.dev)
        (do
          (table.insert mc.options "Editor")
          (table.insert mc.actions (fn [state]
                                      (print "go to editor state")
                                      (let [editor ed.new
                                            old-update state.update]
                                        (set state.update (fn [_ state]
                                                            (let [ns (state:push (editor))]
                                                              (set state.update old-update)
                                                              ns)))))))))

    (fallthrough:spawn fallthrough-message)
    (fallthrough:push menu)))

{
 : new
 }

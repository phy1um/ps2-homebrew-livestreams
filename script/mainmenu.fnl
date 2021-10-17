(local T (require "text"))
(local D2D (require "draw2d"))
(local E (require "events"))
(local state (require "state"))

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
                (let [game (. (reload "game") "new")
                      old-update state.update]
                  (set state.update (fn [_ state]
                    (let [ns (state:push (game 40 30))]
                      (set state.update old-update)
                      ns)))))
              
              (fn [state] 
                (print "show tutorial")
                (let [tut (. (reload "tutorial") "new")
                      old-update state.update]
                  (set state.update (fn [_ state]
                                      (let [ns (state:push (tut))]
                                        (set state.update old-update)
                                        ns)))))


              (fn [state]                 
                (print "show credits")
                (let [credits (. (reload "credits") "new")
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
            (T.printLines -200 -100 (table.unpack me.options))
            (T.printLines -200 180 (.. "D-Pad to select, " T.x " to confirm"))
            (D2D:setColour 255 255 0 0x80)
            (D2D:rect -220 (+ -100 2 (* 16 me.cursor)) 8 8))
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
            (T.printLines -300 -200 
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
    (menu:spawn menu-controller)
    (fallthrough:spawn fallthrough-message)
    (fallthrough:push menu)))

{
 : new
 }
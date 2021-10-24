(local state (require "state"))
(local D2D (require "draw2d"))
(local T (require "text"))
(local E (require "events"))

(fn new []
  (let [s (state.new-state)]
       (s:spawn (fn []
                  {
                   :update (fn [me _ state events] 
                             (each [_ e (ipairs events)]
                               (if (E.is e E.type.a0 E.mod.press) (set state.do-pop true)))
                             me)
                   :draw (fn []
                          (D2D:setColour 255 255 255 0x80)
                            (T.printLines 10 20
                                          (.. "Move with the D-Pad, shoot with " T.x)
                                          "Kill the enemies, if you lose your HP you die"
                                          ""
                                          ""
                                          "This game was made for the Lisp Autumn Game Jam 2021, and I had many"
                                          "issues getting this working on the Playstation 2. I'm writing a "
                                          "postmortem on my website coding.tommarks.xyz"
                                          ""
                                          ""
                                          "If you edit the source files to enable the DEV flag there is "
                                          "a VERY work in progress level editor from when this game was "
                                          "about exploring dungeons with different rooms :)"
                                          ""
                                          "Sadly I had to cut almost everything I worked on because it"
                                          "wasn't coming together"
                                          ))
                    }
                  ))
       s))

{
 : new
 }


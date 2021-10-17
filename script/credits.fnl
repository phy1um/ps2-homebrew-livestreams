(local state (require "state"))
(local D2D (require "draw2d"))
(local T (require "text"))
(local E (require "events"))

(fn new []
  (let [s (state.new-state)]
       (s:spawn (fn []
                  {
                   :part 0
                   :part0 [
                          "Untitled Playstation 2 Game - Autumn Lisp Game Jam 2021"
                          "Created by Tom Marks"
                          (.. (string.char 9) " Website: https://coding.tommarks.xyz")
                          (.. (string.char 9) " Twitch: https://twitch.tv/phylum919")
                          (.. (string.char 9) " Youtube: Tom Marks Talks Code")
                          (.. (string.char 9) " Itch.io: https://tommarkstalkscode.itch.io")
                           ]
                   :part1 [
                          "Assets Used:"
                          ""
                          " 1. Spleen Bitmap Font - github.com/fcambus/spleen"
                          ""
                          "All other code and assets were created by Tom Marks. They are licensed under the MIT license"
                          "which was distributed with this game in the file LICENSE. If you have obtained this game without"
                          "the license you have not obtained it from me!"
                          ""
                          ""
                          ""
                          "Thanks for playing :)"
                          (.. " - " (string.char 20) (string.char 21) (string.char 22) )
                           ]
                   :update (fn [me _ state events] 
                             (each [_ e (ipairs events)]
                               (if (E.is e E.type.a0 E.mod.press) (set me.part (+ me.part 1))))
                             (if (> me.part 1) 
                                  (set state.do-pop true))
                             me)
                   :draw (fn [me]
                          (D2D:setColour 255 255 255 0x80)
                          (let [t (if (= 0 me.part) me.part0
                                      (= 1 me.part) me.part1
                                      [""])]
                            (T.printLines -300 -200 (table.unpack t))))
                    }
                  ))
       s))

{
 : new
 }


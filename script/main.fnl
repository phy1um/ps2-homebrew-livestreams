
;; (local GIF (require "gif"))

(print "running main fennel file")

(local D2D (require "draw2d"))
(local VRAM (require "vram"))

(print "loading utils")
(local T (require "text"))

(var gs nil)
(var font nil)

(print "overriding start function")
(fn PS2PROG.start []
  (set font (D2D.loadTexture "host:bigfont.tga" 256 64))
  (DMA.init DMA.GIF)
  (set gs (GS.newState 640 448 GS.INTERLACED GS.NTSC))
  (let [fb (VRAM.buffer 640 448 GS.PSM24 256)
        zb (VRAM.buffer 640 448 GS.PSMZ24 256)]
    (gs:setBuffers fb zb)
    (gs:clearColour 0x2b 0x2b 0x2b)))

(print "overriding on-frame handler")
(fn PS2PROG.frame []
  (D2D:newBuffer)
  (let [db (D2D:getBuffer)]
    (db:frameStart gs)
    (D2D:setColour 0xff 0xff 0xff 0x80)
    ;;(D2D:sprite font 50 100 256 64 0 0 1 1)
    (T.printLines font -200 -200
                "Untitled PS2 Game"
                "Built for Lisp Autumn Jam 2021"
                "(c) Tom Marks 2021"
                " visit coding.tommarks.xyz"
                ""
                (.. "Press the " T.x " button to continue"))
    (let [db (D2D:getBuffer)]
      (db:frameEnd gs)
      (D2D:kick)))
  (print "tri/frame = " D2D.rawtri ", KC = " D2D.kc)
  (set D2D.rawtri 0)
  (set D2D.kc 0))

(print "ended main fennel file")

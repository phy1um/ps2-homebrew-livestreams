
;; (local GIF (require "gif"))

(local D2D (require "draw2d"))
(var gs nil)
(var font nil)

(var *char-x* (string.char 0))
(var *char-square* (string.char 1))
(var *char-circle* (string.char 2))
(var *char-triangle* (string.char 3))

(fn getCharInd [c]
  (if (and (>= c 33) (<= c 126))
        ;; standard ASCII value, offset for our table
        (- c 32)
      (and (>= c 0) (<= c 32))
        (+ 96 c)
      0))

(fn drawString [s x y]
  (if (> (length s) 0)
    (let [ci (getCharInd (string.byte s))
        ts (* (% ci 32) 0.03125)
        tt (* (math.floor (/ ci 32)) 0.25)]
      (D2D:sprite font x y 8 16 ts tt (+ ts 0.03125) (+ tt 0.25))
      (drawString (string.sub s 2) (+ x 8) y))))

(fn printLines [x y ...]
  (each [i v (ipairs [...])]
    (print v)
    (drawString v x (+ y (* i 16)))))

(fn PS2PROG.start []
  (set font (D2D.loadTexture "host:bigfont.tga" 256 64))
  (DMA.init DMA.GIF)
  (set gs (GS.newState 640 448 GS.INTERLACED GS.NTSC))
  (let [fb (gs:alloc 640 448 GS.PSM24)
        zb (gs:alloc 640 448 GS.PSMZ24)]
    (gs:setBuffers fb zb)
    (gs:clearColour 0x2b 0x2b 0x2b)))

(fn PS2PROG.frame []
  (D2D:newBuffer)
  (let [db (D2D:getBuffer)]
    (db:frameStart gs)
    (D2D:setColour 0xff 0xff 0xff 0x80)
    ;;(D2D:sprite font 50 100 256 64 0 0 1 1)
    (printLines -60 -200 
      "Lisp Autumn Jam 2021"
      "  A Playstation 2 Game by"
      "  TOM MARKS"
      " (C) 2021"
      ""
      " coding.tommarks.xyz"
      (.. "" *char-x* *char-square* *char-triangle* *char-circle*))
    (let [db (D2D:getBuffer)]
      (db:frameEnd gs)
      (D2D:kick)))
  (print "tri/frame = " D2D.rawtri ", KC = " D2D.kc)
  (set D2D.rawtri 0)
  (set D2D.kc 0))


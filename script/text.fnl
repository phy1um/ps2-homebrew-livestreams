(local D2D (require "draw2d"))
(local u {
  "x" (string.char 0)
  "square" (string.char 1)
  "circle" (string.char 2)
  "triangle" (string.char 3)
})

(fn getCharInd [c]
  (if (and (>= c 33) (<= c 126))
        ;; standard ASCII value, offset for our table
        (- c 32)
      (and (>= c 0) (<= c 32))
        (+ 96 c)
      0))

(fn drawString [font s x y]
  (if (> (length s) 0)
    (let [ci (getCharInd (string.byte s))
        ts (* (% ci 32) 0.03125)
        tt (* (math.floor (/ ci 32)) 0.25)]
      (D2D:sprite font x y 8 16 ts tt (+ ts 0.03125) (+ tt 0.25))
      (drawString font (string.sub s 2) (+ x 8) y))))

(fn u.printLines [font x y ...]
  (each [i v (ipairs [...])]
    (print v)
    (drawString font v x (+ y (* i 16)))))

u

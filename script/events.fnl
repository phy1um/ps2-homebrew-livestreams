
(fn event [t m]
  {:type t :mod m})

(fn is [e t m]
  ; (print "test " e.type " == " t ", " e.mod " == " m)
  (and (= e.type t) (= e.mod m)))

{
  :type {:up 0
        :down 1
        :left 2
        :right 3
        :a0 4
        :a1 5
        :a2 6
        :a3 7
        :menu 8
        :reload 9
        :l1 10
        :l2 11
        :r1 12
        :r2 13
        }
  :mod {:press 0
        :release 1
        :hold 2
        }
  : event
  : is
}

(local D2D (require "draw2d"))

(fn get-uv [w h i]
  (let [col (% i w)
        row (math.floor (/ i w))
        dx (/ 1 w)
        dy (/ 1 h)]
    { :u1 (* dx col) :u2 (* dx (+ col 1))
            :v1 (* dy row) :v2 (* dy (+ row 1)) }))


{
  :chars nil
  :font nil
  :tiles nil
  :load (fn [self]
          (let [chars (D2D.loadTexture "host:characters.tga" 256 64)
                font (D2D.loadTexture "host:bigfont.tga" 256 64)
                tiles (D2D.loadTexture "host:tiles.tga" 256 64)]
              (set self.chars chars)
              (set self.font font)
              (set self.tiles tiles)))
  : get-uv
 }

(local D2D (require "draw2d"))

(fn get-uv [w h i]
  (let [col (% i w)
        row (math.floor (/ i w))
        dx (/ 1 w)
        dy (/ 1 h)]
    { :u1 (* dx col) :u2 (* dx (+ col 1))
            :v1 (* dy row) :v2 (* dy (+ row 1)) }))

(fn rect-to-uv [x y w h texw texh]
  {
   :u1 (/ x texw) 
   :v1 (/ y texh)
   :u2 (/ (+ x w) texw)
   :v2 (/ (+ y h) texh)
  })
  

(local player-frames
  {
   :down [(rect-to-uv 0 0 16 16 256 64)
          (rect-to-uv 0 16 16 16 256 64)
          (rect-to-uv 0 0 16 16 256 64)
          (rect-to-uv 0 32 16 16 256 64)]
   :up [(rect-to-uv 16 0 16 16 256 64)
          (rect-to-uv 16 16 16 16 256 64)
          (rect-to-uv 16 0 16 16 256 64)
          (rect-to-uv 16 32 16 16 256 64)]
   :right [(rect-to-uv 32 0 16 16 256 64)
          (rect-to-uv 32 16 16 16 256 64)
          (rect-to-uv 32 0 16 16 256 64)
          (rect-to-uv 32 32 16 16 256 64)]
   :left [(rect-to-uv 48 16 16 16 256 64)
          (rect-to-uv 48 16 16 16 256 64)
          (rect-to-uv 48 0 16 16 256 64)
          (rect-to-uv 48 32 16 16 256 64)]
   })

(local bat-frames
  {
  :move [
         (rect-to-uv 64 24 28 14 256 64)
         (rect-to-uv 92 24 28 14 256 64)]
  })

(local crow-frames
  {
   :move [
          (rect-to-uv 64 0 16 24 256 64)
          (rect-to-uv 80 0 16 24 256 64)
          (rect-to-uv 96 0 16 24 256 64)]
   })


{
  :chars nil
  :font nil
  :tiles nil
  :load (fn [self]
          (let [chars (D2D.loadTexture "host:characters.tga" 256 64)
                font (D2D.loadTexture "host:bigfont.tga" 256 64)]
                
              (set self.chars chars)
              (set self.font font)
              (set self.tiles nil)))
  : get-uv
  : player-frames
  : bat-frames
  : crow-frames
 }

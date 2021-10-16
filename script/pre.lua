
function writeToBuffer(b, ints)
  for i=1,#ints,1 do
    b:pushint(ints[i])
  end
end




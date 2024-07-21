
local SL = require"p2g.slotlist"
local test = require"p2g.test"

local suite = {name = "Slot List"}

function suite.sl_init()
  local sl = SL.new(10)
end

function suite.sl_push()
  local sl = SL.new(10)
  local x = 5
  sl:push(5)
end

function suite.sl_each()
  local sl = SL.new(10)
  sl:push(5, 1)
  sl:push(6, 2)
  local c = 0
  sl:each(function(v, state, i)
    if i == 0 then
      test.equal(v, 5)
      test.equal(state, 1)
    elseif i == 1 then
      test.equal(v, 6)
      test.equal(state, 2)
    else
      test.fail("unexpected index " .. i)
    end
    c = c + 1
  end)
  test.equal(c, 2)
end

function suite.sl_each_state()
  local sl = SL.new(10)
  sl:push(5, 1)
  sl:push(6, 2)
  sl:eachState(2, function(v, state, i)
    if i == 1 then
      test.equal(v,  6)
      test.equal(state, 2)
    else
      test.fail("unexpected index " .. i)
    end
  end)
end

function suite.sl_clear()
  local sl = SL.new(10)
  for i=1,6,1 do
    sl:push(i, 3)
  end
  sl:each(function(v, state, i)
    test.equal(v, i+1) 
    sl:setState(i, 1)
  end)
  local c = 0
  sl:eachState(1, function()
    c = c + 1
  end)
  test.equal(c, 6)
end

return suite

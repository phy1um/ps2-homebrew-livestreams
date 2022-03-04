
local test = {}

function test.handler(testName)
  return function(err)
    LOG.error(" % Failed Test (" .. testName .. ") :: " .. err)
  end
end

function test.run_suite(name, t)
  local ran = 0
  local pass = 0
  LOG.info("Running suite " .. name)
  for name,fn in pairs(t) do
    if type(fn) == "function" then 
      ran = ran + 1
      local rv = xpcall(fn, test.handler(name))
      if rv then pass = pass + 1 end
    end
  end
  local st = "PASS"
  if pass < ran then st = "FAIL" end
  LOG.info(" === " .. st .. " " .. name)
  LOG.info( " @ " .. tostring(pass) .. " / " .. tostring(ran))
  return pass == ran
end

function test.equal(exp, actual)
  if (exp == actual) == false then
    error("expected: " .. tostring(exp) .. ", actual: " .. tostring(actual))
  end
end


return test

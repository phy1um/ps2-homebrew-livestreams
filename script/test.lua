
local test = {}

function test.handler(err)
  LOG.error("TEST FAILED: " .. err)
end

function test.run_suite(name, t)
  local ran = 0
  local pass = 0
  LOG.info("Running suite " .. name)
  for name,fn in pairs(t) do
    if type(fn) == "function" then 
      ran = ran + 1
      local rv = xpcall(fn, test.handler)
      if rv then pass = pass + 1 end
    end
  end
  local st = "PASS"
  if pass < ran then st = "FAIL" end
  LOG.info(" === " .. st .. " " .. name)
  LOG.info( " @ " .. tostring(pass) .. " / " .. tostring(ran))
  return pass == ran
end

return test

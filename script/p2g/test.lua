
local LOG = require"p2g.log"

local test = {}

local default_handlers = {
  failed = function(suite_name, test_name, err)
    LOG.error(" % Failed Test (" .. test_name .. ") :: " .. err)
  end,
  passed = function(suite_name, test_name) end,
}


function test.handler(testName)
  return function(err)
    LOG.error(" % Failed Test (" .. testName .. ") :: " .. err)
  end
end

function test.run_suite(name, t, handlers)
  local handlers = handlers or default_handlers
  local ran = 0
  local pass = 0
  for test_name,fn in pairs(t) do
    if type(fn) == "function" then 
      ran = ran + 1
      local rv = xpcall(fn, function(err) handlers.failed(name, test_name, err) end)
      if rv then 
        handlers.passed(name, test_name)
      end
    end
  end
end

function test.equal(exp, actual)
  if (exp == actual) == false then
    error("expected: " .. tostring(exp) .. ", actual: " .. tostring(actual))
  end
end

function test.fail(msg)
  error(msg)
end

return test

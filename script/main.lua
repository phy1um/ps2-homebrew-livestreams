
local LOG = require"p2g.log"
local TEST = require"p2g.test"

local entrypoint = "eg.cube"

test_record = {}

local test_files = {
  "test_vec2", 
  "test_vec3", 
  "test_mat3x3", 
  "test_mat4x4", 
  "test_slotlist",
}
function run_unit_tests()
  local ok = true
  local any_fail = false
  for _, name in ipairs(test_files) do
    local suite = require("test." .. name)
    local fail = false
    local record = {}
    LOG.info("== Running suite " .. suite.name)
    TEST.run_suite(suite.name, suite, {
      passed = function(s, case) 
        table.insert(record, {
          ok = true,
          test_name = case,
        })
      end,
      failed = function(s, case, err)
        fail = true
        any_fail = true
        LOG.error("  !! " .. case .. ": " .. tostring(err))
        table.insert(record, {
          ok = false,
          test_name = case,
          err = err, 
        })
      end,
    })
    test_record[suite.name] = record
    if fail then 
      LOG.error("!! Tests failed !!")
    end
  end
  if any_fail then
    -- error("test failures")
    -- currently having segfault issues with pcall?
    -- so do this instead
    return true
  end
end

if run_unit_tests() == true then
  LOG.error("invalid startup: unit tests failed")
  entrypoint = "eg.tests"
end

LOG.info("loading entrypoint")

require(entrypoint)
return {}

-- default entrypoint - do nothing and hang!



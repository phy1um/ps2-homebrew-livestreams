
local LOG = require"p2g.log"

local startup_ok = true

local test_files = {"test_vec2", "test_mat3x3", "test_vec3", "test_slotlist"}
function run_unit_tests()
  local ok = true
  for _, name in ipairs(test_files) do
    local fn = require("test." .. name)
    if fn() == false then 
      ok = false 
    end
  end
  if not ok then error("failed unit tests!") end
end

if pcall(run_unit_tests) ~= true then
  LOG.error("failed to run unit tests")
end

LOG.info("loading entrypoint")

require("eg.math")
return {}

-- default entrypoint - do nothing and hang!



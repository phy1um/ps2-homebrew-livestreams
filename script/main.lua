
local test_files = {"test_vec2", "test_mat3x3"}
function run_unit_tests()
  for _, name in ipairs(test_files) do
    local fn = require("test." .. name)
    fn()
  end
end

run_unit_tests()

require("eg.math")
return {}

-- default entrypoint - do nothing and hang!




return { init = function()
  local H = {
    truerequire = require,
    cache = {}, 
    -- '_' == 95
    ignoreChar = 95,
  }


  function H.require(p)
    local m = H.cache[p]
    if m == nil then
      m = H.truerequire(p)
      H.cache[p] = m
    end
    return m
  end

  function H.reload(p)
    local m = H.cache[p]
    if m == nil then return end
    package.loaded[p] = nil
    local newmod = H.truerequire(p)
    for k, v in pairs(newmod) do
      -- do not copy keys that start with some character
      if string.byte(k, 1) ~= H.ignoreChar then
        LOG.info("patching module " .. p .. ": " .. k)
        m[k] = v
      else
        if type(v) == "function" then
          LOG.info("mod call: " .. k)
          v()
        else
          LOG.info("skip non func: " .. type(v))
        end
      end
    end
  end

  return H.require, H.reload
end}

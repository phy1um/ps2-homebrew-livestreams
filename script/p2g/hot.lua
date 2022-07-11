
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
    
    local pre = newmod["_reload_before"]
    if type(pre) == "function" then
      LOG.trace("calling reload_before on " .. p)
      pre(m)
    end

    for k, v in pairs(newmod) do
      -- do not copy keys that start with some character
      if string.byte(k, 1) ~= H.ignoreChar then
        LOG.trace("patching module " .. p .. ": " .. k)
        m[k] = v
      end
    end

    local post = newmod["_reload_after"]
    if type(post) == "function" then
      LOG.trace("calling reload_after on " .. p)
      post(m)
    end
  end

  return H.require, H.reload
end}

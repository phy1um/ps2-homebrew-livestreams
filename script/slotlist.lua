

local list = {
  new = function(n)
    LOG.info("new list")
    return CORE_SLOT_LIST.new(n)
  end
}

return list

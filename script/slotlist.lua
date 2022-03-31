

local list = {
  new = function(n)
    LOG.info("new list")
    return P2GCORE.slotlist.new(n)
  end
}

return list

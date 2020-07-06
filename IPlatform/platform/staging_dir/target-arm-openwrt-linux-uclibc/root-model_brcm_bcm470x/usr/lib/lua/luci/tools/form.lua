--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  form.lua
Details :  Form manipulation class
Author  :  Ye Qianchuan <yeqianchuan@tp-link.net>
Author  :  Wen Kun <wenkun@tp-link.net>
Version :  1.0.0
Date    :  5 Mar, 2014
]]--

module("luci.tools.form", package.seeall)

local util = require "luci.util"
local json = require "luci.json"

Form = util.class()

function Form:__init__(u, k)
    self.uci_r = u
    self.keys = k
end

--- Find target item.
-- @param cfg		UCI config name
-- @param stype		UCI section type
-- @param target	Target item
-- @param keys		Use to match items (optional)
-- @return			Section name of the target item
function Form:_find_item(cfg, stype, target, keys)
    local uci_r = self.uci_r
    keys = keys or self.keys

    local sname
    uci_r:foreach(cfg, stype,
                  function (section)
                      if keys then
                          for _, k in pairs(keys) do
                              if target[k] ~= section[k] then
                                  return
                              end
                          end
                      else
                          local tic = 0
                          for k, v in pairs(target) do
                              tic = tic + 1
                              if v ~= section[k] then
                                  return
                              end
                          end

                          local sic = 0
                          for k, _ in pairs(section) do
                              if k:sub(1, 1) ~= "." then
                                  sic = sic + 1
                              end
                          end

                          if tic ~= sic then
                              return
                          end
                      end

                      -- Found it.
                      sname =  section[".name"]
                      return false
                  end
    )

    return sname
end

--- Insert a form item.
-- @param cfg		UCI config name
-- @param stype		UCI section type
-- @param new		New item
-- @param keys		Use to match items (optional)
-- @return			New item if succeeded or false on error
function Form:insert(cfg, stype, new, keys)
    local uci_r = self.uci_r
    assert(cfg and stype and new)

    local new = type(new) == "table" and new or json.decode(new)
    if not new then
        return false
    end

    if self:_find_item(cfg, stype, new, keys) then
        return false
    end

    local sname = uci_r:section(cfg, stype, nil, new)
    if sname then
        uci_r:reorder(cfg, sname, 0)
        return new
    end

    return false
end


--- Update form items.
-- @param cfg		UCI config name
-- @param stype		UCI section type
-- @param old		Old item
-- @param new		New item
-- @param keys		Use to match items (optional)
-- @return			New item if succeeded or false on error
function Form:update(cfg, stype, old, new, keys)
    local uci_r = self.uci_r
    assert(cfg and stype and old and new)

    local old = type(old) == "table" and old or json.decode(old)
    local new = type(new) == "table" and new or json.decode(new)
    if not new then
        return false
    end

    local sname = self:_find_item(cfg, stype, old, keys)
    if not sname then
        return false
    end

    local sname_new = self:_find_item(cfg, stype, new, keys)
    if sname_new and sname_new ~= sname then
        return false
    end

    if uci_r:section(cfg, stype, sname, new) then
        return new
    end

    return false
end

--- Delete form items.
-- @param cfg		UCI config name
-- @param stype		UCI section type
-- @param key		Key of items to be deleted
-- @param index		Index of items to be deleted
-- @return			Table indicates if succeeded or not
function Form:delete(cfg, stype, key, index)
    local uci_r = self.uci_r
    assert(cfg and stype and key and index)

    local keys  = type(key) == "table" and key or {key}
    local indexes = type(index) == "table" and index or {index}

    local items = {}
    uci_r:foreach(cfg, stype,
        function(section)
            items[#items + 1] = section[".name"]
        end
    )

    local data = {}
    for i, v in ipairs(indexes) do
        data[#data + 1] = {
            key     = keys[i],
            index   = v,
            success = uci_r:delete(cfg, items[v + 1]) and true or false
        }
    end

    return data
end

--- Count of form items.
-- @param cfg		UCI config name
-- @param stype		UCI section type
-- @return			Number
function Form:count(cfg, stype)
    local uci_r = self.uci_r
    assert(cfg and stype)

    local count = 0
    uci_r:foreach(cfg, stype,
                  function()
                      count = count + 1
                  end
    )
    return count
end

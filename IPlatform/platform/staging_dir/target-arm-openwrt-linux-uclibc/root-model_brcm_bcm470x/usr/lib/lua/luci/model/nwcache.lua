--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  nwcache.lua
Details :  Cache for luci.model.network
Author  :  Ye Qianchuan <yeqianchuan@tp-link.net>
Version :  1.0.0
Date    :  08 Mar, 2014
]]--

module("luci.model.nwcache", package.seeall)

-- TODO: cache other stuff.
local _cache

function init()
    if not _cache then
        _cache = require("luci.model.network").init()
    end

    return _cache
end

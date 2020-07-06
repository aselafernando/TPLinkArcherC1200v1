--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  log.lua
Details :  System log library.
Author  :  Ye Qianchuan <yeqianchuan@tp-link.net>
Version :  1.0.0
Date    :  7Jul, 2014
]]--

module("luci.model.log", package.seeall)

local log = require "log"
local util = require "luci.util"

Log = util.class()

function Log:__init__(proj_id)
    assert(proj_id)
    self.proj_id = proj_id
    getmetatable(self).__call = self.log
end

function Log:log(msg_id, ...)
    log.log(self.proj_id, msg_id, ...)
end

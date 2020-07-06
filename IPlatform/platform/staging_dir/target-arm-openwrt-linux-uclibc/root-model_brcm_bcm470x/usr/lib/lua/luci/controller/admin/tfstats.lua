--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  tfstats.lua
Details :  controller for traffic statistics webpage
Author  :  Wen Kun <wenkun@tp-link.net>
Version :  1.0.0
Date    :  17Mar, 2014
]]--
module("luci.controller.admin.tfstats", package.seeall)

local uci               = require "luci.model.uci"
local fs                = require "luci.fs"
local dbg               = require "luci.tools.debug"
local tfs               = require "luci.model.tfstats"
local ctl               = require "luci.model.controller"

function tf_load_status()
    local tfs_t = tfs.TFS_INST()
    return tfs_t:load_enable()
end

function tf_save_status(http_form)
    local tfs_t = tfs.TFS_INST()
    local enable = http_form.enable
    return tfs_t:set_enable(enable)
end

function tf_load_stats()
    local tfs_t = tfs.TFS_INST()
    return tfs_t:load_all_stats()
end

function tf_rst_stats(http_form)
    local tfs_t = tfs.TFS_INST()
    local index = http_form.index
    return tfs_t:reset_one_stats(index)
end

function tf_rst_all_stats()
    local tfs_t = tfs.TFS_INST()
    return tfs_t:reset_all_stats()
end

function tf_del_stats(http_form)
    local tfs_t  = tfs.TFS_INST()
    local keys   = http_form.key
    local indexs = http_form.index
    local data = {
        key   = keys,
        index = indexs
    }
    return tfs_t:delete_stats(data)
end
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------

local dispatch_tbl = {
    status = {
        ["read"]  = { cb  = tf_load_status },
        ["write"] = { cb  = tf_save_status }        
    },

    lists = {
        ["load"]      = { cb  = tf_load_stats },
        ["reset"]     = { cb  = tf_rst_stats },
        ["remove"]    = { cb  = tf_del_stats }, 
        ["reset_all"] = { cb  = tf_rst_all_stats }
    }
}

function dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end

function _index()
    return ctl._index(dispatch)
end

function index()
    entry({"admin", "traffic"}, call("_index")).leaf = true
end
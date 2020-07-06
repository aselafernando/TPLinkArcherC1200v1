--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  system.lua
Details :  Controller for various system-wide functions
Author  :  Ye Qianchuan <yeqianchuan@tp-link.net>
Version :  1.0.0
Date    :  19 Jun, 2014
]]--

module("luci.controller.admin.system", package.seeall)

local ctl = require "luci.model.controller"
local uci = require "luci.model.uci"

function logout()
    local sess = require("luci.dispatcher").context.authsession
    if sess then
        local sauth = require "luci.sauth"
        sauth.kill(sess)
    end
    return true
end

function reboot()
    local sys = require "luci.sys"
    local data = { reboot_time = 75 }
    
    --get reboot time from profile
    data.reboot_time = uci.cursor():get_profile("global", "reboot_time") or 75

    --add for cloud client for closing ss
    sys.fork_call("/etc/init.d/cloud_client stop")
    os.execute("/sbin/upgrade_cleanup")
    sys.fork_exec("sleep 2; reboot")
    return data
end

-- General controller routines

local dispatch_tbl = {
    logout = {
        [".super"] = {cb = logout}
    },
    reboot = {
        [".super"] = {cb = reboot}
    }
}

function dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end

function _index()
    return ctl._index(dispatch)
end

function index()
    entry({"admin", "system"}, call("_index")).leaf = true
end

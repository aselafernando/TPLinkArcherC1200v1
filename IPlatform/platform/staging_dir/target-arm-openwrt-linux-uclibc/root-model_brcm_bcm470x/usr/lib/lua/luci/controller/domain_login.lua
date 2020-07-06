--[[
Copyright(c) 2013 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  domain_login.lua
Details :  controller for domain login tips webpage
Author  :  Guo Dongxian <guodongxian@tp-link.net>
Version :  1.0.1
Date    :  14May, 2014
]]--

module("luci.controller.domain_login", package.seeall)

local uci  = require "luci.model.uci"
local http = require "luci.http"
local sys  = require "luci.sys"
local ctl  = require "luci.model.controller"

DLOGIN_SHELL = "/usr/sbin/domain_login "

function dbg(str)
    sys.call("echo %s >/dev/console 2>&1" % ("[debug]:" .. str))
end

local function dlogin_exec(cmd, which)
    local result = sys.exec(DLOGIN_SHELL .. cmd .. which)
    return result
end

-- Login succeeded, then cancel conflict ip tips
function tips_cancel()
    local uci_c = uci.cursor()
    local conflict  = uci_c:get("domain_login", "tp_domain", "conflict")
    
    if conflict == "on" then
        dlogin_exec("destroy", "")
    end
end

local function tips_get()
    local uci_c = uci.cursor()
    local conflict  = uci_c:get("domain_login", "tp_domain", "conflict")
    local domain    = uci_c:get("domain_login", "tp_domain", "domain")
    local mngr_addr = uci_c:get("domain_login", "tp_domain", "new_addr") 

    local mode      = tostring(dlogin_exec("get ", "mode")):gsub("\n", "")
    local dst_addr  = tostring(dlogin_exec("get ", "dst")):gsub("\n", "")
    local dst_wpath = tostring(dlogin_exec("get ", "wpath")):gsub("\n", "")

    local redirect  = tostring(dlogin_exec("redirect ", "")):gsub("\n", "")
    if redirect and redirect == "1" then
        redirect = "http://" .. domain
        -- Flush the redirect flag, when using old domain, flag be 1
        sys.call("echo f >/proc/domain_login_dns")
    else
        redirect = ""
    end
    
    conflict = (conflict == "on") and true or false
    if conflict and tonumber(mode) == 1 and redirect == "" then
        -- IP conflict tip appear only one time when wan and lan be conflict
        -- Then cancel tip
        dlogin_exec("destroy", "")
    end

    return {
               ["mode"] = tonumber(mode), ["conflict"] = conflict, ["domain"] = domain,
               ["dst_addr"] = dst_addr, ["dst_webpath"] = dst_wpath, 
               ["new_addr"] = mngr_addr, ["redirect"] = redirect
           } 
end

local dispatch_tbl = {
    ["dlogin"] = {
        ["read"] = { cb = tips_get }    
    }
}

function dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end

function _index()
    return ctl._index(dispatch)
end

function index()
    entry({"domain_login"}, call("_index")).leaf = true
end


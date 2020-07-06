--[[
Copyright(c) 2013 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  ddns.lua
Details :  controller for dynamic dns webpage
Author  :  Guo Dongxian <guodongxian@tp-link.net>
Version :  1.0.1
Date    :  09April, 2014
]]--

module("luci.controller.admin.ddns", package.seeall)

local io    = require "io"
local uci   = require "luci.model.uci"
local http  = require "luci.http"
local ddns  = require "luci.model.ddns"
local check = require "luci.tools.datatypes"
local ctl   = require "luci.model.controller"

local Ddns = ddns.DDNS_INST()

local function provider_get()
    local uci_c = uci.cursor()
    local provider = { "noip", "dyndns" }
    -- Default provider is no-ip
    local data = { provider = "noip" }    
    for _, p in ipairs(provider) do
        local enable = uci_c:get("ddns", p, "enabled")
        if enable == "on" then
            data.provider = p     
            break   
        end
    end

    return data
end

--- Normal url parsing
local function comm_parse(formvalue, service)
    local username = formvalue["username"] or ""
    local password = formvalue["password"] or ""
    local domain = formvalue["domain"] or ""
    local update_time = formvalue["update_time"] or "never"

    if #username == 0 or #password == 0 
        or not check.hostname(domain) 
    then
        ddns.err("URL parsing failed.")
        return false    
    end

    local check_interval, check_unit
    if update_time == "never" then
        check_interval = "never"
        check_unit = "seconds"
    else
        local chk_int, chk_unit = update_time:match("(%d+)(%l)$")
        if not chk_int then
            ddns.err("Update interval invalid.")
            return false        
        end
    
        if not chk_unit or (chk_unit ~= "h" and chk_unit ~= "d") 
        then
            ddns.err("Update unit invalid.")
            return false
        end

        local units = { h = "hours", d = "days" }
        check_interval = chk_int
        check_unit = units[chk_unit]
    end
    
    if service == "noip" then
        local wan_bind = formvalue["wan_bind"] or "disable"
        
        return {["username"] = username, ["password"] = password, ["domain"] = domain,
            ["check_interval"] = check_interval, ["check_unit"] = check_unit, ["wan_bind"] = wan_bind} 
            
    end
    
    return {["username"] = username, ["password"] = password, ["domain"] = domain,
            ["check_interval"] = check_interval, ["check_unit"] = check_unit} 
end

--- Parsing dyndns ddns URL
local function dyndns_parse(formvalue)
    return comm_parse(formvalue, "dyndns")
end

--- Parsing noip ddns URL
local function noip_parse(formvalue)
    return comm_parse(formvalue, "noip")
end

--- Refresh state of ddns
local function dyndns_refresh()
    local state = Ddns:status_refresh("dyndns") 
    return {["status"] = state}
end

local function dyndns_get()
    return Ddns:get("dyndns")
end

local function dyndns_set(formvalue)
    local info = dyndns_parse(formvalue)
    if not info then
        return false, "Invaild URL"
    end
    -- When save config, logout first
    Ddns:logout("dyndns")
    return Ddns:set("dyndns", info)
end

local function dyndns_login(formvalue)
    local info = dyndns_parse(formvalue)
    if not info then
        return false, "Invaild URL"
    end

    local dup = Ddns:dup_check("dyndns", info)
    
    if not dup then
        local save = Ddns:set("dyndns", info)
        -- Save succeed, then do logining
        if save then
            Ddns:login("dyndns")
        end
    end

    return Ddns:get("dyndns")
end

local function dyndns_logout()
    Ddns:logout("dyndns")
    -- Logout, get all information from kernel
    return Ddns:get("dyndns")
end

--- Refresh noip ddns state
local function noip_refresh()
    local state = Ddns:status_refresh("noip")
    return {["status"] = state}
end

local function noip_get()
    return Ddns:get("noip")
end

local function noip_set(formvalue)
    local info = noip_parse(formvalue)
    if not info then
        return false, "Invaild URL"
    end
    Ddns:logout("noip")
    return Ddns:set("noip", info)
end

local function noip_login(formvalue)
    local info = noip_parse(formvalue)
    if not info then
        return false, "Invaild URL"
    end

    local dup = Ddns:dup_check("noip", info)
    
    if not dup then    
        local save = Ddns:set("noip", info)
        if save then
            Ddns:login("noip")   
        end
    end

    return Ddns:get("noip")
end

local function noip_logout()
    Ddns:logout("noip")
    return Ddns:get("noip")
end

local dispatch_tbl = {
    ["provider"] = {
        ["read"] = { cb = provider_get }
    },

    ["dyndns"] = {
        ["read"]    = { cb = dyndns_get },
        ["write"]   = { cb = dyndns_set },
        ["login"]   = { cb = dyndns_login },
        ["logout"]  = { cb = dyndns_logout },
        ["refresh"] = { cb = dyndns_refresh }
    },
    
    ["noip"] = {
        ["read"]    = { cb = noip_get },
        ["write"]   = { cb = noip_set },
        ["login"]   = { cb = noip_login },
        ["logout"]  = { cb = noip_logout },
        ["refresh"] = { cb = noip_refresh }
    }
}

function dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end

function _index()
    return ctl._index(dispatch)
end

function index()
    entry({"admin", "ddns"}, call("_index")).leaf = true
end


--[[
Copyright(c) 2011-2015 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  ledgeneral.lua
Details :  Controller for LED General ON/OFF button
Author  :  Liu Qu <liuqu@tp-link.net>
Version :  1.0.0
Date    :  27 Feb, 2015
]]--

module("luci.controller.admin.ledgeneral", package.seeall)

local uci    = require "luci.model.uci"
local dtypes = require "luci.tools.datatypes"
local dbg    = require "luci.tools.debug"
local sys    = require "luci.sys"
local ctl    = require "luci.model.controller"

local RELOAD_LEDGENERAL = "/etc/init.d/ledgeneral reload"

local function get()
    local uci_r = uci.cursor()
    local time_set = sys.exec("[ -f /tmp/ledpm_enable ] && cat /tmp/ledpm_enable")
    
    return { ["enable"] = uci_r:get("ledctrl", "GENERAL", "enable") or "off",
             ["ledpm_support"] = uci_r:get_profile("ledctrl", "ledpm_support") or "no",
             ["time_set"] = time_set:find("1") and "yes" or "no"
             --["night_mode"] = uci_r:get("ledpm", "leds", "enable") or "off"
           }
end

local function set(formvalue)
    local uci_r = uci.cursor()

    local old_data = get()
    local time_set = sys.exec("[ -f /tmp/ledpm_enable ] && cat /tmp/ledpm_enable")
    if time_set:find("1")
    then
        local night_mode_on = uci_r:get("ledpm", "leds", "enable") or "off"
        if night_mode_on == "on"
        then
            local night_enable = uci_r:get("ledctrl", "NIGHT", "enable") or "off"
            if night_enable == "on"
            then
                old_data.enable = "disabled"
                return old_data
            end
        end
    end

    local en = "on"
    if old_data.enable == "on"
    then
        en = "off"
    end

    uci_r:set("ledctrl", "GENERAL", "enable", en)
    uci_r:commit("ledctrl")
    sys.fork_exec(RELOAD_LEDGENERAL)
    
    return get()
end

local dispatch_tbl = {
    ["setting"] = {
        ["read"]  = { cb = get },
        ["write"] = { cb = set }
    }
}

function dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end

function _index()
    return ctl._index(dispatch)
end

function index()
    entry({"admin", "ledgeneral"}, call("_index")).leaf = true
end
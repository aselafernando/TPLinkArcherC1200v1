--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  locale.lua
Details :  Get the locale info.
Author  :  Zhang Zhongwei <zhangzhongwei@tp-link.net>
Version :  1.0.0
Date    :  13 May, 2014
]]--

module("luci.controller.locale", package.seeall)


local nixio = require "nixio"
local uci   = require "luci.model.uci"
local dbg   = require "luci.tools.debug"
local ctl   = require "luci.model.controller"
local log   = require("luci.model.log").Log(283)

local LOCALECFG = "locale"                   -- Config file name.

function log_dbg(msgid, ...)
    log(msgid, ...)
end

function get_sysinfo(formvals)
    local uci_r = uci.cursor()
    local select_permission
    select_permission = uci_r:get_profile("region","select_permission") or "yes"

    local data = {
        locale = uci_r:get(LOCALECFG, "sysinfo", "default") or "en_US",
        force  = uci_r:get(LOCALECFG, "sysinfo", "force") == "true" and true or false,
        model  = uci_r:get(LOCALECFG, "sysinfo", "model") or "AC1900",
        region_select_permission = select_permission
    }
    
    if data.force then
        return data
    end

    local locale = uci_r:get(LOCALECFG, "sysinfo", "locale")
    -- Check whether language has been changed by user.
    if locale then
        -- just do nothing
    else
        -- Get the explorer language.  (e.g. "en-US")
        local web_lang = luci.http.getenv("HTTP_ACCEPT_LANGUAGE") or ""
        local pos      = web_lang:find(",") or 1
        web_lang       = web_lang:sub(1, pos - 1)

        -- Check whether the language package is existed.
        if web_lang and web_lang ~= "" then
            -- Change "en-US" to "en_US".
            locale = web_lang:gsub('-', '_')
            log(2, locale)  -- explorer language is XXX
        else
            -- set default locale
            locale = data.locale
        end
    end

    local file_path  = "/www/webpages/locale/" .. locale .. "/lan.js"

    -- Check the path to determined whether the language is supported.
    if nixio.fs.stat(file_path) then
        data.locale = locale
    end

    -- uci_r:set(LOCALECFG, "sysinfo", "locale", data.locale)
    -- uci_r:commit(LOCALECFG)
    log(51, data.locale)  -- language change to XXX

    return data
end

function set_sysinfo(formvals)
    local uci_r = uci.cursor()

    -- Check whether support multi languages.
    local force = uci_r:get(LOCALECFG, "sysinfo", "force")
    if force and force == "true" then
        log(201)  -- locale change is forbidden
        return false, "locale change is forbidden"
    end

    local locale = formvals.locale or ""
    locale = (locale == "") and "en_US" or locale

    if not locale:match("^[%w_]+$") then
        return false, "invalid args"
    end

    uci_r:set(LOCALECFG, "sysinfo", "locale", locale)
    uci_r:commit(LOCALECFG)

    log(51, locale)  -- locale change to XXX
    -- log(601)  -- save success
    return true
end

--- Dispatch table
local dispatch_tbl = {
    lang = {
        ["read"]  = {cb = get_sysinfo},
        ["write"] = {cb = set_sysinfo}
    }
}

function dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end

function _index()
    return ctl._index(dispatch)
end

--- Module entrance
function index()
    entry({"locale"}, call("_index")).leaf = true
end

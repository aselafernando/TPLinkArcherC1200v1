--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  pptpd.lua
Details :  
Author  :  Zhu Xianfeng<zhuxianfeng@tp-link.net>
Version :  1.0.0
Date    :  02Dec14
History :  arg 1.0.0, 02Dec14, Zhu Xianfeng, Create the file.
]]--

module("luci.controller.admin.pptpd", package.seeall)

local sys      = require "luci.sys"
local muci     = require "luci.model.uci"
local ctl      = require "luci.model.controller"
local vpn      = require "luci.model.vpn"
local form     = require "luci.tools.form"
local debug    = require "luci.tools.debug"
local dtypes   = require "luci.tools.datatypes"
local nixio    = require "nixio"

local uci      = muci.cursor()
form = form.Form(uci)

local PPTPD_MAX_ACCOUNTS = 16
local PPTPD_SHELL = "/etc/init.d/pptpd"
local RESTART = "restart"

-- Verify account
-- @param N/A
-- @return boolean
local function account_valid(str)
    if str == nil then
        return false
    elseif not dtypes.rangelength(str, 1, 15) then
        return false
    elseif not str:find("^[a-zA-Z0-9_-]+$") then
        return false
    end

    return true
end

-- Get PPTP VPN UCI config
-- @param N/A
-- @return table 
local function pptpd_cfg_get()
    local sectype = uci:get("pptpd", "pptpd")

    if sectype ~= "service" then
        return false, "invalid uci config"
    end

    local data = {
        enabled      = uci:get("pptpd", "pptpd", "enabled"),
        remoteip     = uci:get("pptpd", "pptpd", "remoteip"),
        samba_access = uci:get("pptpd", "pptpd", "samba_access"),
        netbios_pass = uci:get("pptpd", "pptpd", "netbios_pass"),
        unencrypted_access = uci:get("pptpd", "pptpd", "unencrypted_access"),

    }

    return data
end

-- Set PPTP VPN UCI config, and restart service
-- @param N/A
-- @return table 
local function pptpd_cfg_set(form)
    local sectype = uci:get("pptpd", "pptpd")

    if sectype ~= "service" then
        return false, "invalid uci config"
    end

    local old = pptpd_cfg_get()

    if form.enabled ~= "on" and form.enabled ~= "off" then
        return false, "invalid parameter enabled"
    end
    if form.samba_access ~= "on" and form.samba_access ~= "off" then
        return false, "invalid parameter enabled"
    end
    if form.netbios_pass ~= "on" and form.netbios_pass ~= "off" then
        return false, "invalid parameter enabled"
    end
    if form.unencrypted_access ~= "on" and form.unencrypted_access ~= "off" then
        return false, "invalid parameter enabled"
    end


    local remoteip = form.remoteip
    local localip  = nil
    local prefix   = nil
    local head     = nil
    local tail     = nil

    prefix, head, tail = remoteip:match("(%d+%.%d+.%d+.)(%d+)-(%d+)")
    if not prefix or not head or not tail then
        return false, "invalid parameter remoteip"
    end

    head = tonumber(head)
    tail = tonumber(tail)

    if (head < 1 or head > 254) or 
       (tail < 1 or tail > 254) or 
       ((tail - head + 1) > 10)
    then
        return false, "invalid parameter remoteip"
    end

    -- localip
    if head ~= 1 then
        localip = string.format("%s%d", prefix, (head -1))
    elseif tail ~= 254 then
        localip = string.format("%s%d", prefix, (tail + 1))
    end

    if old ~= nil and old.enabled == form.enabled
       and old.remoteip == remoteip
           and old.samba_access == form.samba_access
               and old.netbios_pass == form.netbios_pass
                   and old.unencrypted_access == form.unencrypted_access
    then
        return pptpd_cfg_get()
    end

    uci:set("pptpd", "pptpd", "enabled", form.enabled)
    uci:set("pptpd", "pptpd", "localip", localip)
    uci:set("pptpd", "pptpd", "remoteip", remoteip)
    uci:set("pptpd", "pptpd", "samba_access",form.samba_access)
    uci:set("pptpd", "pptpd", "netbios_pass", form.netbios_pass)
    uci:set("pptpd", "pptpd", "unencrypted_access", form.unencrypted_access)
    uci:commit("pptpd")

    -- restart pptpd service
    sys.fork_exec("/etc/init.d/pptpd restart")

    if old ~= nil then
       if old.enabled ~= form.enabled or old.remoteip ~= remoteip then
           sys.fork_exec("/etc/init.d/samba restart")
       end
    end

    --sys.fork_exec("/etc/init.d/pptpd stop")
    --sys.fork_exec("fw pptp_block")
    --nixio.nanosleep(1, 0)
    --if form.enabled == "on" then
    --    sys.fork_exec("/etc/init.d/pptpd start")
    --    sys.fork_exec("fw pptp_access")
    --elseif form.enabled == "off" then
        -- disconnect all pptp clients
    --  vpn.vpn_disconn_all("pptp")
    --end

    return pptpd_cfg_get()
end

function get_max_pptpd_account()
    return { ["max_accounts"] = uci:get_profile("pptpd", "max_accounts") or PPTPD_MAX_ACCOUNTS }
end

function pptpd_account_max_check()
    local max =get_max_pptpd_account()
    local cur = form:count("pptpd", "login")
    
    if cur >= max.max_accounts then
        return false
    end
    
    return true
end

--- Insert a new pptpd account
function insert_pptpd_account(http_form)
    local new_account = http_form["new"]
    local new = luci.json.decode(new_account)

    if not new and type(new) ~= "table" then
        return {errorcode="invalid new params"}
    end

    if not pptpd_account_max_check() then
        return {errorcode="reach max items"}
    end

    -- verify account's username and password
    if not account_valid(new.username) then
        return false, "invalid username"
    elseif not account_valid(new.password) then
        return false, "invalid password"
    end

    local ret = form:insert("pptpd", "login", new, {"username", "password"})
    if ret then
        uci:commit("pptpd")
        return ret
    else
        return false, "insert new items failed"
    end
end

--- Update a pptpd account selected by UI
function update_pptpd_account(http_form)
    local old = http_form["old"]
    local new = http_form["new"]

    local old = luci.json.decode(old)
    local new = luci.json.decode(new)

    if not new and type(new) ~= "table" or
        not old and type(old) ~= "table" then
        return {errorcode="invalid new params"}
    end

    -- verify account's username and password
    if not account_valid(new.username) then
        return false, "invalid username"
    elseif not account_valid(new.password) then
        return false, "invalid password"
    end

    local ret = form:update("pptpd", "login", old, new, {"username", "password"})
    if ret then
        uci:commit("pptpd")
        return ret
    else
        return false, "modify item failed"
    end
end

--- Remove a pptpd account selected by UI
function remove_pptpd_account(http_form)
    local key = http_form["key"]
    local index = http_form["index"]

    local ret = form:delete("pptpd", "login", key, index)

    if ret then
        uci:commit("pptpd")
        return ret
    else
        return false, "remove pptpd account failed"
    end

end

--- Remove all dhcp static lease host
function remove_all_pptpd_account()
    local secs = {}

    uci:foreach("pptpd", "login",
        function(section)
            secs[#secs + 1] = section[".name"]
        end
    )   

    for _, s in ipairs(secs) do
        uci:delete("pptpd", s)
    end

    uci:commit("pptpd")

    return true
end


--- Get pptpd accounts from configuration
function load_pptpd_accounts()
    local pptpd_accounts = {}

    local accounts = {}
    uci:foreach("pptpd", "login",
        function(section)
            accounts[#accounts + 1] = uci:get_all("pptpd", section[".name"])
        end
    )

    for k, rt in ipairs(accounts) do
        local data = {}
        data.username = rt.username
        data.password = rt.password
        pptpd_accounts[k] = data
    end

    return pptpd_accounts
end


-- 1. Get/Set
-- URL: /admin/pptpd?form=config
-- Parameters:
--     enabled: on/off
--     remoteip: 192.168.10.1-10
--     username: admin
--     password: admin
local dispatch_tbl = {
    config = {
        ["read"]  = {cb = pptpd_cfg_get},
        ["write"] = {cb = pptpd_cfg_set},
    },
    accounts = {
        ["load"] = {cb = load_pptpd_accounts, others = get_max_pptpd_account},
        ["insert"] = {cb = insert_pptpd_account, others = get_max_pptpd_account, cmd = RESTART},
        ["update"] = {cb = update_pptpd_account, others = get_max_pptpd_account, cmd = RESTART},
        ["remove"] = {cb = remove_pptpd_account, others = get_max_pptpd_account, cmd = RESTART},
        ["clear"]  = {cb = remove_all_pptpd_account, cmd = RESTART}
    }
}


local function pptpd_dispatch(http_form)
    local function hook(success, action)
        if success and action.cmd and action.cmd ~= "" then
            sys.fork_exec("%s %s" % {PPTPD_SHELL, action.cmd})
        end
        return true
    end
    return ctl.dispatch(dispatch_tbl, http_form, {post_hook = hook})
end

function pptpd_index()
    return ctl._index(pptpd_dispatch)
end

function index()
    entry({"admin", "pptpd"}, call("pptpd_index")).leaf = true
end

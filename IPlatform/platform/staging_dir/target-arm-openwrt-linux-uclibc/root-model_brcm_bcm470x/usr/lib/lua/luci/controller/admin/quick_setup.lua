--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  quick_setup.lua
Details :  Controller for quick setup webpage
Author  :  Ye Qianchuan <yeqianchuan@tp-link.net>
Version :  1.0.0
Date    :  11 Apr, 2014
]]--

module("luci.controller.admin.quick_setup", package.seeall)

local sys = require "luci.sys"
local util = require "luci.util"
local ctl = require "luci.model.controller"
local dbg = require "luci.tools.debug"

function updateonly_and_prefix(t, updates, prefix, keys)
    prefix = prefix or ""
    if keys then
        for _, k in ipairs(keys) do
            t[prefix .. k] = updates[k]
        end
    else
        for k, v in pairs(updates) do
            t[prefix .. k] = v
        end
    end
end

function get_wire_type()
    local addr = sys.getenv("REMOTE_ADDR")
    local clist = require("luci.model.client_mgmt").get_client_list()
    for _, client in ipairs(clist) do
        if client.ip == addr then
            return client.wire_type == "wired" and "wired" or "wireless"
        end
    end
    return "wired"
end

local controller_prefix = "luci.controller.admin"
local ext_dispatch_tbl = {
    time = {
        controller = controller_prefix .. ".timesetting",
        target = "dispatch",
        forms = {
            {form = "settings", prefix = "time_"}
        }
    },
    network = {
        controller = controller_prefix .. ".network",
        target = "dispatch",
        limit_key = "network_conntype",
        forms = {
            {form = "wan_ipv4_status", prefix = "network_", limit = "status"},
            {form = "wan_ipv4_dynamic", prefix = "network_dhcp_", limit = "dhcp"},
            {form = "wan_ipv4_staticip", prefix = "network_static_", limit = "static"},
            {form = "wan_ipv4_pppoe", prefix = "network_pppoe_", limit = "pppoe"},
            {form = "wan_ipv4_l2tp", prefix = "network_l2tp_", limit = "l2tp"},
            {form = "wan_ipv4_pptp", prefix = "network_pptp_", limit = "pptp"}
        }
    },
    wireless = {
        controller = controller_prefix .. ".wireless",
        target = "wireless_dispatch",
        forms = {
            {form = "wireless_2g", prefix = "wireless_2g_"},
            {form = "wireless_5g", prefix = "wireless_5g_",
             limit_key = "wireless_5g_region_enable", limit = "on"},
            {form = "region", prefix = "region_"}
        }
    }
}

function read(http_form)
    local data = {}
    -- Wire type
    data.wire_type = get_wire_type()

    local success = true

    for _, dsp in pairs(ext_dispatch_tbl) do
        local target = require(dsp.controller)[dsp.target]
        target = (type(target) == "function") and target
        if target then
            for _, v in pairs(dsp.forms) do
                local form = {
                    form = v.form,
                    operation = "read"
                }
                local ret = target(form)
                if ret.success then
                    updateonly_and_prefix(data, ret.data, v.prefix)
                else
                    success = false
                end
            end
        end
    end

    return success and data
end

function write(http_form)
    local data = {}
    local success = true

    for _, dsp in pairs(ext_dispatch_tbl) do
        target = require(dsp.controller)[dsp.target]
        target = (type(target) == "function") and target
        if target then
            for _, v in pairs(dsp.forms) do
                local limit_key = v.limit_key or dsp.limit_key
                if not limit_key or v.limit == http_form[limit_key] then
                    local form = {
                        form = v.form,
                        operation = "write"
                    }
                    for oldk, value in pairs(http_form) do
                        local k = oldk:match("^%s(.*)$" % v.prefix)
                        if k then
                            form[k] = value
                        end
                    end

                    local ret = target(form)
                    if ret.success then
                        updateonly_and_prefix(data, ret.data, v.prefix)
                    else
                        success = false
                    end
                end
            end
        end
    end

    return success and data
end

function check_internet(http_form)
    local success = sys.call("online-test") == 0
    return success
end

function check_router(http_form)
    local internet = require("luci.model.internet").Internet()
    return internet:status() == "connected"
end

--[[
function save_session()
    local sauth = require "luci.sauth"
    local sess = require "luci.dispatcher".context.authsession
    local sdat = sauth.read(sess)

    if sdat then
        -- Save session
        local uci_r = require "luci.model.uci".cursor()
        uci_r:section("session", "session", "session", {
                          sid = sess,
                          addr = sdat.addr,
                          user = sdat.user,
                          token = sdat.token,
                          secret = sdat.secret})
        uci_r:commit("session")
    end

    return true
end

function load_session()
    local uci_r = require "luci.model.uci".cursor()
    local sdat = uci_r:get_all("session", "session")

    if sdat then
        local sauth = require "luci.sauth"
        sauth.write(sdat.sid, {
                        addr = sdat.addr,
                        user = sdat.user,
                        token = sdat.token,
                        secret = sdat.secret})
    end

    return true
end

--]]

-- General controller routines

local dispatch_tbl = {
    quick_setup = {
        ["read"] = {cb = read},
        ["write"] = {cb = write}
    },
    check_internet = {
        [".super"] = {cb = check_internet}
    },
    check_router = {
        [".super"] = {cb = check_router}
    }
}

function dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end

function _index()
    return ctl._index(dispatch)
end

function index()
    entry({"admin", "quick_setup"}, call("_index")).leaf = true
end

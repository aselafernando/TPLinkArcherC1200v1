--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  upnp.lua
Details :  Controller for upnp webpage
Author  :  Ye Qianchuan <yeqianchuan@tp-link.net>
Author  :  Wen Kun <wenkun@tp-link.net>
Version :  1.0.0
Date    :  25 Feb, 2014
]]--

module("luci.controller.admin.upnp", package.seeall)

local uci = require "luci.model.uci"
local ctl = require "luci.model.controller"

local uci_r = uci.cursor()

local UPNP_SHELL      = "/etc/init.d/miniupnpd"
local UPNP_SH_ENABLE  = UPNP_SHELL .. " restart"
local UPNP_SH_DISABLE = UPNP_SHELL .. " stop"

function read_enable()
    local enable = uci_r:get("upnpd", "config", "enable_upnp")
    return {
        enable = enable == "1" and "on" or "off"
    }
end

function write_enable(http_form)
    local sys    = require "luci.sys"
    local ctypes = require "luci.model.checktypes"
    local enable = http_form["enable"]
    if not ctypes.check_onoff(enable) then
        return false, "invalid args"
    end

    if read_enable().enable ~= enable then
        uci_r:set("upnpd", "config", "enable_upnp", enable == "on" and "1" or "0")
        uci_r:commit("upnpd")
        sys.fork_exec(enable == "on" and UPNP_SH_ENABLE or UPNP_SH_DISABLE)
    end
    return {
        enable = enable
    }
end

function get_clients()
    local fs = require "nixio.fs"
    local leasefile = uci_r:get("upnpd", "config", "upnp_lease_file")
    local data = {}

    local file = io.open(leasefile)
    if not file then
        return data
    end

    for line in file:lines() do
        local proto, extport, intaddr, intport, ts, desc
            = line:match("^([^:]+):([^:]+):([^:]+):([^:]+):([^:]+):(.*)$")
        if proto and extport and intaddr and intport then
            data[#data + 1] = {
                leasetime     = os.date("%X",
                                        os.difftime(tonumber(ts) or 0,
                                                    os.time())),
                name          = desc or "",
                external_port = extport,
                internal_port = intport,
                ipaddr        = intaddr,
                protocol      = proto:upper()
            }
        end
    end
    file:close()

    return data
end

-- General controller routines

local dispatch_tbl = {
    enable = {
        ["read"] = {cb = read_enable},
        ["write"] = {cb = write_enable}
    },
    service = {
        [".super"] = {cb = get_clients}
    }
}

function dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end

function _index()
    return ctl._index(dispatch)
end

function index()
    entry({"admin", "upnp"}, call("_index")).leaf = true
end

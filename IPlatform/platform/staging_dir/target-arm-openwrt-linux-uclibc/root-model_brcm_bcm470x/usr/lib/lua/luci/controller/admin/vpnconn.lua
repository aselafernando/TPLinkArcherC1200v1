--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  vpnconn.lua
Details :  
Author  :  Zhu Xianfeng<zhuxianfeng@tp-link.net>
Version :  1.0.0
Date    :  02Dec14
History :  arg 1.0.0, 02Dec14, Zhu Xianfeng, Create the file.
]]--

module("luci.controller.admin.vpnconn", package.seeall)

local sys      = require "luci.sys"
local ctl      = require "luci.model.controller"
local vpn      = require "luci.model.vpn"
local debug    = require "luci.tools.debug"

-- Form list handler
-- @param N/A
-- @return table
local function vpnconn_list(form)
    local clients = nil

    if form.vpntype == "openvpn" then
        clients = vpn.vpn_clients("openvpn")
    elseif form.vpntype == "pptp" then
        clients = vpn.vpn_clients("pptp")
    else 
        return false, "invalid parameter vpntype"
    end

    -- add key for client table
    for _, v in pairs(clients)do
        v.key = v.extra
    end

    return clients
end

-- Form disconnect handler
-- @param N/A
-- @return boolean
local function vpnconn_disconn(form)
    if form.vpntype ~= "openvpn" and form.vpntype ~= "pptp" then
        return false, "invalid parameter vpntype"
    end

    local ret = false
    local data = {}
    local client = {
        vpntype = form.vpntype,
        extra   = form.key,
    }

    ret = vpn.vpn_disconn(client)
    if ret == false then
        return false, "Can't find current VPN client connection."
    end

    client = {
        vpntype = form.vpntype,
        index   = form.index,
        key     = form.key,
        success = true,
    }

    table.insert(data, client)
    return data
end

-- 1. List
-- URL: /admin/vpnconn?form=config
-- Parameters:
--     operation: list
--     vpntype: openvpn/pptp
--     clients: [
--         { vpntype: "openvpn", ipaddr: "10.0.0.11", extra: "extra msg1" },
--         { vpntype: "openvpn", ipaddr: "10.0.0.12", extra: "extra msg2" },
--         { vpntype: "openvpn", ipaddr: "10.0.0.13", extra: "extra msg3" },
--     ]
-- 2. Disconnect
-- URL: /admin/vpnconn?form=config
-- Parameters: 
--     operation: disconnect
--     vpntype: openvpn/pptp
--     ipaddr: 10.0.0.11
--     extra: extra msg1
local dispatch_tbl = {
    config = {
        ["list"]       = {cb = vpnconn_list},
        ["disconnect"] = {cb = vpnconn_disconn},
    },
}

local function vpnconn_dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end

function vpnconn_index()
    return ctl._index(vpnconn_dispatch)
end

function index()
    entry({"admin", "vpnconn"}, call("vpnconn_index")).leaf = true
end

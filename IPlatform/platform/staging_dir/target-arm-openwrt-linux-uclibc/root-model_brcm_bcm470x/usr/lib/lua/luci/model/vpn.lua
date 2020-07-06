--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  vpn.lua
Details :  
Author  :  Zhu Xianfeng<zhuxianfeng@tp-link.net>
Version :  1.0.0
Date    :  07Jan15
History :  arg 1.0.0, 07Jan15, Zhu Xianfeng, Create the file.
]]--

module("luci.model.vpn", package.seeall)

require "luci.ip"
local sys      = require "luci.sys"
local debug    = require "luci.tools.debug"

-- Get all OpenVPN clients connected
-- @param N/A
-- @return table
local function openvpn_clients()
    local clients = {}
    local cmd     = 'vpn_mgmt openvpn "status 2"'
    local input   = io.popen(cmd)
    local line    = nil
    local remoteip = nil
    local ipaddr  = nil
    local extra   = nil

    for line in input:lines() do
        -- CLIENT_LIST,client,124.0.0.2:63572,10.8.0.6,7701,7893,Sat Dec 13 10:22:27 2014,1418494947,UNDEF
        extra, ipaddr = line:match("^CLIENT_LIST,client,(%d+%.%d+%.%d+%.%d+:%d+),(%d+%.%d+%.%d+%.%d+)")
        if extra then
            remoteip = extra:match("(%d+%.%d+%.%d+%.%d+):%d+")
        end
        if remoteip and ipaddr and extra then
            table.insert(clients, {vpntype = "openvpn", remote_ip = remoteip, ipaddr = ipaddr, extra = extra})
        end
    end

    input:close()

    return clients
end

-- Disconnect OpenVPN client connected
-- @param N/A
-- @return boolean
local function openvpn_disconn(client)
    local clients = openvpn_clients()
    local v = nil

    if type(client) == "table" then
        for _, v in pairs(clients) do
            if v.extra == client.extra then
                sys.fork_exec('vpn_mgmt openvpn "kill ' .. v.extra .. '"')
                nixio.nanosleep(1, 0)
                return true
            end
        end
    elseif type(client) == "string" and client == "all" then
        -- Disconnect all clients
        for _, v in pairs(clients) do
            sys.fork_exec('vpn_mgmt openvpn "kill ' .. v.extra .. '"')
        end
        return true
    end

    return false
end


-- Get all PPTP VPN clients connected
-- @param N/A
-- @return table
local function pptp_clients()
    local clients = {}
    local cmd = [[
        for pid in `pidof pppd`; do 
            cat /proc/$pid/cmdline | xargs -0 echo "pid=$pid "
        done
    ]]
    local input  = io.popen(cmd)
    local line   = nil
    local pid    = nil
    local ipaddr = nil
    local remoteip = nil
    local username = nil
    local filename = nil
    local file = nil
    local input2 = nil

    for line in input:lines() do
		pid, _, _, ipaddr, remoteip = line:match("^pid=(%d+)%s+.+%s+(%d+)%s+(%d+%.%d+%.%d+%.%d+):(%d+%.%d+%.%d+%.%d+)%s+ipparam%s+(%d+%.%d+%.%d+%.%d+)")
        if pid then
            filename = "/tmp/ppp" .. pid
            file = io.open(filename, "r")
            if file then
                username = file:read("*line")
            else
                username = nil
            end
            file:close()
        end
        if pid and ipaddr and remoteip and username then
            table.insert(clients, {vpntype = "pptp", ipaddr = ipaddr, remote_ip = remoteip, username = username, extra = pid})
        end
        ---line = input:read("*l")
    end

    input:close()

    return clients
end

-- Disconnect PPTP VPN client connected
-- @param N/A
-- @return boolean
local function pptp_disconn(client)
    local clients = pptp_clients()
    local v = nil

    if type(client) == "table" then
        for _, v in pairs(clients) do
            if v.extra == client.extra then
                sys.fork_exec("kill " .. client.extra)
                nixio.nanosleep(1, 0)
                return true
            end
        end
    elseif type(client) == "string" and client == "all" then
        -- Disconnect all clients
        for _, v in pairs(clients) do
            sys.fork_exec("kill " .. v.extra)
        end
        return true
    end

    return false
end

-- Get all VPN clients connected 
-- @param N/A
-- @return table
function vpn_clients(vpntype)
    if vpntype == "openvpn" then
        return openvpn_clients()
    elseif vpntype == "pptp" then
        return pptp_clients()
    else 
        return false
    end
end

function check_ip_is_vpn_client(ip)
    local clients = pptp_clients()
    for _, item in pairs(clients) do
        if luci.ip.IPv4(ip) == luci.ip.IPv4(item.ipaddr) then
            return true
        end
    end
    clients = openvpn_clients()
    for _, item in pairs(clients) do
        if luci.ip.IPv4(ip) == luci.ip.IPv4(item.ipaddr) then
            return true
        end
    end     
    return false
end

-- Disconnect VPN client connected
-- @param N/A
-- @return boolean
function vpn_disconn(client)
    if client.vpntype == "openvpn" then
        return openvpn_disconn(client)
    elseif client.vpntype == "pptp" then
        return pptp_disconn(client)
    else
        return false
    end
end

-- Disconnect all VPN client connected
-- @param N/A
-- @return boolean
function vpn_disconn_all(vpntype)
    if vpntype == "openvpn" then
        return openvpn_disconn("all")
    elseif vpntype == "pptp" then
        return pptp_disconn("all")
    else
        return false
    end
end

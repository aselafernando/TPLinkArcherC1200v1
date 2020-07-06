#!/usr/bin/env lua

local bit = require "bit"
local nw  = require "luci.controller.admin.network"
local luci_cfg = require "luci.config"
local nixio = require "nixio"

local function lua_split_string(string, split)
    local tab = {}
    
    while true do
        local pos = string.find(string, split)
        if not pos then
            tab[#tab + 1] = string
            break
        end
        local sub_str = string.sub(string, 1, pos - 1)
        tab[#tab + 1] = sub_str
        string = string.sub(string, pos + 1, #string)
    end
    
    return tab
end

local function lua_inet_aton(ipaddr)
    if type(ipaddr) ~= "table" then
        return 0
    end

    local ip = bit.lshift(ipaddr[1], 24) + bit.lshift(ipaddr[2], 16) + bit.lshift(ipaddr[3], 8) + ipaddr[4]
    return ip
end

local function dlogin_is_samesubnet(arg)
    if not arg[2] or not arg[3] or not arg[4]
    then
        return false
    end
    
    local lan_addr = lua_split_string(arg[2], "%.")
    local wan_addr = lua_split_string(arg[3], "%.")
    local mask = bit.lshift(tonumber(0xffffffff, 16), 32 - tonumber(arg[4]))

    if bit.band(lua_inet_aton(lan_addr), mask) ==
            bit.band(lua_inet_aton(wan_addr), mask)
    then
        return true
    else
        return false
    end
end

-- Param must be wan ip address
local function dlogin_network_do(arg)
    local conflict_ip = { 
        [1] = "192.168.1.1", 
        [2] = "192.168.0.1"
    }

    if not arg[2] then
        return false
    end

    local new_addr = "192.168.1.1"
    local new_mask = "255.255.255.0"

    local wan_ip = lua_split_string(arg[2], "%.")
    local lan_ip = lua_split_string(new_addr, "%.")
    local mask = bit.lshift(tonumber(0xffffffff, 16), 8)

    if bit.band(lua_inet_aton(wan_ip), mask) ==
            bit.band(lua_inet_aton(lan_ip), mask)
    then
        new_addr = "192.168.0.1"
    else
        new_addr = "192.168.1.1"
    end
    
    local http_form = {
        form = "lan_ipv4",
        operation = "write",
        ipaddr = new_addr,
        mask_type = new_mask
    }

    local res = nw.dispatch(http_form)
    if res.success then
        local new_net = new_addr .. "/" .. new_mask
        return new_net
    else
        return false
    end
end

local function dlogin_kill_session(arg)
    local sauth = require "luci.sauth"
    local path = luci_cfg.sauth.sessionpath
    local sess_dir = nixio.fs.dir(path)
    local sess

    if sess_dir ~= nil then
        for sess in nixio.fs.dir(path) do
            sauth.kill(sess)
        end
    end

    return true
end

local dlogin_inst = {
    api = {
        check = dlogin_is_samesubnet,
        ipchg = dlogin_network_do,
        ksess = dlogin_kill_session
    }
}

local func = dlogin_inst.api[arg[1]]

if not func then
    print(false)
end

local res = func(arg)

print(tostring(res))


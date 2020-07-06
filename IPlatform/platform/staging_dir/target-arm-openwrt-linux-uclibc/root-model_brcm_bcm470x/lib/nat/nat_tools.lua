#!/usr/bin/env lua
--[[ 
Copyright(c) 2011-2013 Shenzhen TP-LINK Technologies Co.Ltd.

file    : nat_tools.lua
brief   : 
author  : Guo Dongxian
version : 1.0.0
date    : 26Feb14
]]--

local bit = require "bit"

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

local pc_addr  = lua_split_string(arg[1], "%.")
local lan_addr = lua_split_string(arg[2], "%.")
local mask = bit.lshift(tonumber(0xffffffff, 16), 32 - tonumber(arg[3]))

local pc_net  = bit.band(lua_inet_aton(pc_addr),  mask)
local lan_net = bit.band(lua_inet_aton(lan_addr), mask)

if pc_net == lan_net then
    print(true)
else
    print(false)
end


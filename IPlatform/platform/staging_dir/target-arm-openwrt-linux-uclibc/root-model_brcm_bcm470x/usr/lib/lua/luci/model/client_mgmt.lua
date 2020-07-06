--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  client_mgmt.lua
Details :  Ubus client for client management.
Author  :  Ye Qianchuan <yeqianchuan@tp-link.net>
Author  :  Jin Xuexue <jinxuexue@tp-link.net>
Version :  1.0.0
Date    :  07 Mar, 2014
]]--

module ("luci.model.client_mgmt", package.seeall)

local ubus = require "ubus"
local _ubus
local _clist

local UBUS_OBJECT = "client_mgmt"

--- Connect ubus for getting data later.
function init()
	_ubus = ubus.connect()
end

--- Refresh clients table.
-- @return		Boolean whether operation succeeded
function update()
	_ubus = _ubus or ubus.connect()
    return _ubus:call(UBUS_OBJECT, "update", {})
end

--- Get the ARP table.
-- @return		Table of ARP
function get_ARP()
	_ubus = _ubus or ubus.connect()
    return _ubus:call(UBUS_OBJECT, "get_ARP", {})
end

--- Get the list of clients.
-- @return		Table of client entries
function get_client_list()
	_ubus = _ubus or ubus.connect()
    _clist = _clist or _ubus:call(UBUS_OBJECT, "get", {request_type=0})
	return _clist
end

--- Get the list of clients with ip mixed and enter.
-- @return		Table of client entries
function get_client_list_dev()
    local _ubus = _ubus or ubus.connect()
    _clist = _clist or _ubus:call(UBUS_OBJECT, "get", {request_type=0})
	local clist = _clist

    for _, client in ipairs(clist) do
        local mac = client.mac
        if mac then
            for _ , client_cmp in ipairs(clist) do
                if client ~= client_cmp and client_cmp.mac == mac then
                    client_cmp.mac = nil
                    client.ip = client.ip .. "<br />" .. client_cmp.ip
                    if client.hostname ~= client_cmp.hostname and client.hostname == "UNKNOW" then
                        client.hostname = client_cmp.hostname
                    end
                end  
            end
        end
    end
    
    for index, client in ipairs(clist) do 
        if client.mac == nil then
            clist[index] = nil
        end
    end

    return clist
end

--- Get the list of clients subjected to the given filter.
-- @param field			Key to be filtered by
-- @param cmp			Value to be compared with
-- @param need_update	Update first if true (optional)
-- @return				Table of client entries filtered
function get_client_by(field, cmp, need_update)
    if need_update then
        update()
    end

    local clist = get_client_list()
    if not clist then
        return
    end

    for _, client in ipairs(clist) do
        if client[field] == cmp then
            return client
        end
    end
end

--- Get Mac address by given IP address.
-- @param ipaddr		IP address to be compared with
function get_mac_by_ip(ipaddr)
    local arp = get_ARP()
    for _, item in pairs(arp) do
        if item.ip == ipaddr then
            return item.mac
        end
    end
end

--[[
-- The format of one client entry is as follow:
    {
	    hostname	= "name"
    	ip 			= "192.168.1.241"
	    mac 		= "00-11-22-33-44-55"
	    wire_type	= "wired/2.4G/5G/UNKNOW"
	    guest 		= "GUEST/NON_GUEST/UNKNOW"
	    active		= 0/1
    }
]]--

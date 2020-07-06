--[[Copyright(c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  wan_error.lua
Details :  wan_error http response operation
Author  :  chenqingpeng  <chenqingpeng@tp-link.net>
Version :  1.0.0
Date    :  25Feb, 2015
]]--


module("luci.controller.wan_error", package.seeall)

local uci       = require "luci.model.uci"
local http      = require "luci.http"
local dbg     = require "luci.tools.debug"
local parttbl   = require "luci.tools.parttbl"
local sys       = require "luci.sys"
local nixio     = require "nixio"
local ctl       = require "luci.model.controller"
local form      = require "luci.tools.form"
local ubus      = require "ubus"

local uci_r = uci.cursor()
form = form.Form(uci_r, {"mac"})

function _index()
    return ctl._index(dispatch)
end

function index()
    entry({"wan_error"}, call("_index")).leaf = true
end

function read_wan_error()
	local status = require("luci.controller.admin.status")
	local network = require("luci.controller.admin.network")
	--local netstat = status.get_internet_status()
	local ret = {}
	local connect_type
	
	if not _ubus then
		_ubus = ubus.connect()
	end
	local link
	if _ubus then
		local wan = _ubus:call("network.interface.wan", "status", {})
		if wan then
			local ifname = wan.device
			wan = _ubus:call("network.device", "status", {name = ifname})
			if wan then
				link = wan.link
			end
		end
	end
	
	if not link then
		ret.errnum = "-50101"  --unplugged
		return ret
	end
	
	dofile("/tmp/wportal/online.lua")
	
	if onlineTestError == "poor_connected" then
		ret.errnum = "-50102"
		return ret
	end
	
	if onlineTestError == "disconnected" or onlineTestError == "connecting" then
		connect_type = network.get_ipv4_conntype()
		if connect_type == "dhcp" then
			ret.errnum = "-50103"
			return ret
		end
		if connect_type == "pppoe" then
			ret.errnum = "-5011"
		end
		if connect_type == "pptp" then
			ret.errnum = "-5012"
		end
		if connect_type == "l2tp" then
			ret.errnum = "-5013"
		end
		
		local upgrade
		local fp = io.open("/tmp/connecterror",'r')
		if fp == nil then
			ret.errnum = "-50140"
		else
			upgrade = fp:read("*a")
			fp:close()
			
			if upgrade:match("[.]*auth_failed[.]*") then
				ret.errnum = ret.errnum .. "1"
			else
				ret.errnum = ret.errnum .. "2"
			end
		end
		return ret
	end
	
	ret.errnum = "-50140"
	return ret
end

local function never_remind()
--[[
	local parent_ctrl = require "luci.controller.admin.parental_control"
	local mac  = parent_ctrl.get_user_mac():upper()
	local ret = {}
	
	ret.mac = mac
	
    local count = form:count("wportal", "device")
    local max_count = uci_r:get_profile("wportal", "max_dev")
	if max_count == nil then
		debug.print("no wportal profile")
		max_count = 64
	end
	if count < max_count then
		form:insert("wportal", "wan_error_device", ret)
	end

    uci_r:commit("wportal")
	sys.fork_call("wportalctrl -a "..mac)
	return true
	]]--
	--增加一个标志位标记不再提示
	
	uci_r:set("wportal", "wanerror", "enable", "no")
	uci_r:commit("wportal")
	sys.fork_exec("wportalctrl -c")
	sys.fork_exec("echo \"stop\" > /tmp/wportal/status")
	sys.fork_call("/etc/hotplug.d/iface/99-wportal")
	return true
	
end

local dispatch_tbl = {
	read = {
        ["load"]  = { cb  = read_wan_error }  
    },
	never = {
		["never"] = { cb = never_remind }
	}
}

function dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end
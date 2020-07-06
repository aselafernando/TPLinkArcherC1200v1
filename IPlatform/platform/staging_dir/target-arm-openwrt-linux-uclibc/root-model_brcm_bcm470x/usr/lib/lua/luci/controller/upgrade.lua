--[[Copyright(c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  upgrade.lua
Details :  upgrade http response operation
Author  :  chenqingpeng  <chenqingpeng@tp-link.net>
Version :  1.0.0
Date    :  25Feb, 2015
]]--

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

module("luci.controller.upgrade", package.seeall)


function ignore_mac(http_form)
	local ret = {}
	local parent_ctrl = require "luci.controller.admin.parental_control"
	local mac  = parent_ctrl.get_user_mac():upper()

    ret.mac  = mac:upper()

    local count = form:count("wportal", "device")
    local max_count = uci_r:get_profile("wportal", "max_dev")
	if count < max_count then
		form:insert("wportal", "device", ret)
	end

    uci_r:commit("wportal")
end

function ignore_forever(http_form)
	local uci_r = uci.cursor()
	uci_r:set("wportal", "upgrade", "enable", "no")
	uci_r:commit("wportal")
	sys.fork_call("/etc/hotplug.d/iface/99-wportal")
	return true
--[[
	local parent_ctrl = require "luci.controller.admin.parental_control"
	if parent_ctrl == nil then
		return false
	end
	local mac  = parent_ctrl.get_user_mac():upper()
	sys.fork_call("wportalctrl -a "..mac)
	ignore_mac(http_form)
	return true]]--
end

function ignore_tmp(http_form)
--[[
	local parent_ctrl = require "luci.controller.admin.parental_control"
	local mac  = parent_ctrl.get_user_mac():upper()
	sys.fork_call("wportalctrl -t -a "..mac)
	sys.fork_call("echo \"wportalctrl -t -a "..mac.." \" >> /tmp/wportal/wportaltmp.sh")
	return true]]--
	--增加一个标志位标志当前提示时间。
	local uci_r = uci.cursor()
	local seconds = os.time()
	
	uci_r:set("wportal", "upgrade", "time", seconds)
	uci_r:commit("wportal")
--	sys.fork_call("echo '" .. seconds .. "' > /tmp/wportal/upgrade_ignore_tmp")
	sys.fork_call("/etc/hotplug.d/iface/99-wportal")
	return true
end

function set_upgrade_flag(http_form)
	local nav = http_form.nav
	local page = http_form.page
	sys.fork_call("echo true > /tmp/isupgrade")
	
	local uci_r = uci.cursor()
	local seconds = 0
	
	uci_r:set("wportal", "upgrade", "time", seconds)
	uci_r:commit("wportal")

	sys.fork_call("/etc/hotplug.d/iface/99-wportal")
	return true
end






function _index()
    return ctl._index(dispatch)
end

function index()
    entry({"upgrade"}, call("_index")).leaf = true
end

function read_is_upgrade()
    local ret = {}
	local configtool = require "luci.sys.config"
	ret.upgrade = configtool.isupgrade()
	if ret.upgrade == true then
		ret.nav="advanced"
		ret.page="firmware-upgrade"
		configtool.clear_upgrade_flag()
		return ret
	else
		return false
    end
end

function read_upgrade_information()
	local data = {}
    local uci_r = uci.cursor()
	local types
	data.firmware_version = uci_r:get("cloud_config", "upgrade_info", "version") or ""
	data.detail = uci_r:get("cloud_config", "upgrade_info", "release_log") or ""
        data.content = uci_r:get("cloud_config", "upgrade_info", "content") or ""
	types = uci_r:get("cloud_config", "upgrade_info", "type") or "1"
	data.level = types
	if types == "2" then
		data.level = "0"
		ignore_tmp()
	else if types == "3" then 
		data.level = "1"
	end
	end
	return data
end

local dispatch_tbl = {
	set = {
		["never"] = { cb = ignore_forever},
		["remind"] = { cb = ignore_tmp},
		["upgrade"] = { cb = set_upgrade_flag},
		["read"] = { cb = read_upgrade_information}
	},
	read = {
        ["read"]  = { cb  = read_is_upgrade }  
    }
}

function dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end

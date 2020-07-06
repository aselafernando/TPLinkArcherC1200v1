--[[
Copyright(c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  wireless.lua
Details :  wireless configure class
Author  :  Chen Jinfu <chenjinfu@tp-link.net>
Version :  1.0.0
Date    :  21Feb, 2014
]]--

local sys = require "luci.sys"
local utl = require "luci.util"
local uci = require "luci.model.uci"
local uci_r = uci.cursor()
local ip = require "luci.ip"
local sys_config = require "luci.sys.config"

module("luci.model.wireless", package.seeall)

-- Wifi shell script file path.
local APCFG_SHELL="env -i /sbin/wifi"

-- Wifi config and action enum values, using prime number sequence.
local ACT_NOTHING = 1
local ACT_COUNTRY, ACT_DEVMODE, ACT_VAP_CFG, ACT_DEV_CFG, ACT_HOSTAPD, ACT_DISABLE, ACT_RENABLE, ACT_RE_LOAD, ACT_MAC_FLT, ACT_VLAN_ID = 2, 3, 5, 7, 11, 13, 17, 19, 23, 29
local CFG_HST_2G, CFG_HST_5G, CFG_GST_2G, CFG_GST_5G, CFG_STA_2G, CFG_STA_5G, CFG_DEV_2G, CFG_DEV_5G, CFG_MACFLT = 2, 3, 5, 7, 11, 13, 17, 19, 23
local CFG_VAP_2G = CFG_HST_2G * CFG_GST_2G * CFG_STA_2G
local CFG_VAP_5G = CFG_HST_5G * CFG_GST_5G * CFG_STA_5G
local CFG_VAP_AL = CFG_VAP_2G * CFG_VAP_5G
local CFG_DEV_AL = CFG_DEV_2G * CFG_DEV_5G
local CFG_HST_AL = CFG_HST_2G * CFG_HST_5G
local CFG_GST_AL = CFG_GST_2G * CFG_GST_5G
local CFG_STA_AL = CFG_STA_2G * CFG_STA_5G

-- Wifi apply action to shell command mapping.
local APCFG_ACTION = {
	[ACT_COUNTRY] = {cmd="country", sync = true},
	[ACT_DEVMODE] = {cmd="mode"},
	[ACT_VAP_CFG] = {cmd="vap"},
	[ACT_DEV_CFG] = {cmd="radio"},
	[ACT_HOSTAPD] = {cmd="vap"},
	[ACT_DISABLE] = {cmd="down"},
	[ACT_RENABLE] = {cmd="up"},
	[ACT_RE_LOAD] = {cmd="reload"},
	[ACT_MAC_FLT] = {cmd="macfilter"},
	[ACT_VLAN_ID] = {cmd="vlan"},
}

-- Wifi UI item to UCI config item mapping.
local APCFG_DATA = {
	region = {
		{cfg=CFG_DEV_AL, act=ACT_COUNTRY, opt="country"},
		{cfg=CFG_DEV_AL, act=ACT_NOTHING, cvt="capability"},
		{cfg = CFG_DEV_AL, act = ACT_NOTHING, cvt = "region_select_permission"},
	},

	guest = {
		{cfg=CFG_GST_AL, act=ACT_VAP_CFG, opt="isolate", 			cvt="off_on"},
		{cfg=CFG_GST_AL, act=ACT_VLAN_ID, opt="access",  			cvt="on_off"},
	},
	
	macfilter = {
		{cfg=CFG_MACFLT, act=ACT_MAC_FLT, opt="enable", 			cvt="on_off"},
		{cfg=CFG_MACFLT, act=ACT_MAC_FLT, opt="action", 			val={"allow", "deny"}},
	},
	
	syspara_wps = {
		{cfg=CFG_HST_AL, act=ACT_HOSTAPD, opt="wps", 					cvt="on_off"},
	},

	wps_pin = {
		{cfg=CFG_DEV_AL, act=ACT_NOTHING, opt="disabled"},
		{cfg=CFG_HST_AL, act=ACT_NOTHING, opt="wps_pbc", 			cvt="on_off"},
		{cfg=CFG_HST_AL, act=ACT_NOTHING, opt="wps_timeout"},
		{cfg=CFG_HST_AL, act=ACT_NOTHING, opt="wps_label",		cvt="on_off"},
		{cfg=CFG_HST_AL, act=ACT_NOTHING, opt="wps_pin"},
	},
	
	wireless_2g = {
		{cfg=CFG_DEV_2G, act=ACT_NOTHING, opt="macaddr"},
		{cfg=CFG_DEV_2G, act=ACT_DEVMODE, opt="disabled",                       cvt="on_off"},
		{cfg=CFG_DEV_2G, act=ACT_RE_LOAD, opt="disabled_all",                   cvt="on_off"},
		{cfg=CFG_DEV_2G, act=ACT_NOTHING, cvt="extinfo"},
		{cfg=CFG_STA_2G, act=ACT_NOTHING, cvt="wds_status"},
		{cfg=CFG_DEV_2G, act=ACT_DEVMODE, opt="hwmode", 			val={"n", "gn", "bgn"}},
		{cfg=CFG_DEV_2G, act=ACT_DEVMODE, opt="htmode", 			cvt="htmode_2g"},
		{cfg=CFG_DEV_2G, act=ACT_DEVMODE, opt="channel", 			cvt="channel_2g"},
		{cfg=CFG_DEV_2G, act=ACT_DEVMODE, opt="txpower", 			val={"low", "middle", "high"}},
		{cfg=CFG_VAP_2G, act=ACT_NOTHING, cvt="current_channel"},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="enable", 			cvt="on_off"},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="ssid"},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="wps_state"},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="hidden", 			cvt="on_off"},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="encryption", 	val={"none", "psk", "wpa", "wep"}},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="psk_version",	val={"auto", "wpa", "rsn"}},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="psk_cipher",		val={"auto", "aes", "ccmp", "tkip"}},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="psk_key"},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="server"},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="port"},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="wpa_key"},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="wpa_version",	val={"auto", "wpa", "rsn"}},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="wpa_cipher",		val={"auto", "aes", "ccmp", "tkip"}},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="wep_mode",			val={"auto", "open", "shared"}},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="wep_select",		val={"1", "2", "3", "4"}},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="wep_format1",	val={"asic", "hex"}},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="wep_format2",	val={"asic", "hex"}},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="wep_format3",	val={"asic", "hex"}},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="wep_format4",	val={"asic", "hex"}},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="wep_type1",		val={"64", "128", "152"}},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="wep_type2",		val={"64", "128", "152"}},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="wep_type3",		val={"64", "128", "152"}},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="wep_type4",		val={"64", "128", "152"}},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="wep_key1",			cvt="wep_key"},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="wep_key2",			cvt="wep_key"},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="wep_key3",			cvt="wep_key"},
		{cfg=CFG_HST_2G, act=ACT_VAP_CFG, opt="wep_key4",			cvt="wep_key"},
	},

	syspara_2g = {
		{cfg=CFG_DEV_2G, act=ACT_NOTHING, opt="disabled"},
		{cfg=CFG_DEV_2G, act=ACT_NOTHING, cvt="extinfo"},
		{cfg=CFG_DEV_2G, act=ACT_DEV_CFG, opt="beacon_int"},
		{cfg=CFG_DEV_2G, act=ACT_DEV_CFG, opt="rts"},
		{cfg=CFG_DEV_2G, act=ACT_DEV_CFG, opt="frag"},
		{cfg=CFG_DEV_2G, act=ACT_DEV_CFG, opt="dtim_period"},
		{cfg=CFG_DEV_2G, act=ACT_DEV_CFG, opt="wpa_group_rekey"},
		{cfg=CFG_DEV_2G, act=ACT_DEV_CFG, opt="wmm", 					cvt="on_off"},
		{cfg=CFG_DEV_2G, act=ACT_DEV_CFG, opt="shortgi", 			cvt="on_off"},
		{cfg=CFG_DEV_2G, act=ACT_DEV_CFG, opt="isolate", 			cvt="on_off"},
		{cfg=CFG_STA_2G, act=ACT_VAP_CFG, opt="enable", 			cvt="on_off"},
		{cfg=CFG_STA_2G, act=ACT_VAP_CFG, opt="ssid",         cvt="root_ssid"},
		{cfg=CFG_STA_2G, act=ACT_VAP_CFG, opt="bssid"},
		{cfg=CFG_STA_2G, act=ACT_VAP_CFG, opt="encryption", 	val={"none", "psk", "wep"}},
		{cfg=CFG_STA_2G, act=ACT_VAP_CFG, opt="psk_key"},
		{cfg=CFG_STA_2G, act=ACT_VAP_CFG, opt="wep_mode",			val={"auto", "open", "shared"}},
		{cfg=CFG_STA_2G, act=ACT_VAP_CFG, opt="wep_select",		val={"1", "2", "3", "4"}},
		{cfg=CFG_STA_2G, act=ACT_VAP_CFG, opt="wep_format1",	val={"asic", "hex"}},
		{cfg=CFG_STA_2G, act=ACT_VAP_CFG, opt="wep_format2",	val={"asic", "hex"}},
		{cfg=CFG_STA_2G, act=ACT_VAP_CFG, opt="wep_format3",	val={"asic", "hex"}},
		{cfg=CFG_STA_2G, act=ACT_VAP_CFG, opt="wep_format4",	val={"asic", "hex"}},
		{cfg=CFG_STA_2G, act=ACT_VAP_CFG, opt="wep_type1",		val={"64", "128", "152"}},
		{cfg=CFG_STA_2G, act=ACT_VAP_CFG, opt="wep_type2",		val={"64", "128", "152"}},
		{cfg=CFG_STA_2G, act=ACT_VAP_CFG, opt="wep_type3",		val={"64", "128", "152"}},
		{cfg=CFG_STA_2G, act=ACT_VAP_CFG, opt="wep_type4",		val={"64", "128", "152"}},
		{cfg=CFG_STA_2G, act=ACT_VAP_CFG, opt="wep_key1",			cvt="wep_key"},
		{cfg=CFG_STA_2G, act=ACT_VAP_CFG, opt="wep_key2",			cvt="wep_key"},
		{cfg=CFG_STA_2G, act=ACT_VAP_CFG, opt="wep_key3",			cvt="wep_key"},
		{cfg=CFG_STA_2G, act=ACT_VAP_CFG, opt="wep_key4",			cvt="wep_key"},
		{cfg=CFG_STA_2G, act=ACT_VAP_CFG, opt="wds_mode",			val={"0", "1", "2"}},
	},

	guest_2g = {
		{cfg=CFG_DEV_2G, act=ACT_NOTHING, opt="disabled"},
		{cfg=CFG_DEV_2G, act=ACT_NOTHING, cvt="extinfo"},
		{cfg=CFG_GST_2G, act=ACT_VAP_CFG, opt="enable", 			cvt="on_off"},
		{cfg=CFG_GST_2G, act=ACT_VAP_CFG, opt="ssid"},
		{cfg=CFG_GST_2G, act=ACT_VAP_CFG, opt="hidden", 			cvt="on_off"},
		{cfg=CFG_GST_2G, act=ACT_VAP_CFG, opt="encryption", 	val={"none", "psk"}},
		{cfg=CFG_GST_2G, act=ACT_VAP_CFG, opt="psk_version",	val={"auto", "wpa", "rsn"}},
		{cfg=CFG_GST_2G, act=ACT_VAP_CFG, opt="psk_cipher",		val={"auto", "aes", "ccmp", "tkip"}},
		{cfg=CFG_GST_2G, act=ACT_VAP_CFG, opt="psk_key"},
	},

	wireless_5g = {
		{cfg=CFG_DEV_5G, act=ACT_NOTHING, opt="macaddr"},
		{cfg=CFG_DEV_5G, act=ACT_DEVMODE, opt="disabled",                       cvt="on_off"},
		{cfg=CFG_DEV_5G, act=ACT_RE_LOAD, opt="disabled_all",                   cvt="on_off"},
		{cfg=CFG_DEV_5G, act=ACT_NOTHING, cvt="extinfo"},
		{cfg=CFG_STA_5G, act=ACT_NOTHING, cvt="wds_status"},
		{cfg=CFG_DEV_5G, act=ACT_DEVMODE, opt="hwmode", 			val={"n_5", "ac_5", "an_5", "nac_5", "anac_5"}},
		{cfg=CFG_DEV_5G, act=ACT_DEVMODE, opt="htmode", 			cvt="htmode_5g"},
		{cfg=CFG_DEV_5G, act=ACT_DEVMODE, opt="channel", 			cvt="channel_5g"},
		{cfg=CFG_DEV_5G, act=ACT_DEVMODE, opt="txpower", 			val={"low", "middle", "high"}},
		{cfg=CFG_VAP_5G, act=ACT_NOTHING, cvt="current_channel"},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="enable", 			cvt="on_off"},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="ssid"},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="wps_state"},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="hidden", 			cvt="on_off"},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="encryption", 	val={"none", "psk", "wpa", "wep"}},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="psk_version",	val={"auto", "wpa", "rsn"}},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="psk_cipher",		val={"auto", "aes", "ccmp", "tkip"}},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="psk_key"},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="server"},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="port"},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="wpa_key"},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="wpa_version",	val={"auto", "wpa", "rsn"}},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="wpa_cipher",		val={"auto", "aes", "ccmp", "tkip"}},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="wep_mode",			val={"auto", "open", "shared"}},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="wep_select",		val={"1", "2", "3", "4"}},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="wep_format1",	val={"asic", "hex"}},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="wep_format2",	val={"asic", "hex"}},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="wep_format3",	val={"asic", "hex"}},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="wep_format4",	val={"asic", "hex"}},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="wep_type1",		val={"64", "128", "152"}},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="wep_type2",		val={"64", "128", "152"}},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="wep_type3",		val={"64", "128", "152"}},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="wep_type4",		val={"64", "128", "152"}},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="wep_key1",			cvt="wep_key"},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="wep_key2",			cvt="wep_key"},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="wep_key3",			cvt="wep_key"},
		{cfg=CFG_HST_5G, act=ACT_VAP_CFG, opt="wep_key4",			cvt="wep_key"},
	},

	syspara_5g = {
		{cfg=CFG_DEV_5G, act=ACT_NOTHING, opt="disabled"},
		{cfg=CFG_DEV_5G, act=ACT_NOTHING, cvt="extinfo"},
		{cfg=CFG_DEV_5G, act=ACT_DEV_CFG, opt="beacon_int"},
		{cfg=CFG_DEV_5G, act=ACT_DEV_CFG, opt="rts"},
		{cfg=CFG_DEV_5G, act=ACT_DEV_CFG, opt="frag"},
		{cfg=CFG_DEV_5G, act=ACT_DEV_CFG, opt="dtim_period"},
		{cfg=CFG_DEV_5G, act=ACT_DEV_CFG, opt="wpa_group_rekey"},
		{cfg=CFG_DEV_5G, act=ACT_DEV_CFG, opt="wmm", 					cvt="on_off"},
		{cfg=CFG_DEV_5G, act=ACT_DEV_CFG, opt="shortgi", 			cvt="on_off"},
		{cfg=CFG_DEV_5G, act=ACT_DEV_CFG, opt="isolate", 			cvt="on_off"},
		{cfg=CFG_STA_5G, act=ACT_VAP_CFG, opt="enable", 			cvt="on_off"},
		{cfg=CFG_STA_5G, act=ACT_VAP_CFG, opt="ssid",         cvt="root_ssid"},
		{cfg=CFG_STA_5G, act=ACT_VAP_CFG, opt="bssid"},
		{cfg=CFG_STA_5G, act=ACT_VAP_CFG, opt="encryption", 	val={"none", "psk", "wep"}},
		{cfg=CFG_STA_5G, act=ACT_VAP_CFG, opt="psk_key"},
		{cfg=CFG_STA_5G, act=ACT_VAP_CFG, opt="wep_mode",			val={"auto", "open", "shared"}},
		{cfg=CFG_STA_5G, act=ACT_VAP_CFG, opt="wep_select",		val={"1", "2", "3", "4"}},
		{cfg=CFG_STA_5G, act=ACT_VAP_CFG, opt="wep_format1",	val={"asic", "hex"}},
		{cfg=CFG_STA_5G, act=ACT_VAP_CFG, opt="wep_format2",	val={"asic", "hex"}},
		{cfg=CFG_STA_5G, act=ACT_VAP_CFG, opt="wep_format3",	val={"asic", "hex"}},
		{cfg=CFG_STA_5G, act=ACT_VAP_CFG, opt="wep_format4",	val={"asic", "hex"}},
		{cfg=CFG_STA_5G, act=ACT_VAP_CFG, opt="wep_type1",		val={"64", "128", "152"}},
		{cfg=CFG_STA_5G, act=ACT_VAP_CFG, opt="wep_type2",		val={"64", "128", "152"}},
		{cfg=CFG_STA_5G, act=ACT_VAP_CFG, opt="wep_type3",		val={"64", "128", "152"}},
		{cfg=CFG_STA_5G, act=ACT_VAP_CFG, opt="wep_type4",		val={"64", "128", "152"}},
		{cfg=CFG_STA_5G, act=ACT_VAP_CFG, opt="wep_key1",			cvt="wep_key"},
		{cfg=CFG_STA_5G, act=ACT_VAP_CFG, opt="wep_key2",			cvt="wep_key"},
		{cfg=CFG_STA_5G, act=ACT_VAP_CFG, opt="wep_key3",			cvt="wep_key"},
		{cfg=CFG_STA_5G, act=ACT_VAP_CFG, opt="wep_key4",			cvt="wep_key"},
		{cfg=CFG_STA_5G, act=ACT_VAP_CFG, opt="wds_mode",			val={"0", "1", "2"}},
	},

	guest_5g = {
		{cfg=CFG_DEV_5G, act=ACT_NOTHING, opt="disabled"},
		{cfg=CFG_DEV_5G, act=ACT_NOTHING, cvt="extinfo"},
		{cfg=CFG_GST_5G, act=ACT_VAP_CFG, opt="enable", 			cvt="on_off"},
		{cfg=CFG_GST_5G, act=ACT_VAP_CFG, opt="ssid"},
		{cfg=CFG_GST_5G, act=ACT_VAP_CFG, opt="hidden", 			cvt="on_off"},
		{cfg=CFG_GST_5G, act=ACT_VAP_CFG, opt="encryption", 	val={"none", "psk"}},
		{cfg=CFG_GST_5G, act=ACT_VAP_CFG, opt="psk_version",	val={"auto", "wpa", "rsn"}},
		{cfg=CFG_GST_5G, act=ACT_VAP_CFG, opt="psk_cipher",		val={"auto", "aes", "ccmp", "tkip"}},
		{cfg=CFG_GST_5G, act=ACT_VAP_CFG, opt="psk_key"},
	},

}

local wifi_info = nil

--Get the cached iwinfo lib.
local function get_iwinfo()
	if not wifi_info then
		local stat, iwinfo = pcall(require, "iwinfo")
		wifi_info = iwinfo
	end
	return wifi_info
end

-- Generate a radom PIN number.
-- @param NA
-- @return string PIN number 
local function wps_new_pin()
	local acc = 0
	local pin = 0
	local seed = tostring(os.time())
	local rnd = io.open("/dev/urandom", "rb")

	if rnd then
		seed = rnd:read(4)
		rnd:close()
	end

	for i = 1, 4 do
		pin = pin * 256 + string.byte(seed, i)
	end

	math.randomseed(pin%10000000)
	pin = math.random(100000000)

	if pin < 10000000 then
		pin = pin + 10000000
	end

	for i = 0, 6 do
		acc = acc + (3 - 2 * (i % 2)) * (math.floor(pin / (10^i)) % 10)
	end

	pin = tostring(pin):sub(2,8)..tostring((10 - (acc % 10)) % 10)

	return pin
end

-- Get factory default PIN number.
-- @param NA
-- @return string PIN number 
local function wps_def_pin()
	--
	-- get wps pin in firmware
	--
	return sys_config.getsysinfo("PIN")
end

-- Check a PIN number valid or not.
-- @param NA
-- @return bool 
local function wps_check_pin(pin)
	pin = tonumber(pin)

	if pin and pin < 100000000 and pin > 10 then
		local acc = 0
		for i = 1, 7 do
			acc = acc + (1 + 2 * (i % 2)) * (math.floor(pin / (10^i)) % 10)
		end
		return (pin % 10) == ((10 - (acc % 10)) % 10)
	end

	return false
end

-- Execute an WPS command and return the outputs as a table.
-- @param vap Virtual ap interface name
-- @param str WPS command line string
-- @return table WPS command result
local function wps_do_cmd(vap, str)
	local res = {}
	local cmd = " wps %s %s" % {vap, str or ""}
	local util = require("io").popen(APCFG_SHELL..cmd)

	printf(APCFG_SHELL..cmd)

	if util then
		local ln, key, val
		while true do
			ln = util:read("*l")
			if ln then 
				if ln == "wps_shell_over" then break end
				key, val = ln:match("([^\:]+)\: (.+)")
				if not key then key = ln end
				res[key] = val or key
				printf(key.." = "..res[key])
			end
		end
		util:close()
	end

	return res
end

-- Debug console output method.
-- @param	str	String to display on console.
-- @return N/A
function printf(str)
	if str then
		-- os.execute("echo %q &>/dev/console" % (str))
	end
end

-- Fork and execute a command.
-- @return N/A
function fork(str,sync)
	if str then
		--os.execute("echo %q &>/dev/console" % (str))
		os.execute(str.." &>/dev/null"..(sync and "" or " &"))
	end
end

-- Define class APCFG.
APCFG = utl.class()

-- Initialization for APCFG class.
-- @param	cfg	Config data.
-- @return N/A
function APCFG:__init__(form, cfg)
	self.change = 1
	self.action = 1
	self.module = "wireless"

	self.uci = uci.cursor()
	self.section = self:scan_driver()
	self.section[CFG_MACFLT] = {"filter"}

	local data = {}
	if form and cfg then
		for i, name in ipairs(cfg) do
			for _, item in ipairs(APCFG_DATA[form] or {}) do
				if name == item.opt or name == item.cvt then
					data[name] = item
				end
			end
		end
	elseif form then
		if type(form) == "table" then
			for i, name in ipairs(form) do
				local prefix = name .. "_"
				for _, item in ipairs(APCFG_DATA[name] or {}) do
					data[prefix .. (item.opt or item.cvt)] = item
				end
			end
		else
			for _, item in ipairs(APCFG_DATA[form] or {}) do
				data[item.opt or item.cvt] = item
			end
		end
	end

	self.data = data
end

-- Execute an WPS command and return the result.
-- @param vap Virtual ap interface name
-- @param cmd WPS command line string
-- @param data input and output data
-- @return bool WPS command result
function APCFG:wps_vap_cmd(vap, cmd, data)
	local res

	if cmd == "pbc" then
		res = wps_do_cmd(vap, "pbc")
		if res.OK then return true end
		res = wps_do_cmd(vap, "status")
		data.wps_status = (res["PBC Status"] == "Overlap" and "overlap") or "error"
	elseif  cmd == "pin" then
		if not wps_check_pin(data.pin) then
			data.wps_status = "error"
			return false
		end
		res = wps_do_cmd(vap, "pin "..data.pin)
		if res.OK then return true end
		res = wps_do_cmd(vap, "status")
		data.wps_status = (res["PIN Status"] == "Invalid" and "faild") or "error"
	elseif  cmd == "cancel" then
		res = wps_do_cmd(vap, "cancel")
		if res.OK then
			data.wps_status = "cancel"
			return true
		end
	elseif  cmd == "status" then
		res = wps_do_cmd(vap, "status")
		local method = data.method

		if method ~= "pin" and method ~= "pbc" then
			method = (res["PIN Status"] == "Active" and "pbc") or "pbc"
			data.method = method
		end

		if data.wps_status ~= "success" and data.wps_status ~= "failed" then
			if res["Peer Address"] then
				if res["Last WPS result"] == "Success" then
					data.mac = res["Peer Address"]
					data.wps_status = "success"
				else
					data.wps_status = "failed"
				end
			else
				local status = res[method:upper().." Status"]
				if status and (data.wps_status ~= "timeout" or data.wps_status ~= "overlap") then
					local map = {["Active"]="ok", ["Overlap"]="overlap", ["Timed-out"]="timeout", ["Disabled"]="na"}
					data.wps_status = map[status] or "na"
				end
			end
		end
		return true
	elseif cmd == "ap_pin" then
		if data.wps_label == "on" then
			res = wps_do_cmd(vap, "wps_ap_pin set "..data.wps_pin.." 0")
		else
			res = wps_do_cmd(vap, "wps_ap_pin disable")
		end
		return res.OK or not res.FAIL
	elseif cmd == "pin_lock" then
		res = wps_do_cmd(vap, "pin_lock")
		local sec
		for _, sec in pairs(self.section[CFG_HST_2G]) do
			if vap == sec then
				data.lock_2g = string.lower(res["LockDown"])
			end
		end
		for _, sec in pairs(self.section[CFG_HST_5G]) do
			if vap == sec then
				data.lock_5g = string.lower(res["LockDown"])
			end
		end
		return true
	end

	return false
end

-- Execute an WPS command.
-- @param	cmd	 WPS command name.
-- @param	data	input or output data.
-- @return bool 
function APCFG:wps_cmd(cmd, data)
	for _, vap in pairs(self.section[CFG_HST_AL]) do
		local cfg = self.uci:get_all(self.module, vap)
		if cfg and cfg.enable == "on"	and cfg.wps == "on" and cfg.hidden == "off"
			and (cfg.encryption == "none"	or (cfg.encryption == "psk" and cfg.psk_cipher ~= "tkip" and cfg.psk_version ~= "wpa")) then
				if not cmd then
					return true
				else
					if not self:wps_vap_cmd(vap, cmd, data) then
						return false
					end
					break
				end
		end
	end
	return (cmd and true) or false
end

-- Check the given MAC filtering rule is valid or not.
-- @param	data	Values of the rule.
-- @return bool 
function APCFG:maclist_check(data)
	if data.mac and data.mac:match("^%x[02468aceACE]%-%x%x%-%x%x%-%x%x%-%x%x%-%x%x$")
		and (data.enable == "on" or data.enable == "off") 
		and (not data.note or #data.note < 32) then
		return true
	end
	return false
end

-- Remove a/some MAC filtering rule.
-- @param	index	Indexs to be removed.
-- @param	data	Result of every rule.
-- @return bool 
function APCFG:maclist_remove(index, data)
	local id = 0
	local result, old_cfg
	local tmp = {}
	local filter = self.uci:get(self.module, "filter", "enable") == "on"
	
	if type(index) == "string" then
		index = {index}
	end

	for i, v in ipairs(index) do
		id = tonumber(v)
		if id then tmp[id] = false end
	end
	
	id = -1
	self.uci:delete_all(self.module, "mac-list",
			function(s)
				id = id + 1
				if tmp[id] == false then
					tmp[id] = true
					old_cfg = s
					return true
				end
				return false
			end)

	for i, r in pairs(tmp) do
		data[#data + 1] = {index = i, success = r}
	end
	-- Commit the delete operation
	result = old_cfg and self.uci:commit(self.module)

	-- Delete the rule from driver
	if result and filter then
		if #data == 1 then
			fork("%s %s del %s" % {APCFG_SHELL, APCFG_ACTION[ACT_MAC_FLT].cmd, old_cfg.mac})
		else
			fork("%s %s" % {APCFG_SHELL, APCFG_ACTION[ACT_MAC_FLT].cmd})
		end
	end

	return result
end

-- Insert a MAC filtering rule.
-- @param	data	Values of the new rule.
-- @return bool 
function APCFG:maclist_insert(data)
	local id = -1
	local result = true
	local mac = data.mac
	local limit = self.uci:get(self.module, "filter", "limit") or "64"
	local filter = self.uci:get(self.module, "filter", "enable") == "on"
	
	data = {
		mac = data.mac,
		note = data.note or "",
		enable = data.enable,
	}

	limit = tonumber(limit)
	self.uci:foreach(self.module, "mac-list",
		function(s)
			limit = limit - 1
			if s.mac == mac then
				result = false
			end
		end)

	result = result and limit > 0
	result = result and self.uci:section_first(self.module, "mac-list", nil, data)
	result = result and self.uci:commit(self.module)

	-- Insert the rule to driver
	if result and filter and data.enable == "on" then
		fork("%s %s add %s" % {APCFG_SHELL, APCFG_ACTION[ACT_MAC_FLT].cmd, data.mac})
	end

	return result
end

-- Modify a MAC filtering rule by index.
-- @param	index	Index of the rule to be updated.
-- @param	data	Values of the rule.
-- @return bool 
function APCFG:maclist_update(index, data)
	local id = -1
	local old_cfg
	local result = true
	local mac = data.mac
	local filter = self.uci:get(self.module, "filter", "enable") == "on"

	data = {
		mac = data.mac,
		note = data.note or "",
		enable = data.enable,
	}

	self.uci:foreach(self.module, "mac-list",
		function(s)
			id = id + 1
			if id == index then
				old_cfg = s
			elseif s.mac == mac then
				result = false
			end
		end)

	-- Have we found the entry?
	if result and old_cfg then
		result = self.uci:tset(self.module, old_cfg['.name'], data)
		result = result and self.uci:commit(self.module)

		-- Remove old rule and apply new rule
		if result and filter then
			if old_cfg.enable == "on" then
				fork("%s %s del %s" % {APCFG_SHELL, APCFG_ACTION[ACT_MAC_FLT].cmd, old_cfg.mac})
			end
			if data.enable == "on" then
				fork("%s %s add %s" % {APCFG_SHELL, APCFG_ACTION[ACT_MAC_FLT].cmd, data.mac})
			end
		end
	end

	return result
end

-- Modify one/all MAC filtering.
-- @param	index	Index of the rule to be read, nil means ALL.
-- @param	data	Output rules.
-- @return bool 
function APCFG:maclist_read(index, data)
	local id = -1

	self.uci:foreach(self.module, "mac-list",
		function(s)
			id = id + 1
			if not index or id == index then
				data[#data + 1 ] = {
					mac = s.mac,
					enable = s.enable,
					note = s.note or ""
				}
			end
		end)

	return true
end

-- Sort the ap list by channel.
-- @param	ap_list	ap list to be sorted.
-- @return ap list sorted.
function sort_aplist(ap_list)
	table.sort(ap_list, function(a,b)
	if a.channel == nil then
		return false
	elseif b.channel == nil then
		return true
	else
		return (a.channel < b.channel)
	end
	end)
end

-- Get all scan SSID results on wifi chip(s).
-- @param	survery_2g	Do survery on 2G radio.
-- @param	survery_5g	Do survery on 5G radio.
-- @return table All scan results
function APCFG:scanlist(survery_2g, survery_5g)
	local iwinfo = get_iwinfo()
	local ap_list = {}
	local map = {
		[CFG_DEV_2G] = (survery_2g and CFG_VAP_2G) or nil,
		[CFG_DEV_5G] = (survery_5g and CFG_VAP_5G) or nil,
	}

	if not iwinfo then return {} end
	for phy, sec in pairs(map) do
		local dev = self.section[phy][1]
		local iftype = dev and iwinfo.type(dev)
		local iw = iftype and iwinfo[iftype]

		if dev and sec and iw then
			for _, vap in ipairs(self.section[sec]) do
				if iftype == iwinfo.type(vap) then 
					dev = vap
					break
				end
			end
			for _, ap in ipairs(iw.scanlist(dev) or {}) do
				local enc = "none"
				if ap.encryption.wep then 
					enc = "wep" 
				elseif ap.encryption.wpa > 0 then
					enc = ap.encryption.auth_suites
					if enc and enc[1] then
						if enc[1] == "PSK" then
							enc = "psk"
						elseif enc[1] == "802.1X" then
							enc = "wpa"
						elseif enc[1] == "NONE" then
							enc = "none"
						else
							enc = "unknown"
						end
					else
						enc = "unknown"
					end
				elseif ap.encryption.enabled then
					enc = "unknown"
				end
				ap_list[#ap_list + 1] = {
					bssid = ap.bssid:gsub(':', '-'),
					ssid = ap.ssid,
					signal = ap.signal + 91,
					channel = ap.channel,
					encryption = enc,
				}
			end
		end
	end
	sort_aplist(ap_list)
	return ap_list
end

-- Get all associted stations.
-- @param	N/A
-- @return table All stations
function APCFG:assoclist()
	local sta = {}
	local map = {CFG_HST_2G, CFG_HST_5G, CFG_GST_2G, CFG_GST_5G}
	local iwinfo = get_iwinfo()
	local wifi_type = ""

	for id, sec in ipairs((iwinfo and map) or {}) do
		for _, vap in pairs(self.section[sec]) do
			local iftype = iwinfo.type(vap)
			local iw = iftype and iwinfo[iftype]
			if iw then
				local enc = iw.encryption(vap)
				local sec = "none"
				if enc.wep then
					sec = "wep" 
				elseif enc.wpa > 0 then
					if enc.wpa == 1 then
						sec = "wpa"
					elseif enc.wpa == 2 then
						sec = "wpa2"
					elseif enc.wpa == 3 then
						sec = "wpa/wpa2"
					else
						sec = "unknow"
					end

					-- FIXME: these codes should work but not.
					-- It can be another bug.
					if enc.auth_suites and enc.auth_suites[1] then
						if enc.auth_suites[1] == "PSK" then
							sec = sec .. "-psk"
						end
					end

					-- FIXME: a temporary fix.
					--[[
					local encopt = self.uci:get(self.module, vap, "encryption")
					if encopt == "psk" then
						sec = sec .. "-psk"
					end
					--]]
				elseif enc.enabled then
					sec = "unknow"
				end

				if wifi_type == "" then
					local ifname = self.uci:get(self.module, vap, "device")
					wifi_type = self.uci:get(self.module, ifname, "type")
				end

				if wifi_type == "brcmwifi" then
					local enable = self.uci:get(self.module, vap, "enable")
					local mode = self.uci:get(self.module, vap, "mode")
					local guest = self.uci:get(self.module, vap, "guest") or ""
					local guest_enable = "off"

					if enable == "on" and mode == "ap" and guest == "on" then
						guest_enable = "on"
					end

					if guest_enable ~= "on" and id > 2 then
						id = id - 2
					end
				end

				for mac, data in pairs(iw.assoclist(vap) or {}) do
					data.mac = mac:gsub(':', '-')
					data.type = id
					data.security = sec
					sta[#sta + 1] = data
				end
			end
		end
	end

	return sta
end

-- Scan wireless uci configure file, generate wifi device and interface table.
-- @param	N/A.
-- @return N/A
function APCFG:scan_driver()
	local dev2g, dev5g, hst2g, gst2g, sta2g, hst5g, gst5g, sta5g

	self.uci:foreach(self.module, "wifi-device",
		function(s)
			if dev2g or s['band'] == '5g' then
				dev5g = s['.name']
			else
				dev2g = s['.name']
			end
		end)

	self.uci:foreach(self.module, "wifi-iface",
		function(s)
			if s['device'] == dev2g then
				if s['mode'] ~= 'ap' then
					sta2g = s['.name']
				elseif hst2g or s['guest'] then
					gst2g = s['.name']
				else
					hst2g = s['.name']
				end
			else
				if s['mode'] ~= 'ap' then
					sta5g = s['.name']
				elseif hst5g or s['guest'] then
					gst5g = s['.name']
				else
					hst5g = s['.name']
				end
			end
		end)

	return {
		[CFG_HST_2G] = {hst2g}, [CFG_GST_2G] = {gst2g}, [CFG_STA_2G] = {sta2g},
		[CFG_HST_5G] = {hst5g}, [CFG_GST_5G] = {gst5g}, [CFG_STA_5G] = {sta5g},
		[CFG_DEV_2G] = {dev2g}, [CFG_DEV_5G] = {dev5g}, [CFG_DEV_AL] = {dev2g, dev5g},
		[CFG_HST_AL] = {hst2g, hst5g}, [CFG_GST_AL] = {gst2g, gst5g}, [CFG_STA_AL] = {sta2g, sta5g},
		[CFG_VAP_2G] = {hst2g, gst2g, sta2g}, [CFG_VAP_5G] = {hst5g, gst5g, sta5g},
		[CFG_VAP_AL] = {hst2g, gst2g, sta2g, hst5g, gst5g, sta5g}
	}
end

-- Set uci option value.
-- @param	cfg.
-- @param	option.
-- @param	value.
-- @return N/A
function APCFG:set_option(cfg, option, value, convert)
	local set_func = self["set_"..(convert or option)]

	printf("set %s %s = %s" % {self.section[cfg][1] or "", option or "", value or ""})
	
	if option then
		if not value then return true end
		if set_func then value = set_func(self, value) end
		if value then
			for _, section in pairs(self.section[cfg]) do
				self.uci:set(self.module, section, option, value)
			end
		end
		return value and true
	else
		return (set_func and set_func(self, self.section[cfg])) or true
	end
end

-- Get uci option value.
-- @param cfg.
-- @param	option.
-- @return N/A
function APCFG:get_option(cfg, option, convert)
	local value = ""
	local get_func = self["get_"..(convert or option)]

	if option then
		for _, section in pairs(self.section[cfg]) do
			value = self.uci:get(self.module, section, option)
			break
		end
		if get_func then value = get_func(self, value) end
	else
		value = get_func and get_func(self, cfg)
	end

	printf("get %s %s = %s" % {self.section[cfg][1] or "", option or convert or "", (type(value)=="string" and value) or ""})
	
	return value or ""
end

-- Save uci config file, calculating changes and action.
-- @param	N/A.
-- @return N/A
function APCFG:commit()
	local changes = self.uci:changes(self.module)[self.module]
	
	if changes then
		for i, item in pairs(self.data) do
			for _, section in pairs(self.section[item.cfg]) do
				if (self.change % item.cfg > 0) and changes[section] and changes[section][item.opt] then
					self.change = self.change * item.cfg
					if self.action % item.act > 0 then
						self.action = self.action * item.act
					end
				end
			end
		end
		-- set WPS configure status to 'configured' when change host vap config.
		for _, sec in ipairs({CFG_HST_2G, CFG_HST_5G}) do
			if self:get_option(sec, "wps_state") ~= "configured" and self.change % sec == 0 then 
				self:set_option(sec, "wps_state", "configured")
			end
		end
		self.uci:commit(self.module)
	end
end

-- Apply uci config changes, let then take effect.
-- @param	N/A.
-- @return N/A
function APCFG:apply()
	local sync
	local apply_cmd
	for act, cmd in pairs(APCFG_ACTION) do
		if self.action % act == 0 then
			local command = cmd.cmd
			local change = self.change
			sync = sync or cmd.sync
			for sec, items in pairs(self.section) do
				if change % sec == 0 then
					change = change / sec
					for _, item in pairs(items) do
						command = command .. " " .. item
					end
				end
			end
			if apply_cmd then
				if (CFG_VAP_2G * CFG_DEV_2G) % self.change == 0 then
					apply_cmd = "%s %s %s" % {APCFG_SHELL, APCFG_ACTION[ACT_RE_LOAD].cmd, (self.section[CFG_DEV_2G][1] or "")}
				elseif  (CFG_VAP_5G * CFG_DEV_5G) % self.change == 0 then
					apply_cmd = "%s %s %s" % {APCFG_SHELL, APCFG_ACTION[ACT_RE_LOAD].cmd, (self.section[CFG_DEV_5G][1] or "")}
				else
					apply_cmd = APCFG_SHELL
				end
			else
				apply_cmd = APCFG_SHELL.." "..command
			end
		end
	end

	fork(apply_cmd, sync)
end

-- Read all data needed, return data table(for json).
-- @param	N/A.
-- @return N/A
function APCFG:read_data()
	local data = {}

	for i, item in pairs(self.data) do
		data[i] = self:get_option(item.cfg, item.opt, item.cvt)
	end

	return data
end

-- Read operation.
-- @param	N/A.
-- @return N/A
function APCFG:read()
	return {operator = "read", success = true, data = self:read_data()}
end

-- Withdraw current write operation.
-- @param	N/A.
-- @return N/A
function APCFG:withdraw(reason)
	self.uci:revert(self.module)

	return {operator = "write", success = false, data = self:read_data(), reason = reason}
end

-- Write operation.
-- @param	N/A.
-- @return N/A
function APCFG:write(formvalue)
	local data = {}

	if not formvalue or type(formvalue) ~= "table" then
		return self:withdraw()
	end
	for name, item in pairs(self.data) do
		item.value = formvalue[name]
	end
	for _, item in pairs(self.data) do
		local value = item.value
		local valid = not (item.val and value)
		for i, v in ipairs(item.val or {}) do
			if v == value then 
				valid = true
				break
			end
		end
		if not valid then printf("ERROR: ".._..value.." is invalid!")	end
		if not (valid and self:set_option(item.cfg, item.opt, value, item.cvt)) then
			return self:withdraw(item.opt or item.cvt)
		end
	end
	
	self:commit()
	self:apply()
	if formvalue["form"]=="syspara_2g" or formvalue["form"]=="syspara_5g" then
		sys.fork_call("/etc/hotplug.d/iface/99-wportal")
	end
	return {operator = "write", success = true, data = self:read_data()}
end

function APCFG:get_on_off(value)	return (value == "on" and "on") or "off" end
function APCFG:set_on_off(value)	return (value == "on" and "on") or "off" end
function APCFG:set_off_on(value)	return (value == "on" and "off") or "on" end
function APCFG:get_off_on(value)	return (value == "on" and "off") or "on" end

----------------------------------------------------------------------------------------------------
--                   The codes below this comment are uci get/set functions.                      --
----------------------------------------------------------------------------------------------------
--GET current radio working channel
function APCFG:get_current_channel(secs)
	local chan
	for _, sec in pairs(self.section[secs]) do
		local iwinfo = get_iwinfo()
		local iftype = iwinfo and iwinfo.type(sec)
		local iw = iftype and iwinfo[iftype]
		chan = iw and iw.channel(sec)
		if chan then
			chan = tostring(chan)
			break
		end
	end
	return chan or "N/A"
end

-- Get wds status.
-- @param	sec	section.
-- @return wds status
function APCFG:get_wds_status(sec)
	local wds_status = "disable"
	local iwinfo = get_iwinfo()
	
	for _, vap in pairs(self.section[sec]) do
		local ifname = self.uci:get(self.module, vap, "ifname")
		local iftype = iwinfo.type(ifname)
		local iw = iftype and iwinfo[iftype]
		if iw then
			if iw.bssid(ifname) and iw.bssid(ifname) ~= "00:00:00:00:00:00" and iw.channel(ifname) then
				local enc = iw.encryption(ifname)
				if enc.enabled then
					wds_status = "run_encrypt"
				else
					wds_status = "run_unencrypt"
				end
			else
				wds_status = "assoc"
			end
		end
	end
	return wds_status
end

--judge region selection is permitted.
function APCFG:get_region_select_permission(secs)
    local select_permission
    select_permission = uci_r:get_profile("region","select_permission") or "yes"
    return select_permission
end

--GET available channel and modes in current region.
function APCFG:get_capability(secs)
	if not self.capability then
		local uci_state = uci.cursor_state()
		local country = self:get_option(CFG_DEV_AL, "country")
		local cap = uci_state:get_all(self.module, "capability")

		if not cap or cap.country ~= country then
			local iwinfo = get_iwinfo()
			local success = false
			cap = {country = country}
			for idx, dev in ipairs(self.section[secs or CFG_DEV_AL]) do
				local htmode = {}
				local hwmode = {}
				local channel = {}
				local ht20, ht40, ht80
				local is5g = (self.section[CFG_DEV_5G][1] == dev)
				local vaps = self.section[is5g and CFG_VAP_5G or CFG_VAP_2G]
				local iftype = iwinfo and iwinfo.type(dev)
				local iw = iftype and iwinfo[iftype]
				if iw then
					local hw = iw.hwmodelist(dev) or {}

					-- Decode hwinfo list infomation.
					if is5g then
						if hw.ac then
							hwmode[#hwmode + 1] = "ac_5"
							if hw.n then
								hwmode[#hwmode + 1] = "nac_5"
							end
							if hw.a and hw.n then
								hwmode[#hwmode + 1] = "anac_5"
							end
						else
							if hw.n then
								hwmode[#hwmode + 1] = "n_5"
							end
							if hw.n and hw.a then
								hwmode[#hwmode + 1] = "an_5"
							end
						end
					else
						if hw.g and hw.n then
							hwmode[#hwmode + 1] = "n"
							hwmode[#hwmode + 1] = "gn"
						end
						if hw.g and hw.n and hw.b then
							hwmode[#hwmode + 1] = "bgn"
						end
					end

					-- Try to find a already created vap on this device
					for _, vap in ipairs(vaps) do
						if iwinfo.type(vap) then dev = vap break end
					end
					for _, chan in ipairs(iw.freqlist(dev) or {}) do
						if is5g == (chan.mhz > 4000) then 
							local flag = tonumber(chan.flags) or 0
							ht20 = ht20 or ((flag % 64) < 32)
							ht40 = ht40 or ((flag % 128) < 64)
							ht80 = ht80 or ((flag % 256) < 128)
							channel[#channel + 1] = tostring(chan.channel)
						end
					end

					-- Generate htmode list from channel flag info.
					if ht20 then htmode[#htmode + 1] = "20" end
					if ht40 then htmode[#htmode + 1] = "40" end
					if ht80 then htmode[#htmode + 1] = "80" end

					-- Is any channel supported in country?
					if #channel > 0 then
						if is5g then
							cap.htmode_5g  = htmode
							cap.hwmode_5g  = hwmode
							cap.channel_5g = channel
						else
							cap.htmode_2g  = htmode
							cap.hwmode_2g  = hwmode
							cap.channel_2g = channel
						end
					end
					success = true
				end
			end

			uci_state:revert(self.module)
			if success then uci_state:section(self.module, country, "capability", cap) end
			uci_state:save(self.module)
		end
		self.capability = {
			htmode_2g  = cap.htmode_2g  or {},
			htmode_5g  = cap.htmode_5g  or {},
			hwmode_2g  = cap.hwmode_2g  or {},
			hwmode_5g  = cap.hwmode_5g  or {},
			channel_2g = cap.channel_2g or {},
			channel_5g = cap.channel_5g or {},
		}
	end

	return self.capability
end

--GET available channel and modes in current region.
function APCFG:get_extinfo(secs)
	local extinfo = {}
	local band = "none"
	if #self:get_capability().channel_2g > 0 and #self:get_capability().channel_5g > 0 then
		band = "both"
	elseif #self:get_capability().channel_2g > 0 then
		band = "2G"
	elseif #self:get_capability().channel_5g > 0 then
		band = "5G"
	else
		band = "none"
	end
	wds_show = uci_r:get_profile("wireless","wds_show") or "yes"
	extinfo = {support_band = band,support_wds_show = wds_show,}
	return extinfo
end

--GET mac address 
function APCFG:get_macaddr(val)
	if val then val = val:gsub(':', '-') end
	return (val and val:match("^%x[02468aceACE]%-%x%x%-%x%x%-%x%x%-%x%x%-%x%x$")) or "00-00-00-00-00-00"
end

--SET mac address is not allowed
function APCFG:set_macaddr(val)
	return nil
end

--SET device disabled is not allowed
function APCFG:set_disabled(val)
	return nil
end

--GET wps_state 
function APCFG:get_wps_state(val)
	return (val == "2" and "configured") or "unconfigured"
end

--SET wps_state is allowed and alway set it as 2.
function APCFG:set_wps_state(val)
	return "2"
end

--SET enable
function APCFG:set_enable(val, sec)
	local dev_disabled = self:get_option(CFG_DEV_AL, "disabled")
	if dev_disabled == "on" then
		return self:get_option(sec, "enable")
	else
		return val
	end
end

--GET check 2G channel
function APCFG:set_channel_2g(val)
	if val == "auto" then return val end
	if val == "14" then
		local hwmode = self.data.hwmode or self.data.wireless_2g_hwmode
		hwmode = hwmode or self:get_option(CFG_DEV_2G, "hwmode")
		if not hwmode or hwmode.value ~= "b" then return nil end
	end
	for _, i in ipairs(self:get_capability().channel_2g) do
		if val == i then return val	end
	end
	return nil
end

--GET check 2G channel
function APCFG:get_channel_2g(val)
	if val == "auto" then return val end
	if val == "14" and self:get_option(CFG_DEV_2G, "hwmode") ~= "b" then
		return "auto"
	end
	for _, i in ipairs(self:get_capability().channel_2g) do
		if val == i then return val	end
	end
	return "auto"
end

--GET check 5G channel
function APCFG:set_channel_5g(val)
	if val == "auto" then return val end
	for _, i in ipairs(self:get_capability().channel_5g) do
		if val == i then return val	end
	end
	return nil
end

--GET check 5G channel
function APCFG:get_channel_5g(val)
	if val == "auto" then return val end
	for _, i in ipairs(self:get_capability().channel_5g) do
		if val == i then return val	end
	end
	return "auto"
end

--GET check 2G htmode
function APCFG:set_htmode_2g(val)
	if val == "auto" then return val end
	for _, i in ipairs(self:get_capability().htmode_2g) do
		if val == i then return val	end
	end
	return nil
end

--GET check 2G htmode
function APCFG:get_htmode_2g(val)
	if val == "auto" then return val end
	for _, i in ipairs(self:get_capability().htmode_2g) do
		if val == i then return val	end
	end
	return "auto"
end

--GET check 5G htmode
function APCFG:set_htmode_5g(val)
	if val == "auto" then return val end
	for _, i in ipairs(self:get_capability().htmode_5g) do
		if val == i then return val	end
	end
	return nil
end

--GET check 5G htmode
function APCFG:get_htmode_5g(val)
	if val == "auto" then return val end
	for _, i in ipairs(self:get_capability().htmode_5g) do
		if val == i then return val	end
	end
	return "auto"
end

--GET¡¡WPS_TIME_OUT, time value of micro second.
function APCFG:get_wps_timeout(val)
	return (tonumber(val) or 120) * 1000
end

--SET WPS_TIME_OUT, read only, just return nil.
function APCFG:set_wps_timeout(val)
	return nil
end

--SET WPS_PIN, generate new pin or restore factory pin.
function APCFG:set_wps_pin(val)
	if val == "generate" then
		val = wps_new_pin()
	elseif val == "default" then
		val = wps_def_pin()
	else
		val = nil
	end
	return val
end

--SET WEP_KEY, string length 5,13,16 or hex string length 10,26,32.
function APCFG:set_wep_key(val)
	if val then
		local len = #val
		if len == 5 or len == 13 or len == 16 then
			return val
		elseif len == 10 or len == 26 or len == 32 then 
			return val:match("^[%x]+$")
		end
	end
	return nil
end

--SET SERVER_PORT, IPV4 port.
function APCFG:set_port(val)
	local port = tonumber(val)
	return ( port and port >= 0 and port <= 65535 ) and val
end

--SET SERVER_IP, IPV4 address.
function APCFG:set_server(val)
	return ip.IPv4(val) and val
end

--SET WPA_GROUP_REKEY, 0 or range 30~86400.
function APCFG:set_wpa_group_rekey(val)
	local sec = tonumber(val)
	return (sec and (sec == 0 or (sec >= 30 and sec <= 86400 ))) and val
end

--SET WPA_KEY, string length 1~64.
function APCFG:set_wpa_key(val)
	if val then
		local len = #val
		if len > 0 and len <= 64 then
			return val
		end
	end
	return nil
end

--SET PSK_KEY, string length 8~63, or 64 bytes hex string.
function APCFG:set_psk_key(val)
	if val then
		local len = #val
		if len >= 8 and len < 64 then
			return val
		elseif len == 64 then
			return val:match("^[%x]+$")
		end
	end
	return nil
end

--SET BSSID, Ethernet MAC address, or null when root AP ssid is set.
function APCFG:set_bssid(val)
	if val then
		local len = #val
		if len == 0 then
			local ssid = self.data.ssid
			local enable = self.data.enable
			return (enable.value == "off" or ssid and #ssid.value > 0) and val
		else
			return val:match("^%x[02468aceACE]%-%x%x%-%x%x%-%x%x%-%x%x%-%x%x$")
		end
	end
	return nil
end

--SET ROOT SSID, string length 1~32 or 0 when bssid is set.
function APCFG:set_root_ssid(val)
	if val then
		local len = #val
		if len == 0 then
			local bssid = self.data.bssid
			local enable = self.data.enable
			return (enable.value == "off" or bssid and #bssid.value > 0) and val
		elseif len > 0 and len <= 32 then
			return val
		end
	end
	return nil
end

--SET SSID, string length 1~32.
function APCFG:set_ssid(val)
	if val then
		local len = #val
		if len > 0 and len <= 32 then
			return val
		end
	end
	return nil
end

--SET Country short name, two upper case Letter.
function APCFG:set_country(val)
	return val and val:match("^[A-Z][A-Z]$")
end

--SET Beacon interval range 40 ~ 1000
function APCFG:set_beacon_int(val)
	local ms = tonumber(val)
	return (ms and ms <= 1000 and ms >= 40) and val
end

--SET Fragementation threshold range 256 ~ 2346
function APCFG:set_frag(val)
	local byte = tonumber(val)
	return (byte and byte <= 2346 and byte >= 256) and val
end

--SET RTS threshold range 1 ~ 2346
function APCFG:set_rts(val)
	local byte = tonumber(val)
	return (byte and byte <= 2346 and byte >= 1) and val
end

--SET DTIM period range 1 ~ 15
function APCFG:set_dtim_period(val)
	local int = tonumber(val)
	return (int and int <= 15 and int >= 1) and val
end

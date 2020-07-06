--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  wireless.lua
Details :  Web controller for wireless module.
Author  :  Chen Jinfu <chenjinfu@tp-link.net>
Version :  1.0.0
Date    :  24Feb, 2014
]]--
local wlan = require("luci.model.wireless")
local uci = require "luci.model.uci"
local uci_r = uci.cursor()
local dbg = require "luci.tools.debug"

module("luci.controller.admin.wireless", package.seeall)

local CFG_HST_2G, CFG_HST_5G, CFG_GST_2G, CFG_GST_5G, CFG_DEV_2G, CFG_DEV_5G = 2, 3, 5, 7, 17, 19

-- Wireless forms, the table is informat {name=true/false/{table}, ...}.
-- false means using a function in luci.controller.admin.wireless
-- true means using pre defined forms in luci.model.wireless
-- {table} means using a subset of the pre defined forms in luci.model.wireless
local wireless_form = {
	wireless_2g = true,
	wireless_5g = true,
	guest_2g = true,
	guest_5g = true,
	syspara_2g = true,
	syspara_5g = true,
	region = true,
	guest = true,
	syspara_wps = true,
	macfilter = true,
	wps_pin = false,
	maclist = false,
	devlist = false,
	survey_2g = false,
	survey_5g = false,
	statistics = false,
	wps_connect = false,
	tmp_read = false,
	tmp_read_guest = false,
	tmp_set = false,
	tmp_set_guest = false,
}

-- Register wireless URL and RPM
-- @param N/A
-- @return N/A
function index()
	entry({"admin", "wireless"}, call("wireless_index")).leaf = true
end

-- Process wireless configure request.
-- @param N/A
-- @return N/A
function wireless_dispatch(formvalue)
	local form = formvalue["form"] or ""
	local operate = formvalue["operation"] or ""
	local rpm = (type(form) ~= "string") or wireless_form[form]	
	local json = {}
		
	if rpm then
		local ap = wlan.APCFG(form, (rpm ~= true and rpm))
		operate = ap[operate] or ap["read"]
		json = operate(ap, formvalue)
	elseif rpm == false then
		json = luci.controller.admin.wireless["wireless_"..form](formvalue)
	else
		form = {"wireless_2g", "wireless_5g", "guest_2g", "guest_5g", "guest", "region"}
		json = wlan.APCFG(form):read(formvalue)
	end

	return json
end

-- Process wireless webpage's HTTP request.
-- @param N/A
-- @return N/A
function wireless_index()
	local form = luci.http.formvalue() or {}
	local json = wireless_dispatch(form)

	-- Only for debug use!
	-- luci.http.header("Access-Control-Allow-Origin", "*")

	-- Output http response header
	luci.http.prepare_content("application/json")
	-- Output json string
	luci.http.write_json(json)
end

-- Process WPS PIN HTTP request.
-- @param N/A
-- @return N/A
function wireless_wps_pin(formvalue)
	local option = formvalue["option"]
	local label = formvalue["wps_label"]
	local ap = wlan.APCFG("wps_pin", {"wps_label", "wps_pin"})
	local cfg = nil
	local json = {}
	local res = true

	if option == "generate" or option == "default" then
		cfg = {wps_pin = option}
	elseif label == "on" or label == "off" then
		cfg = {wps_label = label}
	end

	-- Is there any commit?
	if cfg then
		local oldcfg = ap:read_data()

		ap:write(cfg)
		json = ap:read()

		-- Apply WPS config if any changed.
		if oldcfg.wps_label ~= json.data.wps_label or
			oldcfg.wps_pin ~= json.data.wps_pin then
			ap:wps_cmd("ap_pin", json.data)
		end
	else
		json = ap:read()
		json.data.lock_2g = "disable"
		json.data.lock_5g = "disable"
		res = ap:wps_cmd("pin_lock", json.data)
	end

	return json
end

-- Process WPS Connection HTTP request.
-- @param N/A
-- @return N/A
function wireless_wps_connect(formvalue)
	local option = formvalue["option"]
	local ap = wlan.APCFG("wps_pin", {"wps_timeout", "disabled"})
	local data = {wps_status = "ok"}
	local res = true

	-- Query PIN method connect status
	if option == "pin" then
		data.method = "pin"
		res = ap:wps_cmd("status", data)
		data.method = nil
	-- Query PBC method connect status
	elseif option == "pbc" then
		data.method = "pbc"
		res = ap:wps_cmd("status", data)
		data.method = nil
	-- Cancel PBC and PIN connect
	elseif option == "cancel" then
		res = ap:wps_cmd("cancel", data)
	-- Add a WPS device by PBC or PIN method
	elseif option == "connect" then
		local pin = formvalue["wps_pin"]
		-- Use PIN method if a PIN number is given.
		if pin and pin ~= "" then
			data.pin = pin
			res = ap:wps_cmd("pin", data)
			data.pin = nil
		else
			res = ap:wps_cmd("pbc", data)
		end
		file = require("io").popen("tp_wps_led &")
		if file then
			file:close()
		end
	-- Default response: 'wps_timeout' and 'available'.
	else
		data = ap:read_data()
		data.wps_timeout = data.wps_timeout + 1000
		data.available = ap:wps_cmd()
	end

	return {success = res, timeout = false, data = data}
end

-- Process wireless statistics HTTP request.
-- @param N/A
-- @return N/A
function wireless_statistics(formvalue)
	local operate = formvalue["operation"] or "read"
	local map = {"2.4GHz", "5GHz", "Guest 2.4GHz", "Guest 5GHz"}
	local ap = wlan.APCFG()
	local result = true
	local sta_list = {}
	local json = {}

	if operate == "read" or operate == "load" then
		for _, sta in ipairs(ap:assoclist()) do
			sta_list[#sta_list + 1 ] = {
				mac = sta.mac,
				type = map[sta.type],
				encryption = sta.security,
				rxpkts = sta.tx_packets,
				txpkts = sta.rx_packets,
			}
		end
	end

	return {operator = operate, success = result, timeout = false, data = sta_list}
end

-- Process 2G survey HTTP request.
-- @param N/A
-- @return N/A
function wireless_survey_2g(formvalue)
	local ap = wlan.APCFG()
	local ap_list = ap and ap:scanlist(true, false)
	function list_cmp (a, b)
		return a.signal > b.signal
	end
	table.sort(ap_list, list_cmp)
	return {operator = "read", success = true, timeout = false, data = ap_list}
end

-- Process 5G survey HTTP request.
-- @param N/A
-- @return N/A
function wireless_survey_5g(formvalue)
	local ap = wlan.APCFG()
	local ap_list = ap and ap:scanlist(false, true)
	function list_cmp (a, b)
		return a.signal > b.signal
	end
	table.sort(ap_list, list_cmp)
	return {operator = "read", success = true, timeout = false, data = ap_list}
end

-- Process wireless devlist HTTP request.
-- @param N/A
-- @return N/A
function wireless_devlist(formvalue)
    local client   = require("luci.model.client_mgmt")
	local operate = formvalue["operation"] or "read"
	local map = {"Wireless 2GHz", "Wireless 5GHz", "Guest 2GHz", "Guest 5GHz"}
	local ap = wlan.APCFG()
	local result = true
	local dev_list = {}
	
	if operate == "read"  or operate == "load" then
		local pc_list = (client and client.get_client_list()) or {}
		local sta_list = (ap and ap:assoclist()) or {}

		-- If a sta is not in the client list, output it first.
		for _, sta in ipairs(sta_list) do
			local found = false
			for _, pc in ipairs(pc_list) do
				if sta.mac == pc.mac then
					found = true
					pc.wire_type = map[sta.type]
					break
				end
			end
			if not found then
				dev_list[#dev_list + 1] = {
						mac = sta.mac,
						ip = "0.0.0.0",
						name = "UNKNOW",
						type = map[sta.type]
				}
			end
		end

		-- Output all client list.
		for _, pc in ipairs(pc_list) do
			dev_list[#dev_list + 1] = {
					mac = pc.mac,
					ip = pc.ip,
					name = pc.hostname,
					type = pc.wire_type
			}
		end
	end

	return {operator = operate, success = result, timeout = false, data = dev_list}
end

-- Process wireless macfiltering HTTP request.
-- @param N/A
-- @return N/A
function wireless_maclist(formvalue)
	local operate = formvalue["operation"] or "read"
	local uci = require "luci.model.uci"
	local uci_r = uci.cursor()
	local index = formvalue["index"]
	local ap = wlan.APCFG()
	local result = false
	local data = {}
	local others = {}
	
	-- Read one or all mac list
	if operate == "read" or operate == "load" then
		result = ap:maclist_read(index and tonumber(index), data)
	-- Remove a mac entry by index
	elseif operate == "remove" then
		result = index and ap:maclist_remove(index, data)
	-- Modify or insert a mac entry
	elseif operate == "update" or operate == "insert" then

		data = require("luci.json").decode(formvalue["new"])

		result = ap:maclist_check(data)
		
		if result then
			if operate == "insert" then
				result = ap:maclist_insert(data)
			else
				result = index and ap:maclist_update(tonumber(index), data)
			end
		end
	end
	
	local max_rules=uci_r:get_profile("wireless","max_mac_filter")
	others.max_rules=max_rules
	
	result = result or false
	return {operation = operate, success = result, timeout = false, data = data, others=others}
end

function tmp_read_ath(is_guest)
	local wl2g = { enSecurity = "", ssid = "", secMode = "", pskCfg = "", wpaCfg = "", wepCfg = ""}
	local wl5g = { enSecurity = "", ssid = "", secMode = "", pskCfg = "", wpaCfg = "", wepCfg = ""}
	local ret  = { basicCfg = {wl2g, wl5g} }
	
	ap = wlan.APCFG()
	local info = ap:scan_driver()
	
	if is_guest == true then
		if2g = info[CFG_GST_2G][1]
		if5g = info[CFG_GST_5G][1]
	else
		if2g = info[CFG_HST_2G][1]
		if5g = info[CFG_HST_5G][1]
	end
	
	local gst2g_if = info[CFG_GST_2G][1]
	local gst5g_if = info[CFG_GST_5G][1]
	local dev2g = info[CFG_DEV_2G][1]
	local dev5g = info[CFG_DEV_5G][1]

	local guest2g_access = uci_r:get("wireless", gst2g_if, "access") or ""
	local guest5g_access = uci_r:get("wireless", gst5g_if, "access") or ""

	if guest2g_access == "on" and guest5g_access == "on" then
		ret.access = "on"
	else
		ret.access = "off"
	end

	local dev2g_disabled = uci_r:get("wireless", dev2g, "disabled")
	local dev5g_disabled = uci_r:get("wireless", dev5g, "disabled")

	if dev2g_disabled == "on" or dev5g_disabled == "on" then
		ret.hardSwitchEn = "off"
	else
		ret.hardSwitchEn = "on"
	end	

	local wl2g_enable = uci_r:get("wireless", if2g, "enable")
	local wl5g_enable = uci_r:get("wireless", if5g, "enable")
	-- 2.4g
	if wl2g_enable == "on" then
		ret.enableBand = "1"
	else
		ret.enableBand = "0"
	end
	-- 5g
	if wl5g_enable == "on" then
		ret.enableBand = ret.enableBand .. "1"
	else
		ret.enableBand = ret.enableBand .. "0"
	end

	local wl2g_encry = uci_r:get("wireless", if2g, "encryption") or "none"
	local wl5g_encry = uci_r:get("wireless", if5g, "encryption") or "none"
	wl2g.ssid = uci_r:get("wireless", if2g, "ssid")
	wl5g.ssid = uci_r:get("wireless", if5g, "ssid")

	if wl2g_encry == "none" then
		wl2g.enSecurity = "off"
		wl2g.secMode = "none"		
		wl2g.pskCfg = uci_r:get("wireless", if2g, "psk_key")
		wl2g.wepCfg = uci_r:get("wireless", if2g, "wep_key1")
		wl2g.wpaCfg_ip = uci_r:get("wireless", if2g, "server") or "0.0.0.0"
		wl2g.wpaCfg_passwd = uci_r:get("wireless", if2g, "wpa_key") or ""
	else
		wl2g.enSecurity = "on"
		wl2g.secMode = wl2g_encry
		wl2g.pskCfg = uci_r:get("wireless", if2g, "psk_key")
		wl2g.wepCfg = uci_r:get("wireless", if2g, "wep_key1")
		wl2g.wpaCfg_ip = uci_r:get("wireless", if2g, "server") or "0.0.0.0"
		wl2g.wpaCfg_passwd = uci_r:get("wireless", if2g, "wpa_key") or ""
	end

	if wl5g_encry == "none" then
		wl5g.enSecurity = "off"
		wl5g.secMode = "none"
		wl5g.pskCfg = uci_r:get("wireless", if5g, "psk_key")
		wl5g.wepCfg = uci_r:get("wireless", if5g, "wep_key1")
		wl5g.wpaCfg_ip = uci_r:get("wireless", if5g, "server") or "0.0.0.0"
		wl5g.wpaCfg_passwd = uci_r:get("wireless", if5g, "wpa_key") or ""
	else
		wl5g.enSecurity = "on"
		wl5g.secMode = wl5g_encry
		wl5g.pskCfg = uci_r:get("wireless", if5g, "psk_key")
		wl5g.wepCfg = uci_r:get("wireless", if5g, "wep_key1")
		wl5g.wpaCfg_ip = uci_r:get("wireless", if5g, "server") or "0.0.0.0"
		wl5g.wpaCfg_passwd = uci_r:get("wireless", if5g, "wpa_key") or ""
	end
	-- dbg.dumptable(ret)
	return {success = true, data = ret}
end

function set_ath(ifname, lua_form)
	-- dbg.dumptable(lua_form)
	uci_r:set("wireless", ifname, "encryption", lua_form.secMode)
    uci_r:set("wireless", ifname, "ssid", lua_form.ssid)

    if lua_form.secMode == "psk" then
    	uci_r:set("wireless", ifname, "psk_key", lua_form.pskCfg)
    elseif lua_form.secMode == "wep" then
    	uci_r:set("wireless", ifname, "wep_key1", lua_form.wepCfg)
    elseif lua_form.secMode == "wpa" then
    	uci_r:set("wireless", ifname, "server", lua_form.wpaCfg_ip)
    	uci_r:set("wireless", ifname, "wpa_key", lua_form.wpaCfg_passwd)
    end
end

function wireless_tmp_read()
	return tmp_read_ath(false)
end

function wireless_tmp_read_guest()
	return tmp_read_ath(true)
end

function wireless_tmp_set(lua_form)
	local cmd = "/etc/init.d/wireless restart"
	
	ap = wlan.APCFG()
	local info = ap:scan_driver()
	
	if2g  = info[CFG_HST_2G][1]
	if5g  = info[CFG_HST_5G][1]
	dev2g = info[CFG_DEV_2G][1]
	dev5g = info[CFG_DEV_5G][1]
	
	if lua_form.operation == "set" then
		if lua_form.enable2g == "on" then
			uci_r:set("wireless", if2g, "enable", "on")
			uci_r:set("wireless", dev2g, "disabled_all", "off")
		else
			uci_r:set("wireless", if2g, "enable", "off")
			uci_r:set("wireless", dev2g, "disabled_all", "on")
		end

		if lua_form.enable5g == "on" then
			uci_r:set("wireless", if5g, "enable", "on")
			uci_r:set("wireless", dev5g, "disabled_all", "off")
		else
			uci_r:set("wireless", if5g, "enable", "off")
			uci_r:set("wireless", dev5g, "disabled_all", "on")
		end

	elseif lua_form.operation == "set_2g" then
		set_ath(if2g, lua_form)
	elseif lua_form.operation == "set_5g" then
		set_ath(if5g, lua_form)
	else
		dbg("error operation")
	end

	uci_r:commit("wireless")
	wlan.fork(cmd, nil)
	return { success = true }
end

function wireless_tmp_set_guest(lua_form)
	local cmd = "/etc/init.d/wireless restart"
	
	ap = wlan.APCFG()
	local info = ap:scan_driver()
	
	if2g  = info[CFG_GST_2G][1]
	if5g  = info[CFG_GST_5G][1]
	
	if lua_form.operation == "set" then
		if lua_form.enable2g == "on" then
			uci_r:set("wireless", if2g, "enable", "on")
		else
			uci_r:set("wireless", if2g, "enable", "off")
		end

		if lua_form.enable5g == "on" then
			uci_r:set("wireless", if5g, "enable", "on")
		else
			uci_r:set("wireless", if5g, "enable", "off")
		end
		uci_r:set("wireless", if2g, "access", lua_form.access)
		uci_r:set("wireless", if5g, "access", lua_form.access)

	elseif lua_form.operation == "set_2g" then
		set_ath(if2g, lua_form)
	elseif lua_form.operation == "set_5g" then
		set_ath(if5g, lua_form)
	else
		dbg("error operation")
	end

	uci_r:commit("wireless")
	wlan.fork(cmd, nil)
	return { success = true }
end
#!/usr/bin/env lua

local uci = require "luci.model.uci"

local uci_c = uci.cursor()

local function config_get_wan_ifname_i()
	return uci_c:get("network", "wan", "ifname")
end

local function config_get_lan_ifname()
	print(uci_c:get("network", "lan", "ifname"))
end
local function config_get_lan_type()
	print(uci_c:get("network", "lan", "type"))
end
local function config_get_wan_ifname()
	print(uci_c:get("network", "wan", "ifname"))
end
local function config_get_wan_type()
	print(uci_c:get("network", "wan", "type"))
end

local function config_set_network(cfg, sec, opt, val)
	local tmp_sec
	if val == nil then
		return
	end
	local wan_ifname = config_get_wan_ifname_i()
	if sec == "device" and wan_ifname ~= nil then
		uci_c:foreach(cfg, sec,
			function(section)
			if section["name"] == wan_ifname then
				 tmp_sec = section[".name"]
			end			
			end)
	end
    
	if tmp_sec == nil then
		tmp_sec = sec
	end    
	old_val = uci_c:get(cfg, tmp_sec, opt)
	if old_val == nil and val == "" then
		return
	end
	-- Only value changed would strike the commit action.
	if old_val ~= val then 
		uci_c:set(cfg, tmp_sec, opt, val or "")
            -- verify the file of tmp/iptv_state is existed or not
            -- not existed -> DUT start, no need to save flash
            -- existed     -> page operation, need to save flash
            local file,err=io.open("/tmp/iptv_state")
            if file == nil then
            uci_c:commit_without_write_flash(cfg)
	else
            uci_c:commit(cfg)
            end
	end
end

if arg[1] == "set" then
	if arg[2] and arg[3] and arg[4] then
		config_set_network(arg[2], arg[3], arg[4], arg[5])
	end
elseif arg[1] == "get_lan_ifname" then
	return config_get_lan_ifname()
elseif arg[1] == "get_lan_type" then
	return config_get_lan_type()
elseif arg[1] == "get_wan_ifname" then
	return config_get_wan_ifname()
elseif arg[1] == "get_wan_type" then
	return config_get_wan_type()
else
	print(false)
end


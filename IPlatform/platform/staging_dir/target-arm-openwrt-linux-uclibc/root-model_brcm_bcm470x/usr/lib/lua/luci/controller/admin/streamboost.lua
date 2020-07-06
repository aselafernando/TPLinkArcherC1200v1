--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  streamboost.lua
Details :  controller for streamboost webpage
Author  :  Weng Kaiping <wengkaiping@tp-link.net>
Version :  1.0.0
Date    :  3Apr, 2014
]]--

require "luci.sys"
local uci    = require "luci.model.uci"
local util   = require "luci.util"
local http   = require "luci.http"




module("luci.controller.admin.streamboost", package.seeall)

local uci_r = uci.cursor()


function index()
	entry({"admin","streamboost"}, call("appflow_operate")).leaf = true
end

local err_data = {
    success = false,
    timeout = false,
    data = {
        error_code = 0x300001
    }
}

local function write_json(data)
	http.prepare_content("application/json")
	http.write_json(data)
end

local function get_appflow_settings()
	local appflow_data = {
		timeout = false,
		success = true,
		data = {}
	}
	
	local datas = {}
        datas.enable_streamboost  = uci_r:get("appflow", "tccontroller", "enable_streamboost") or "0"
	datas.enable_auto         = uci_r:get("appflow", "tccontroller", "enable_auto") or "0"
	datas.auto_update         = uci_r:get("appflow", "tccontroller", "auto_update") or "0"
	datas.uplimit             = uci_r:get("appflow", "tccontroller", "uplimit") 
	datas.downlimit           = uci_r:get("appflow", "tccontroller", "downlimit") 

	
	appflow_data.data = datas

	write_json(appflow_data)
end

local function set_appflow_settings()
	local uplimit, downlimit, enable, auto_update, auto_bw

        enable      = http.formvalue("enable_streamboost") or "0"
	auto_bw     = http.formvalue("enable_auto") or "0"
	auto_update = http.formvalue("auto_update") or "0"
	uplimit     = http.formvalue("uplimit")
	downlimit   = http.formvalue("downlimit") 

	if enable == "1" and ( uplimit == nil or downlimit == nil) then
		return false
	end
	
	if enable == "" then
		enable = "0"
	end
	if auto_bw == "" then
		auto_bw = "0"
	end
	if auto_update == "" then
		auto_update = "0"
	end
	-- luci.sys.call("echo \"enable %s auto_bw %s auto_update %s\" > /dev/console" % {enable, auto_bw, auto_update})

	-- save settings to config file
	uci_r:set("appflow", "tccontroller", "enable_streamboost", enable)
	uci_r:set("appflow", "tccontroller", "enable_auto", auto_bw)
	uci_r:set("appflow", "tccontroller", "uplimit_auto", auto_bw)
	uci_r:set("appflow", "tccontroller", "downlimit_auto", auto_bw)
	uci_r:set("appflow", "tccontroller", "auto_update", auto_update)

        if uplimit ~= nil and downlimit ~= nil then	
		uci_r:set("appflow", "tccontroller", "uplimit", uplimit)
		uci_r:set("appflow", "tccontroller", "downlimit", downlimit)
	end

	uci_r:commit("appflow")
	get_appflow_settings()
	fork_exec("/sbin/luci-reload appflow >/dev/null 2>&1")
	

end

function fork_exec(command)
	local pid = nixio.fork()
	if pid > 0 then
		return
	elseif pid == 0 then
		-- change to root dir
		nixio.chdir("/")

		-- patch stdin, out, err to /dev/null
		local null = nixio.open("/dev/null", "w+")
		if null then
			nixio.dup(null, nixio.stderr)
			nixio.dup(null, nixio.stdout)
			nixio.dup(null, nixio.stdin)
			if null:fileno() > 2 then
				null:close()
			end
		end

		-- replace with target command
		nixio.exec("/bin/sh", "-c", command)
	end
end


function appflow_operate()
    local operate = http.formvalue("operation")
    local ret

	if operate == "read" then
		get_appflow_settings()
	elseif operate == "write" then
		set_appflow_settings()		
	else
		write_json(err_data)
	end
   return
end



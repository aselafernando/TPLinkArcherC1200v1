--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  openvpn.lua
Details :  
Author  :  Zhu Xianfeng<zhuxianfeng@tp-link.net>
Version :  1.0.0
Date    :  02Dec14
History :  arg 1.0.0, 02Dec14, Zhu Xianfeng, Create the file.
]]--

module("luci.controller.admin.openvpn", package.seeall)

local sys      = require "luci.sys"
local http     = require "luci.http"
local ltn12    = require "luci.ltn12"
local muci     = require "luci.model.uci"
local vpn      = require "luci.model.vpn"
local ctypes   = require "luci.model.checktypes"
local debug    = require "luci.tools.debug"
local ctl      = require "luci.model.controller"
local fs 	   = require "luci.fs"
local uci      = muci.cursor()


--判断文件是否存在
function file_exists(path)  
	local file = io.open(path, "rb")  
	if file then 
		file:close()
	end

	return file ~= nil
end

-- Generate new openvpn certificate
local function openvpn_after_cert_rebuild()
	--local cry = require "luci.model.crypto"
	
	os.execute("cp -f /tmp/etc/openvpn/* /etc/openvpn/ >/dev/null 2>&1")
	os.execute("cd /etc/openvpn/;ls | grep -v 'crt.sed' | xargs tar -cf /tmp/openvpn-cert.tar >/dev/null 2>&1")
	--cry.enc_file("/tmp/openvpn-cert.tar", "/tmp/openvpn-cert.cry", "0123456789abcdef    ")
	os.execute("nvrammanager -w /tmp/openvpn-cert.tar -p certificate >/dev/null 2>&1")
	os.execute("rm -f /tmp/openvpn-cert.tar >/dev/null 2>&1")
end

-- Get OpenVPN UCI config
-- @param N/A
-- @return table
local function openvpn_cfg_get()
    local sectype = uci:get("openvpn", "server")

    if sectype ~= "openvpn" then
        return false, "invalid uci config"
    end
    local server = uci:get("openvpn", "server", "server")
    local serverip = nil
    local mask = nil
    serverip, mask = server:match("(%d+%.%d+%.%d+%.%d+)%s(%d+%.%d+%.%d+%.%d+)")
    if not serverip or not mask then
        return false, "invalid uci config"
    end

    local data = {
        enabled = uci:get("openvpn", "server", "enabled"), 
        proto   = uci:get("openvpn", "server", "proto"),
        port    = uci:get("openvpn", "server", "port"),
        access  = uci:get("openvpn", "server", "access"),
        serverip= serverip,
        mask    = mask,
        cert_exist = file_exists("/etc/openvpn/ca.crt")
    }

    return data
end

-- Set OpenVPN UCI config
-- @param N/A
-- @return table
local function openvpn_cfg_set(form)
    local sectype = uci:get("openvpn", "server")
    local oldport = uci:get("openvpn", "server", "port")
    local oldproto = uci:get("openvpn", "server", "proto")
    local oldaccess = uci:get("openvpn", "server", "access")

    if sectype ~= "openvpn" then
        return false, "invalid uci config"
    end

    local port = tonumber(form.port)

    if form.enabled ~= "on" and form.enabled ~= "off" then
        return false, "invalid enabled"
    end

    if form.proto ~= "tcp" and form.proto ~= "udp" then
        return false, "invalid proto"
    end

    if port < 1024 or port > 65535 then
        return false, "invalid port " .. tostring(form.port)
    end
    if not ctypes.check_ipv4(form.serverip) or not ctypes.check_netmask(form.mask, false) then
        return false, "invalid vpn subnet"
    end

    if form.access ~= "home" and form.access ~= "internet" then
        return false, "invalid access type"
    end

    local data   = {
        enabled = form.enabled,
        proto   = form.proto,
        port    = port,
        server  = form.serverip .. " " .. form.mask,
        access  = form.access,
    }
    uci:section("openvpn", "openvpn", "server", data)
    uci:commit("openvpn")

	-- restart openvpn service
    sys.fork_call("/etc/init.d/openvpn stop")
    sys.fork_call("fw openvpn_block " .. oldproto .. " " .. oldport .. " " .. oldaccess)
    nixio.nanosleep(1, 0)
    if form.enabled == "on" then
        sys.fork_call("/etc/init.d/openvpn start")
        sys.fork_call("fw openvpn_access " .. form.proto .. " " .. form.port .. " " .. form.access)
    elseif form.enabled == "off" then
        -- disconnect all openvpn clients
        vpn.vpn_disconn_all("openvpn")
    end

    return openvpn_cfg_get()
end


-- Generate openvpn certificate
-- @param N/A
-- @return table
local function openvpn_generate_cert()
	local cmd = [[
		for pid in `pidof build-ovpn-crt`; do 
			kill $pid
		done
		for pid in `pidof build-dh`; do 
			kill $pid
		done
		for pid in `pidof openssl`; do 
			kill $pid
		done
	]]

	os.execute(cmd)        
	debug.printf("============generate certificate...")
	
	os.execute("rm -f /tmp/cert_generate.status >/dev/null 2>&1")
	
	update_cert_status("generating")
	
	sys.fork_exec("/usr/sbin/build-ovpn-crt rebuild")
		
	return true

end

function update_cert_status(cert_status)
    local file = io.open("/tmp/generate_status.lua", "w")
    local check_cmd = "check_status = {cert_status=\"%s\"}\n"
    file:write(string.format(check_cmd, cert_status))
    file:close()
end


-- Get the status of generating certificate
-- @param N/A
-- @return table
local function openvpn_check_cert()
	
	if nixio.fs.access("/tmp/cert_generate.status") then
		
		local fp = io.open("/tmp/cert_generate.status", 'r')
		local line = fp:read()
		if not line then 
			return false
		else
			if line == "success" then
				openvpn_after_cert_rebuild()
				update_cert_status("success")
				debug.printf("============generate certificate  finish !!!")  
		
		local enable = uci:get("openvpn", "server", "enabled")
		if enable == "on" then
			sys.fork_exec("/etc/init.d/openvpn restart")
		end
			end
		end
	end
		
	if nixio.fs.access("/tmp/generate_status.lua") then
		dofile("/tmp/generate_status.lua")
		ret = check_status
	else
		return true
    end
		
	return ret

end

-- Export OpenVPN configuration for client devices
-- @param N/A
-- @return table
local function openvpn_export()

    -- re-generate client.conf
    sys.fork_call("/usr/sbin/build-ovpn-crt")

    local reader = sys.ltn12_popen("cat /etc/openvpn/client.conf")
    --local value  = string.format('attachment; filename="OpenVPN-Config-%s.ovpn"', sys.hostname())
    local value  = string.format('attachment; filename="OpenVPN-Config.ovpn"')

    http.header("Content-Disposition", value)
    http.prepare_content("text/plain")
    ltn12.pump.all(reader, http.write)

    return {}
end

--load openvpn certificate
function load_openvpn_cert()
	os.execute("nvrammanager -r /tmp/openvpn-cert.tar -p certificate >/dev/null 2>&1")
	local filesize = fs.stat("/tmp/openvpn-cert.tar").size
	if filesize == 0 then
		os.execute("rm -f /tmp/openvpn-cert.tar >/dev/null 2>&1")
		return true
	end
	
	--local cry = require "luci.model.crypto"
	--cry.dec_file("/tmp/openvpn-cert.cry", "/tmp/openvpn-cert.tar", "0123456789abcdef    ")
	os.execute("tar -xf /tmp/openvpn-cert.tar -C /etc/openvpn >/dev/null 2>&1")
	os.execute("rm -f /tmp/openvpn-cert.tar >/dev/null 2>&1")
end

-- 1. Get/Set
-- URL: /admin/openvpn?form=config
-- Parameters:
--     enabled: on/off
--     proto: udp/tcp
--     port: 1194 or in (1024, 65535)
--     access: home/internet
-- 2. Export 
-- URL: /admin/openvpn?form=export
-- Parameters: None
local dispatch_tbl = {
    config = {
        ["read"]  = {cb = openvpn_cfg_get},
        ["write"] = {cb = openvpn_cfg_set},
    },
	openvpn_cert = {
		["generate"] = {cb = openvpn_generate_cert},
		["check"] = {cb = openvpn_check_cert},
    },
    export = {
        [".super"] = {cb = openvpn_export},
    },
}

local function openvpn_dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end

function openvpn_index()
    return ctl._index(openvpn_dispatch)
end

function index()
    entry({"admin", "openvpn"}, call("openvpn_index")).leaf = true
end

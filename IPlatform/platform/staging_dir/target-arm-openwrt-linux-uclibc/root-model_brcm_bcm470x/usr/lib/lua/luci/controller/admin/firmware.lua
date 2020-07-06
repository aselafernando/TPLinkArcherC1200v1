--[[Copyright(c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  firmware.lua
Details :  firmware http response operation
Author  :  He Ye  <heye@tp-link.net>
Version :  1.0.0
Date    :  27Mar, 2014
]]--

module("luci.controller.admin.firmware", package.seeall)

local uci       = require "luci.model.uci"
local http      = require "luci.http"
local debug     = require "luci.tools.debug"
local sys       = require "luci.sys"
local nixio     = require "nixio"
local ctl       = require "luci.model.controller"
local fs        = require "luci.fs"
local util   	= require "luci.util"
local configtool = require "luci.sys.config"
local cloud_account = require("luci.controller.admin.cloud_account")

local REBOOT_TIME = uci.cursor():get_profile("global", "reboot_time") or 75
local IMAGE_TMP_PATH = "/tmp/firmware.bin"
local CONFIG_TMP = "/tmp/config.bin"
local BACKUP_TMP = "/tmp/backup"
local uploadFileSize = 0
local UPGRADE_TIME = 75

local function get_partition_size(partition_name)
    local PARTITION_FILE = "/tmp/partition.txt"
    local partition_size = 0
    os.execute("nvrammanager -s > "..PARTITION_FILE)
    local fp = io.open(PARTITION_FILE, 'r')
    if nil ~= fp then
        repeat
            local line = fp:read("*line")
            if nil ~= line then
                local i, j
                i, j = string.find(line, partition_name)
                if nil ~= i and nil ~= j then
                    local pat = "(.-),()"
                    local size_pat = "size%s*=%s*(.-)%s*Bytes"
                    local part, pos, value
                    for part, pos in string.gfind(line, pat) do
                        i, j = string.find(part, "size")
                        if nil ~= i and nil ~= j then
                            _,_,value = string.find(part, size_pat)
                            partition_size = tonumber(value)
                        end
                    end
                end
            end
        until nil == line or partition_size > 0
    end
    os.execute("rm "..PARTITION_FILE)
    return partition_size
end

function fork_reboot()
    local system = require("luci.controller.admin.system")
    system.reboot()
    --sys.fork_exec("sleep 2;reboot")
end

function file_flash(totaltime, ops)
    local file = io.open("/tmp/firmware_status.lua", "w")
    local check_cmd   = "check_status = {totaltime=%d, ops=\"%s\"}\n"
    file:write(string.format(check_cmd, totaltime, ops))
    file:close()
end

function update_fwuppercent(percent, ops)
    local file = io.open("/tmp/firmware_status.lua", "w")
    local check_cmd   = "check_status = {percent=%d, ops=\"%s\"}\n"
    file:write(string.format(check_cmd, percent, ops))
    file:close()
end

function update_rebootflag(flag, ops)
    local file = io.open("/tmp/reboot_flag.lua", "w")
    local check_cmd   = "check_status = {reboot=%d, ops=\"%s\"}\n"
    file:write(string.format(check_cmd, flag, ops))
    file:close()
end

function restore_error(error_code, ops)
    local file = io.open("/tmp/firmware_status.lua", "w")
    local check_cmd = "check_status = {error_code=\"%s\", ops=\"%s\"}\n"
    file:write(string.format(check_cmd, error_code, ops))
    file:close()
end

function config_read( ... )
    local ret = {totaltime = REBOOT_TIME}
    return ret
end

function config_check( ... )
    local ret
    if nixio.fs.access("/tmp/firmware_status.lua") then
        dofile("/tmp/firmware_status.lua")
        ret = check_status
        if ret.ops == "restore" then
            return false, ret.error_code
        end
    else
        return true
    end

    return ret
end

function config_backup( ... )
    local uci_r = uci.cursor()
    os.execute("mkdir /tmp/backupfolder >/dev/null 2>&1")	
    -- 备份extern分区, 如有特殊情况的分区，再特殊处理	
    local extern_partitions = uci_r:get_profile("backup_restore", "extern_partition") or nil
    if extern_partitions ~= nil then
        extern_partitions = util.split(extern_partitions, " ")			
        for i, v in ipairs(extern_partitions) do
            if v ~= nil then
        	    luci.sys.exec("nvrammanager -r /tmp/backupfolder/ori-backup-" .. v .. ".bin -p " .. v .. " >/dev/null 2>&1")
            end
        end
    end
    --打包					
    luci.sys.exec("nvrammanager -r /tmp/backupfolder/ori-backup-user-config.bin -p user-config >/dev/null 2>&1")
    os.execute("tar -cf "..BACKUP_TMP.." -C /tmp/backupfolder . >/dev/null 2>&1")					
    luci.sys.exec("rm -rf /tmp/backupfolder >/dev/null 2>&1")

    local cry = require "luci.model.crypto"
    local cryfunc1 = cry.enc_file(BACKUP_TMP, configtool.getsysinfo("product_name"))
    cry.dump_to_file(cryfunc1,"/tmp/backup.cry")

    local reader = sys.ltn12_popen("cat /tmp/backup.cry")
    luci.http.header('Content-Disposition', 'attachment; filename="backup-%s-%s.bin"' % {
        configtool.getsysinfo("product_name"), os.date("%Y-%m-%d")})
    luci.http.prepare_content("application/x-bin")
    luci.ltn12.pump.all(reader, luci.http.write)
    luci.sys.exec("rm -f " ..BACKUP_TMP.. "; rm -f /tmp/backup.cry")
    return true
end

function check_country()
	local configtool = require "luci.sys.config"
	--the @wifi-device[0] can not be delivered to uci_r:get() 
	local region = luci.sys.exec("uci get wireless.@wifi-device[0].country")
	--debug.print(region)
	
	configtool.xmlToFile("/tmp/restore/ori-backup-user-config.bin", "/tmp/")
	local tmpcountry = luci.sys.exec("uci -c /tmp/config/ get wireless.@wifi-device[0].country")
	--debug.print(tmpcountry)
	if region == tmpcountry then
		luci.sys.call("rm -rf /tmp/config/")
		return true
	end
	luci.sys.call("rm -rf /tmp/config/")
	return false
end

function config_restore( ... )
    local uci_r = uci.cursor()
    local cry = require "luci.model.crypto"
    local fs  = require "luci.fs"
    local backup_max_size = get_partition_size("user%-config")
    local cryfunc1
    local backup_file

    local extern_partitions = uci_r:get_profile("backup_restore", "extern_partition") or nil
    if extern_partitions ~= nil then
        extern_partitions = util.split(extern_partitions, " ")
        for i, v in ipairs(extern_partitions) do
            if v ~= nil then
                backup_max_size = backup_max_size + get_partition_size(v);
            end
        end	
    end

    --check bin file size
    if uploadFileSize < backup_max_size + 1024 then
        cryfunc1 = cry.dec_file(CONFIG_TMP, configtool.getsysinfo("product_name"))
        cry.dump_to_file(cryfunc1, BACKUP_TMP)
        os.execute("mkdir /tmp/restore >/dev/null 2>&1")
        --解压
        ret = os.execute("tar -xf "..BACKUP_TMP.." -C /tmp/restore >/dev/null 2>&1")

		if ret ~= 0 then
			restore_error("err_failed", "restore")
			return true
		end

        if extern_partitions ~= nil then			
            for i, v in ipairs(extern_partitions) do
                if v ~= nil then
        		    local filesize = fs.stat("/tmp/restore/ori-backup-" .. v .. ".bin").size
        		    local backup_max_size = get_partition_size(v)
        		    if filesize > 0 and filesize <= backup_max_size then
        		        luci.sys.exec("nvrammanager -e -p " .. v .. " >/dev/null 2>&1")
        		        luci.sys.exec("nvrammanager -w /tmp/restore/ori-backup-" .. v .. ".bin -p ".. v .. " >/dev/null 2>&1")
        		    else
        		        luci.sys.exec("nvrammanager -e -p " .. v .. " >/dev/null 2>&1")
        		    end
                end
            end
        end
		
        backup_file = fs.stat("/tmp/restore/ori-backup-user-config.bin")
        backup_max_size = get_partition_size("user%-config")
        if backup_file.size > 0 and backup_file.size <= backup_max_size and check_country() then
            luci.sys.exec("nvrammanager -e -p user-config >/dev/null 2>&1")
            luci.sys.exec("nvrammanager -w /tmp/restore/ori-backup-user-config.bin -p user-config >/dev/null 2>&1")
            configtool.xmlToFile("/tmp/restore/ori-backup-user-config.bin", "/tmp")
            luci.sys.exec("rm -rf /tmp/restore; rm -f " ..BACKUP_TMP)

            luci.sys.call("[ -f /sbin/board_restore ] && board_restore")

            debug.printf("reboot...")
            fork_reboot()
            --return true
        else
            debug.printf("Decry file failed")
            luci.sys.exec("rm -rf /tmp/restore; rm -f " ..BACKUP_TMP)
            restore_error("err_failed", "restore")
            --return false
        end
		
    else
        debug.printf("restoring file len error")
        restore_error("err_failed", "restore")
        --return false
    end
	-- return false will return json data {success:"false"} to webpages ,
	-- and for ie11 , there has a bug that the restore_error() will not be execute 
	return ture
end

function config_reboot( ... )
    debug.printf("reboot...")
    fork_reboot()
    return true
end

function config_factory( ... )
    local uci_r = uci.cursor()
    debug.printf("reset to factory config")

    file_flash(REBOOT_TIME, "factory")

    configtool.resetconfig()

    -- 擦除extern分区, 如有特殊情况的分区，再特殊处理
    local extern_partitions = uci_r:get_profile("backup_restore", "extern_partition") or nil
    if extern_partitions ~= nil then
    	extern_partitions = util.split(extern_partitions, " ")
    	for i, v in ipairs(extern_partitions) do
    		if v ~= nil then
    			os.execute("nvrammanager -e -p " .. v .. " >/dev/null 2>&1")
    		end
    	end
    end		
	
    luci.sys.call("/etc/init.d/logd stop ; logreset")
    luci.sys.call("[ -f /sbin/board_factory ] && board_factory")

    debug.printf("reboot...")
    fork_reboot()
    return true
end

function upgrade_read( ... )
	local SID = {
		["00000000"] = "UN",
		["55530000"] = "US",
		["45550000"] = "EU",
		["43410000"] = "CA",
	}
		
	local region = " ("..(SID[configtool.getsysinfo("special_id")] or "UN")..")"
    local ret = {
        model = configtool.getsysinfo("product_name"), 
        hardware_version = configtool.getsysinfo("HARDVERSION"), 
        firmware_version = configtool.getsysinfo("SOFTVERSION")..region,
        --firmware_version = configtool.getsysinfo("SOFTVERSION"), 
        is_default = configtool.isdftconfig(),
        totaltime = REBOOT_TIME
    }
    return ret
end

function set_download_inf()
	local uci_r = require("luci.model.uci").cursor()
	local dl_percent = uci_r:get("cloud_status", "client_info", "fw_download_progress") 
	if tonumber(dl_percent) >= 100 then
		uci_r:delete("cloud_config", "new_firmware")
		uci_r:delete("cloud_config", "upgrade_info")
		uci_r:set("cloud_config", "new_firmware", "cloud_push")
		uci_r:set("cloud_config", "upgrade_info", "cloud_reply")
		uci_r:set("wportal", "upgrade", "enable", "yes")
		uci_r:set("wportal", "upgrade", "time", "0")
		uci_r:set("cloud_config", "info", "show_flag", "0")
		uci_r:commit("cloud_config")
		--sys.fork_exec(". /lib/wportal/wportal.sh && wportalctrl_clear_all")
	end
end

function get_upgrade_detail(processcmd)	
	local ret
	local ubus = require "ubus"
	local _ubus = ubus.connect()
	local UBUS_OBJECT = "nvram_ubus"
	local fwup_percent  = _ubus:call(UBUS_OBJECT, "getFwupPercent", {})
	local percent

	if fwup_percent ~= nil then
		percent = fwup_percent.percent
		update_fwuppercent(percent, "flash")
			
		if nixio.fs.access("/tmp/firmware_status.lua") then
			dofile("/tmp/firmware_status.lua")
			ret = check_status
		else
			return true, percent
		end
		
		if nixio.fs.access("/tmp/reboot_flag.lua") then
			dofile("/tmp/reboot_flag.lua")
			flag = check_status.reboot
		end
		
		if tonumber(flag) == 1 then
			return ret, percent
		end
		
		if percent >= 100 and processcmd then
			update_rebootflag(1, "reboot")
			debug.printf("upgrade true")
			--clean cloud upgrade flag after upgrade firmware sucessfully.
			local uci_r = require("luci.model.uci").cursor()
			local dl_percent = uci_r:get("cloud_status", "client_info", "fw_download_progress") 
--			if tonumber(dl_percent) >= 100 then
				uci_r:delete("cloud_config", "new_firmware")
				uci_r:delete("cloud_config", "upgrade_info")
				uci_r:set("cloud_config", "new_firmware", "cloud_push")
				uci_r:set("cloud_config", "upgrade_info", "cloud_reply")
				uci_r:set("wportal", "upgrade", "enable", "yes")
				uci_r:set("wportal", "upgrade", "time", "0")
                uci_r:set("cloud_config", "info", "show_flag", "0")
				uci_r:commit("cloud_config")
				--sys.fork_exec(". /lib/wportal/wportal.sh && wportalctrl_clear_all")
--            end

			debug.printf("reboot...")
			--use os.exexcute to reboot to avoid require module file
			os.execute("sleep 2 && reboot")
			--fork_reboot()
			--sys.fork_exec("sleep 1;reboot")
		end

		return ret, percent
	else
		debug.printf("upgrade false")
		sys.fork_exec("ledcli STATUS_ON")
		return false, "err_failed"
	end
end

function upgrade_fwup_check( ... )
	return get_upgrade_detail(true)
end

function upgrade_firmware( ... )
    debug.printf("upgrade firmware...")

    if nixio.fs.access(IMAGE_TMP_PATH) then
        sys.fork_exec("ledcli STATUS_SAN")
        sys.fork_exec("nvrammanager -u  " .. IMAGE_TMP_PATH)
    else
        sys.fork_exec("ledcli STATUS_ON")
        debug.printf("false")
    end

    return true
end

function utfstrlen(str)
	local len = #str;
	local left = len;
	local cnt = 0;
	local arr={0,0xc0,0xe0,0xf0,0xf8,0xfc};
	while left ~= 0 do
		local tmp=string.byte(str,-left);
		local i=#arr;
		while arr[i] do
			if tmp>=arr[i] then
				left=left-i;
				break;
			end
			i=i-1;
		end
		cnt=cnt+1;
	end
	return cnt;
end

function GetShortName(sName,nMaxCount,nShowCount)
	if sName == nil or nMaxCount == nil then
		return
	end
	
	local sStr = sName
	local tCode = {}
	local tName = {}
	local nLenInByte = #sStr
	local nWidth = 0
	
	if nShowCount == nil then
		nShowCount = nMaxCount - 3
	end
	
	for i=1,nLenInByte do
		local curByte = string.byte(sStr, i)
		local byteCount = 0;
		
		if curByte>0 and curByte<=127 then
			byteCount = 1
		elseif curByte>=192 and curByte<=223 then
			byteCount = 2
		elseif curByte>=224 and curByte<=239 then
			byteCount = 3
		elseif curByte>=240 and curByte<=247 then
			byteCount = 4
		elseif curByte>=248 and curByte<=251 then
			byteCount = 5
		elseif curByte>=252 and curByte<=253 then
			byteCount = 6
		end
		
		local char = nil
		
		if byteCount > 0 then
			char = string.sub(sStr, i, i+byteCount-1)
			i = i + byteCount -1
		end
		
		if byteCount >= 1 then
			nWidth = nWidth + 1
			table.insert(tName,char)
			table.insert(tCode,1)
		end
	end
	
	if nWidth >= nMaxCount then
		local _sN = ""
		local _len = 3
		
		for i=1,#tName do
			_sN = _sN .. tName[i]
			if _len > nShowCount then
				break
			end
			_len = _len + tCode[i]
		end
		sName = _sN .. "..."
	end
	return sName
end

function tmp_get_firmware_info()
	local data = {}
    local uci_r = uci.cursor()
	local configtool = require "luci.sys.config"
	data.name = configtool.getsysinfo("product_name") or ""
	data.version = uci_r:get("cloud_config", "upgrade_info", "version") or ""
	if data.version == "" then
		data.version = configtool.getsysinfo("SOFTVERSION") or ""
	end
	local releaseNote = uci_r:get("cloud_config", "upgrade_info", "release_log") or ""
	
	releaseNote = GetShortName(releaseNote,300)
	
	data.releaseNote = nixio.bin.b64encode(releaseNote)
	data.isLatest = uci_r:get("cloud_config", "new_firmware", "fw_new_notify") or "0"
	return data
end

--- call ubus operation 
-- @param  	method
-- @param  	args
-- @param  	path
-- @return 			ubus call return value
function invoke(method, args, path)
    if type(args) ~= "table" then
        args = {args}
    end

	local ubus = require "ubus"
	local _ubus = ubus.connect()
	
    path = path or "cloudclient"

    return _ubus:call(path, method, args)
end


	local dbg      = require "luci.tools.debug"

local function detect_download_status(processcmd)
	local cloud_account = require "luci.controller.admin.cloud_account"
	local data1
	local data2 = {}
	data1, data2 = cloud_account.get_download_detail(processcmd)
	if not data1 then
		return 0, false
	end
	return data1.percent, true
end

local function detect_upgrade_status(processcmd)
	local success
	local percent
	success, percent = get_upgrade_detail(processcmd)
	return percent, success
--[[
	local ret
	local ubus = require "ubus"
	local _ubus = ubus.connect()
	local UBUS_OBJECT = "nvram_ubus"
	local fwup_percent  = _ubus:call(UBUS_OBJECT, "getFwupPercent", {})
	local percent

	if fwup_percent ~= nil then
		percent = fwup_percent.percent
		update_fwuppercent(percent, "flash")

		if percent >= 100 and processcmd then
			debug.printf("upgrade true")
			debug.printf("reboot...")
			-- fork_reboot()
			sys.fork_exec("sleep 3;reboot")
		end

		return percent, true
	else
		debug.printf("upgrade false")
		sys.fork_exec("ledcli STATUS_ON")
		return 0, false
	end
]]--
end

function fw_check_loop()
    local uci_r = require("luci.model.uci").cursor()
	local url = uci_r:get("cloud_config", "upgrade_info", "download_url")
	local filename = string.match(url, ".+/([^/]*%.%w+)$")
	local null = nixio.open("/dev/null", "w+")
	
	if null then
		nixio.dup(null, nixio.stderr)
		nixio.dup(null, nixio.stdout)
		nixio.dup(null, nixio.stdin)
		if null:fileno() > 2 then
			null:close()
		end
	end

	local percent = "0"
	local success = true
	sys.call("sleep 3")
	--download 轮询，以及时激活升级。
	while true do
		percent, success = detect_download_status(false)
		if success == false then 
			return
		end
		if tonumber(percent) >= 100 then
			sys.call("sleep 3")
			break
		end
		sys.call("sleep 3")
	end
	
	sys.call("sleep 3")
	set_download_inf()
	filename = "/tmp/" .. filename
	return sys.call("/sbin/sysupgrade " .. filename)
end

--[[由于手机端的实现要求只要点击过upgrade按钮，之后就算手机断开也要upgrade成功，因此修改upgrade逻辑：
    在选择了upgrade之后，先执行下载，然后fork一个进程，3秒轮询，调用正常的下载命令。
	    ps：正常的下载命令在下载到达100%以后才会主动调用升级进程。
	在获取到下载程度达到100%以后，进行3秒轮询，调用正常的upgrade命令。
	    ps：正常的升级固件命令在达到100%以后才会主动调用reboot。
]]--

function fw_upgrade()
    local uci_r = uci.cursor()
	local dl_url = uci_r:get("cloud_config", "upgrade_info", "download_url")	
	local percent = uci_r:get("cloud_status", "client_info", "fw_download_progress") or "0"
	local dlStatus  = uci_r:get("cloud_status", "client_info", "fw_download_status") or "0"
	
    if dl_url == nil then
        return false, "illegel download url"
    end
	
	if tonumber(percent) >= 1 and ( dlStatus == "1" or dlStatus == "2" or dlStatus == "3" ) then
		--有其他程序正在进行下载升级的相关操作，不再进行。
		return false
	else
		--发送下载命令
		local args = {download_url = dl_url}
		invoke("download_fw", args)
	end
	--轮询
	local pid = nixio.fork()
	if pid == 0 then
		fw_check_loop()
	end
 	return true, percent
end


function tmp_upgrade_firmware()
	local cloud_account = require "luci.controller.admin.cloud_account"
	local internet_access = cloud_account.check_internet()
	if internet_access == false then 
		return false
	end
	return fw_upgrade()
end

function tmp_get_upgrade_info()
    local uci_r = require("luci.model.uci").cursor()
    local percent = uci_r:get("cloud_status", "client_info", "fw_download_progress") 
	local status = "idle"
	local process = "0"
	local success = true
	local data = {}

	--local actStatus = uci_r:get("cloud_status", "download_fw", "action_status")
	local actStatus = uci_r:get("cloud_status", "download_fw", "action_status")
	local dlStatus  = uci_r:get("cloud_status", "client_info", "fw_download_status") or "-1"
	local connect_status = uci_r:get("cloud_status", "client_info", "connect_status") or "1"
	process = detect_download_status(false)
	--dlStatus 0:fail 1:on going 2:successful 3:waiting
	if dlStatus == "0" then
		if actStatus == "4" then
			status = "fail"
			process = "0"
		else
			if actStatus == "3" then 
				status = "preparing"
				process = "0"
			else
				status = "idle"
				process = "0"
			end
		end
	else
		if dlStatus == "1" then
			status = "downloading"
			process = detect_download_status(false)
		else
			if dlStatus == "2" then
				status = "downloading"
				process = "100"
			end
		end
	end
	if dlStatus == "3" then
		status = "preparing"
		process = "0"
	end
	data.status = status
	data.process = process
	data.upgradeTime = UPGRADE_TIME
	data.rebootTime = REBOOT_TIME
	return data
end

local dispatch_tbl = {
    config = {
    	["read"]	= { cb = config_read },
    	["check"]	= { cb = config_check },
    	["backup"]	= { cb = config_backup },
    	["restore"]	= { cb = config_restore, own_response = true },
        ["reboot"]  = { cb = config_reboot },
        ["factory"] = { cb = config_factory }        
    },

    upgrade = {
    	["read"]		= { cb = upgrade_read },
    	["fwup_check"]	= { cb = upgrade_fwup_check },
    	["firmware"]	= { cb = upgrade_firmware, own_response = true }
	},
	
	tmp_cmd = {
		["get_firmware_info"] = { cb = tmp_get_firmware_info },
		["upgrade_firmware"] = { cb = tmp_upgrade_firmware},
		["get_upgrade_info"] = { cb = tmp_get_upgrade_info}
	}
}

function dispatch(http_form)
    local function hook_cb(success, action)
        if success and action.cmd then
            sys.fork_exec(action.cmd)
        end
        return true
    end
    return ctl.dispatch(dispatch_tbl, http_form, {post_hook = hook_cb})
end

function _index()
    --according to nvrammanager sysUpfirmware.h, set maximum file size
    local maximumFileSize = 0x2000000 + 0x1000 + 0x10 + 0x04 
    local rejectOneTime = 0
    local fp
    local firmwareFileName
    uploadFileSize = 0

    luci.http.setfilehandler(
        function(meta, chunk, eof)
            if not fp then
                file_flash(REBOOT_TIME, "upload") 
                if meta and meta.name == "image" then
                    firmwareFileName = IMAGE_TMP_PATH
                else
                    firmwareFileName = CONFIG_TMP
                end
                fp = io.open(firmwareFileName, "w")
                uploadFileSize = 0
            end
            if chunk then
                uploadFileSize = uploadFileSize + #chunk
                if uploadFileSize <= maximumFileSize then
                    fp:write(chunk)
                else
                    if 0 == rejectOneTime then
                        --[[
                            echo fail to tmp file only once,
                            if input file size is too large, 
                            it may be block by other module,
                            so add fail flag and 
                            remove upload file in this place.
                        ]]--
                        fp:close()
                        os.execute("rm -f "..firmwareFileName)
                        fp = io.open(firmwareFileName, "w")
                        rejectOneTime = 1
                    end
                end
            end
            if eof then
                fp:close()
            end
        end
    )
    return ctl._index(dispatch)
end

function index()
    entry({"admin", "firmware"}, call("_index")).leaf = true
end

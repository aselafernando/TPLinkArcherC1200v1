--[[
Copyright(c) 2008-2015 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  	cloud-account.lua
Details :  	controller for cloud accout webpage
Author  :  	Chen Yuqi <chenyuqi@tp-link.net>
Version :  	1.0.0
Date    :  	12feb, 2015
History :  	01, 12Feb15, create file.			
			02, 26Feb15, 
			03, 09Feb15, 
]]--

module("luci.controller.admin.cloud_account", package.seeall)

local cloud_account = require("luci.model.cloud_account")
local asycrypto = require("luci.model.asycrypto").Crypto("rsa")
local ctl = require "luci.model.controller"
local nixio = require "nixio"
local fs = require "nixio.fs"
local sys = require "luci.sys"
local util = require "luci.util"
--local pwdrec = require "luci.model.passwd_recovery"
local dbg = require "luci.tools.debug"
local _lock
local ATTEMPTS_LOCKFILE = "/var/run/luci-attempts.lock"
local ATTEMPTS_FILE = "/tmp/luci-attempts"
local ATTEMPTS_INTERVAL = 2 * 60 * 60
local ATTEMPTS_MAX = 10


--- return the keys according the request form.
-- @param  http_form    form contains the needed variable
-- @return 			
local function read_keys(http_form)
    local asycrypto = require("luci.model.asycrypto").Crypto("rsa")
    local pubkey = asycrypto.read_pubkey()
    local keys   = { pubkey.n, pubkey.e } 
	
	local form = http_form["form"]
	local data = {}
	
	if form == "modify_cloud_password" then
		data = {
			username = "",		
			old_password = keys,
			new_password = keys,
			confirm = keys
		}
	elseif form == "reset_cloud_password" or
		   form == "cloud_reset_pwd_login" or form == "cloud_reset_pwd_login_cloud"
	then
		data = {
			username = "",		
			password = keys,
			confirm = keys,
			vercode = keys
		}
	elseif form == "user_register" then
		data = {
			username = "",		
			password = keys,
			confirm = keys,			
		}	
	else
		data = {
			username = cloud_account.CloudAccount:get_login_username(),
			password = keys
		}	
	end
	
    return data
end

local function lock(w)
    _lock = nixio.open(ATTEMPTS_LOCKFILE, "w", 600)
    _lock:flock(w and "ex" or "sh")
end

local function unlock()
    _lock:close()
    _lock = nil
end

local function read_attempts()
    if not fs.access(ATTEMPTS_FILE, "r") then
        return {}
    end

    lock()
    local blob = fs.readfile(ATTEMPTS_FILE)
    unlock()

    local func = loadstring(blob)
    setfenv(func, {})
    local attempts = func()
    assert(type(attempts) == "table")
    return attempts
end

local function write_attempts(attempts)
    lock(true)
    local f = nixio.open(ATTEMPTS_FILE, "w", 600)
    f:writeall(util.get_bytecode(attempts))
    f:close()
    unlock()
end

local function reap_attempts(attempts)
    for k, v in pairs(attempts) do
        if v.ltime + ATTEMPTS_INTERVAL < sys.uptime() then
            attempts[k] = nil
        end
    end
end

--- check whether the binded cloud acclout logins
-- @param   N/A
-- @return 			true/false
function check_login()
	local account = cloud_account.CloudAccount	
	return account:get_user_login_status()
end
--- check whether the binded cloud acclout logins
--- if login, return with username
-- @param  http_form    form contains the needed variable
-- @return 				false, or true with the logined username
function check_login_with_username_ret()
	if check_login() then
		local data = cloud_account.CloudAccount:user_info()		
		return data
	else
		return false
	end
end

--- check if the device has ever been logined with cloud accout
function cloud_ever_login()
	local account = cloud_account.CloudAccount	
	local data = { ["cloud_ever_login"] = account:get_cloud_ever_login() }	
	return data
end

--- cloud user login. 
--- the cloud server do not support user login operation currentyly, call user_bind actually
-- @param  http_form    form contains the needed variable
-- @return 				errcode of the operation
function user_login(http_form)
    local data = {}
	local err_code = nil
    local user = http_form["username"]
    local pass = http_form["password"]
	local account = cloud_account.CloudAccount
	pass = asycrypto.decrypt(pass)		

	err_code = account:user_login(user, pass)
	return err_code
end

--- cloud user login in account page
-- @param  http_form    form contains the needed variable
-- @return true with username, or false with err_msg
function user_login_accountpage(http_form)
    local data = {}
	local err_code = nil
	local account = cloud_account.CloudAccount


    local macaddr
    local ctypes = require "luci.model.checktypes"
    local ipaddr = sys.getenv("REMOTE_ADDR")
    local remote = not ctypes.check_ip_in_lan(ipaddr)

    local cm = require "luci.model.client_mgmt"
    if not remote then
        macaddr = cm.get_mac_by_ip(ipaddr)
        assert(macaddr, "lan mac is nil!")
    end
    local addr = remote and ipaddr or macaddr

    local attempts_addr = addr.."_CloudAccount"
    local attempts = read_attempts()
    reap_attempts(attempts)
    local att = attempts[attempts_addr] or {attempts = 0}
    if att.attempts >= ATTEMPTS_MAX then
        data = {}
        data.failureCount = att.attempts
        data.attemptsAllowed = ATTEMPTS_MAX - att.attempts
        return false, "exceeded max attempts", data
    end
	
	err_code = user_login(http_form)
	if "0" == err_code then	
        attempts[attempts_addr] = nil
        write_attempts(attempts)	
		local data = {
			["username"] = account:get_login_username()
		}
		return data	
	else
        if err_code == cloud_account.ERROR_ACCOUNT_NOT_FOUND 
           or err_code == cloud_account.ERROR_PASSWORD_INCORRECT then
            att.attempts = att.attempts + 1
        end
        att.ltime = sys.uptime()
        attempts[attempts_addr] = att
        write_attempts(attempts)
        data = {}
        data.failureCount = att.attempts
        data.attemptsAllowed = ATTEMPTS_MAX - att.attempts
		return false, err_code, data
	end
end

local function inited()
    local uci_r = require("luci.model.uci").cursor()
	uci_r:set("wportal", "defcfg", "defcfg", "no")
	uci_r:commit("wportal")
	
	sys.fork_exec("wportalctrl -c")
	sys.fork_exec("echo \"stop\" > /tmp/wportal/status")
	sys.fork_exec("/etc/init.d/wportal restart")
end

--- user login with cloud account
-- @param  http_form    form contains the needed variable
-- @return 				result of the operation
function login(http_form)
    local data = {}

    local sauth = require "luci.sauth"
    local sys = require "luci.sys"
    local ctypes = require "luci.model.checktypes"

    local user = http_form["username"]
    local pass = http_form["password"]
    local ipaddr = sys.getenv("REMOTE_ADDR")
	local device_login = false 			-- login with the device password
	
	-- if no username, only password, login with the device password
	if nil == user then
		local uci_r = require("luci.model.uci").cursor()        
		uci_r:foreach("accountmgnt", "account",
			function(section)
				if nil == user then
					user = section["username"]				
				end
			end
		)				
		device_login = true
	end
	
    local macaddr
    local remote = not ctypes.check_ip_in_lan(ipaddr)

    local cm = require "luci.model.client_mgmt"
    if not remote then
        macaddr = cm.get_mac_by_ip(ipaddr)
        assert(macaddr, "lan mac is nil!")
    end
    local addr = remote and ipaddr or macaddr

    local allow, sid, sdat = sauth.limit(addr, remote)
    if not allow then
        data.logined_user = sdat.user
        data.logined_remote = sdat.remote
        if sdat.remote then
            data.logined_ip = sdat.addr
        else
            data.logined_mac = sdat.addr
            local client = cm.get_client_by("mac", data.logined_mac)
            if client then
                data.logined_ip = client.ip
                data.logined_host = client.hostname
            end
        end

        local uci_r = require("luci.model.uci").cursor()
        local preempt = uci_r:get("administration", "login", "preempt")
        if preempt == "off" then
            return false, "user conflict", data
        end
    end
    -- count login times each for cloud login and device login, chenyuqi,16Jul15
    local attempts_addr = (true == device_login and addr or addr.."_CloudAccount")
    local attempts = read_attempts()
    reap_attempts(attempts)
    local att = attempts[attempts_addr] or {attempts = 0}
    if att.attempts >= ATTEMPTS_MAX then
		data = {}
		data.failureCount = att.attempts
		data.attemptsAllowed = ATTEMPTS_MAX - att.attempts
        return false, "exceeded max attempts", data
    end

	local acc_check = require("luci.model.accountmgnt").check
	local login_err_msg = nil
	if true == device_login then
		-- if default factory config, set the password and login		
		if require("luci.sys.config").isdftconfig() then
		    local accmgnt   = require "luci.model.accountmgnt"
			accmgnt.set(user, pass)
			inited()
			sys.fork_exec("wportalctrl -c")
			sys.fork_exec("echo \"stop\" > /tmp/wportal/status")
			sys.fork_exec("/etc/init.d/wportal restart")
		end
		if false == acc_check(user, pass) then
			login_err_msg = "login failed"
		else
			cloud_account.CloudAccount:set_user_login_status(false)	-- cloud account logout
		end
	else -- true ~= device_login 
		local err_code = user_login(http_form)
		if "0" ~= err_code then
			login_err_msg = err_code
		end
	end
	
    if nil == login_err_msg then
		attempts[attempts_addr] = nil
		write_attempts(attempts)
	else
        if login_err_msg == "login failed" 
            or login_err_msg == cloud_account.ERROR_ACCOUNT_NOT_FOUND 
            or login_err_msg == cloud_account.ERROR_PASSWORD_INCORRECT then
            att.attempts = att.attempts + 1
        end
        att.ltime = sys.uptime()
        attempts[attempts_addr] = att
        write_attempts(attempts)
		data = {}
		data.failureCount = att.attempts
		data.attemptsAllowed = ATTEMPTS_MAX - att.attempts
        return false, login_err_msg, data
    end

    local token = sys.uniqueid(16)
    if sid then
        sauth.kill(sid)
    end

    sid = sys.uniqueid(16)
    sauth.write(sid, {
                    addr = addr,
                    remote = remote,
                    user = user,
                    token = token,
                    secret = sys.uniqueid(16)
    })

    local dl = require "luci.controller.domain_login"
    dl.tips_cancel()

    local http = require "luci.http"
    http.header("Set-Cookie", "sysauth=" .. sid
                    .. ";path=" .. sys.getenv("SCRIPT_NAME") or "")
    data.stok = token
    return data
end

--- logout the cloud accout
-- @param  N/A
-- @return N/A
function user_logout()
	local account = cloud_account.CloudAccount
	account:set_user_login_status(false)
end

--- logout the cloud accout and exit the DUT management webpage
-- @param  N/A
-- @return N/A
function logout()
	local system = require("luci.controller.admin.system")	
	user_logout()
	system.logout()
end

--- get user information: username and password
-- @param  	N/A
-- @return 				username and password
function user_info()
	local account = cloud_account.CloudAccount
	local data = account:user_info()		
	local err_code = data["err_code"]
	if "0" == err_code then
	return data
	else
		return false, err_code
	end
end

--- cloud server to send the verify code for password resetting
-- @param  N/A   	give the register account would better
-- @return 				true/false, verified or not
function user_check_mail_verified()
	local account = cloud_account.CloudAccount
	if account:user_check_mail_verified() then
		local data = { ["username"] = cloud_account.CloudAccount:get_register_username() }
		return data
	else
		return false
	end
end

--- cloud accout register
-- @param  http_form    form contains the needed variable
-- @return 				result of the operation
function user_register(http_form)
	local err_code = nil
	local email = http_form["email"]
	local username = http_form["username"]
	local password = http_form["password"]
	local account = cloud_account.CloudAccount
	password = asycrypto.decrypt(password)
	err_code = account:user_register(email, username, password, 0) -- 0 mail, 1 tel
	if "0" == err_code then
		return true
	else
		return false, err_code
	end	
end

--- resend the register mail, after finish the register operation
-- @param  
-- @return 				result of the operation
function user_register_resend()
	local err_code = nil
	local account = cloud_account.CloudAccount

	err_code = account:user_register_resend()
	if "0" == err_code then
		return true
	else
		return false, err_code
	end	
end

--- login after register, the user do not need to input the account and password
--- read accout and password from cloud_config.register
--- operate in quick_setup or account page
function cloud_register_login_account()
	local err_code = nil
	local account = cloud_account.CloudAccount
    local user = account:get_register_username()
    local pass = account:get_register_password()
	
	err_code = account:user_login(user, pass)
	if "0" == err_code then
		local data = {
			["username"] = account:get_login_username()
		}
		return data
	else
		return false, err_code
	end
end

--- operate in login page
function cloud_register_login()
	local err_code = nil
	local account = cloud_account.CloudAccount
    http_form = 
	{
		["username"] = account:get_register_username(),
		["password"] = account:get_register_password()
	}
    http_form["password"] = asycrypto.encrypt(http_form["password"])
	return  login(http_form)	
end

--- cloud server to send the verify code for password resetting
-- @param  http_form    form contains the needed variable
-- @return 				result of the operation
function get_reset_pwd_email(http_form)
	local err_code = nil
	local email = http_form["email"]
	local account = cloud_account.CloudAccount
	err_code = account:get_reset_pwd_email(email, 0) -- 0 mail, 1 tel
	if "0" == err_code then
		return true
	else
		return false, err_code
	end		
end

--- reset the cloud accout password without old password
-- @param  http_form    form contains the needed variable
-- @return 				err_code of the operation
function reset_cloud_password(http_form)
	local err_code = nil
	local account = cloud_account.CloudAccount
	local username = http_form["username"]
	local vercode = http_form["vercode"]
	local password = http_form["password"]
	password = asycrypto.decrypt(password)
	vercode = asycrypto.decrypt(vercode)	
	err_code = account:reset_cloud_password(username, password, vercode, 0)
	return err_code
end

--- modify the cloud accout password (old password needed)
-- @param  http_form    form contains the needed variable
-- @return 				result of the operation
function modify_cloud_password(http_form)
	local data = {}
	local err_code = nil
    local user = http_form["username"]
	local old_pwd = http_form["old_password"]
	local new_pwd = http_form["new_password"]
	local account = cloud_account.CloudAccount
	old_pwd = asycrypto.decrypt(old_pwd)
	new_pwd = asycrypto.decrypt(new_pwd)
	err_code = account:modify_cloud_password(user, old_pwd, new_pwd)
	if "0" == err_code then
		return true
	else
		return false, err_code
	end	
end

--- login page, reset cloud password and login 
-- @param 	
-- @return  
function cloud_reset_pwd_login(http_form)
	local form = http_form["form"]
	local err_code = reset_cloud_password(http_form)	
	if err_code == "0" then
		-- login from the login.html
		if form == "cloud_reset_pwd_login" then
			return login(http_form)
		-- login from the basic_accout.html, only need login cloud user
		elseif form == "cloud_reset_pwd_login_cloud" then
			--return user_login(http_form)
			local err_code = user_login(http_form)
			if "0" == err_code then
				-- return username, so that updata the account info in the page
				local data = {["username"] = cloud_account.CloudAccount:get_login_username()}
				return data				
			else
				return false, err_code
			end
		end
	else
		return false, err_code
	end
end

-- TODO need adjust, should ping google.com yahu.com ... cloud.tplink...
--- check whether the DUT connect to internet successfully
-- @param 	 N/A
-- @return   true or false
function check_internet()
	local is_connected = false
	local socket = require("socket")            
	local test = socket.tcp()
	test:settimeout(1000)                   -- Set timeout to 1 second           
	--local netConn = test:connect("172.29.88.113", 10080)	
	if sys.call("online-test") ~= 0 then
        is_connected = false
    else
		is_connected = true		
	end
	test:close()
	return is_connected
end

-- check if legal device
-- @param    N/A
-- @return   true or false
function check_device()
	local uci_r = require("luci.model.uci").cursor()
	local illegal = uci_r:get("cloud_config", "device_legality", "illegal")
	if illegal == "0" then
		return true
	end  

	local errorcode = "-10000"
	local data = {}
	local illegal_type = uci_r:get("cloud_config", "device_legality", "illegal_type")
	if illegal_type then
		errorcode = illegal_type
	end

	return false, errorcode, data
end

--- check if the device in default config.
--- configtool.isdftconfig just compare the password
function check_factory_default()
	local configtool = require "luci.sys.config"
	local data = { ["is_default"] = configtool.isdftconfig() }
	
	return data
end

--- another version for checking whether the dev is default config
--- if the device have not logined with cloud account, it's defaultcfg
--- after login, switch to quick setup.
function check_factory_default_cloud()
    local account = cloud_account.CloudAccount
    return (not account:get_cloud_ever_login())
end

--- device unbind the cloud accout
function user_unbind(http_form)
	local err_code = nil
	local account = cloud_account.CloudAccount
--	local user_info = account:user_info()
	
--	err_code = account:user_unbind(user_info.username, user_info.password)
	err_code = account:user_unbind()
	if "0" == err_code then
		return true
	else
		return false, err_code
	end	
end

function load_fw_list()
	local account = cloud_account.CloudAccount
	return account:load_fw_ver()
end

function get_fw_list()
    local account = cloud_account.CloudAccount
    return account:get_fw_ver()
end

function cloud_fw_upgrade()
    local account = cloud_account.CloudAccount

    return account:fw_upgrade()
end

function get_download_detail(processcmd)
    local uci_r = require("luci.model.uci").cursor()
    local percent = uci_r:get("cloud_status", "client_info", "fw_download_progress")   

	if tonumber(percent) >= 100	and processcmd then
		local url = uci_r:get("cloud_config", "upgrade_info", "download_url")
		local filename = string.match(url, ".+/([^/]*%.%w+)$")
		filename = "/tmp/" .. filename
		if nixio.fs.access(filename) then
			dbg.print("begin upgrade firmware...")
			sys.fork_exec("sleep 1;nvrammanager -u  " .. filename)
        else
            return	false
        end
	end

	local err_code
	local actStatus = uci_r:get("cloud_status", "download_fw", "action_status")
	local dlStatus  = uci_r:get("cloud_status", "client_info", "fw_download_status")

	--try download or send download request sucessfully.
	if actStatus == "3" or actStatus == "4"	then 
		if dlStatus == "0" then
			err_code = "-20701"
			return false, err_code,{["percent"] = percent}
		end

		err_code = "0"
		return {["percent"] = percent, ["err_code"] = err_code}
	else
		err_code = uci_r:get("cloud_status", "download_fw", "err_code")
		return false, err_code,{["percent"] = percent}
	end
end

function detect_upgrade_status()
	return get_download_detail(true)
end

function check_upgrade()
    local account = cloud_account.CloudAccount
    local update_number = account:check_need_upgrade()
	
	if(update_number == nil) then
		return {["update_number"] = "0"}  
	end      
    
        return {["update_number"] = update_number}    
end

function check_cloud_version()
    local display
    local uci_r = require("luci.model.uci").cursor()
    local tcsp_status = uci_r:get("cloud_config", "info", "tcsp_status")
    local show_flag = uci_r:get("cloud_config", "info", "show_flag")
    
    if show_flag ~= "1" and tcsp_status == "2" then
        display = "2"
    elseif show_flag ~= "1" and tcsp_status == "3" then
        display = "3"
    else
        display = "1"
    end
    return {["type"] = display} 
end

function set_show_flag()
    local uci_r = require("luci.model.uci").cursor()
	uci_r:set("cloud_config", "info", "show_flag", "1")
	uci_r:commit("cloud_config")
    return true
end
-- General controller routines
local dispatch_tbl = {
	check_internet = {
		["read"] = {cb = check_internet},
	},
    check_device = {
        ["read"] = {cb = check_device},
    },
	cloud_upgrade = {
        ["load"] = {cb = load_fw_list},
        ["read"] = {cb = get_fw_list},
        ["upgrade"] = {cb = cloud_fw_upgrade},
    },
    detect_upgrade_status = {
        ["read"] = {cb = detect_upgrade_status},
    },
    check_upgrade = {
        ["read"] = {cb = check_upgrade},
    },
    check_cloud_version = {
        ["read"] = {cb = check_cloud_version},
    },
    set_show_flag = {
        ["write"] = {cb = set_show_flag},
    },
}

function dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end

function _index()	
    return ctl._index(dispatch)
end

function index()
	entry({"admin", "cloud_account"}, call("_index")).leaf = true	
end

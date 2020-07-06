--[[
Copyright(c) 2008-2015 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  	cloud-account.lua
Details :  	model for cloud accout webpage, work for cloud account operation such as register, login...
Author  :  	Chen Yuqi <chenyuqi@tp-link.net>
Version :  	1.0.0
Date    :  	12feb, 2015
History :  	01, 12Feb15, create file.			
			02, 26Feb15, code the functions.
			03, 09Feb15, 
]]--

module("luci.model.cloud_account", package.seeall)

local bus    = require "ubus"
local ucim   = require "luci.model.uci"
local util   = require "luci.util"
local dbg    = require "luci.tools.debug"
local configtool = require "luci.sys.config"

local UCI_CLOUD_CONFIG = "cloud_config"
local UCI_CLOUD_STATUS = "cloud_status"
local uci    = ucim.cursor()

CloudAccount = util.class()

ERROR_SUCCESS                         = "0"
ERROR_GENERIC                         = "-10000"
ERROR_DEVICE_HAS_BIND_OTHER_ACCOUNT   = "-20506"
ERROR_ACCOUNT_NOT_FOUND               = "-20600"
ERROR_PASSWORD_INCORRECT              = "-20601"
ERROR_ACCOUNT_INACTIVE                = "-20602"
ERROR_NEW_PASSWORD_FORMAT_ERROR       = "-20616"
ERROR_VERI_CODE_ERROR                 = "-20607"
ERROR_PASSWORD_FORMAT_ERROR           = "-20615"
--ERROR_ACCOUNT_DUPLICATED              = "-20603"
ERROR_UNBIND_DEVICE_ERROR             = "-20503"

ERROR_EMAIL_FORMAT_ERROR              = "-20200"
ERROR_USERNAME_FORMAT_ERROR           = "-20202"
ERROR_EMAIL_EXISTED                   = "-20621"
ERROR_USERNAME_EXISTED                = "-20622"
ERROR_ACCOUNT_LOCKED                  = "-20661"

ERROR_CLOUD_CLIENT_ISSUE              = "-51200"
ERROR_CONNECTING_TO_CLOUD_SERVER      = "-51218"
ERROR_REQUEST_TIMEOUT                 = "-51215"

ERROR_WAN_UNPLUGGED                   = "-50101"
ERROR_WAN_POOR_CONNECT                = "-50102"
ERROR_WAN_FAIL_DHCP                   = "-50103"
ERROR_WAN_FAIL_PPPOE_AUTH             = "-50111"
ERROR_WAN_FAIL_PPPOE_SERVER           = "-50112"
ERROR_WAN_FAIL_PPTP_AUTH              = "-50121"
ERROR_WAN_FAIL_PPTP_SERVER            = "-50122"
ERROR_WAN_FAIL_L2TP_AUTH              = "-50131"
ERROR_WAN_FAIL_L2TP_SERVER            = "-50132"
ERROR_WAN_FAIL_UNKNOWN                = "-50140"

local cloud_err_msg = 
{
    [ERROR_SUCCESS]                       = "Operation succeeded.",
    [ERROR_GENERIC]                       = "Unknown error.",
	
	[ERROR_DEVICE_HAS_BIND_OTHER_ACCOUNT] = "Device has bind other account",
	[ERROR_ACCOUNT_NOT_FOUND]             = "Account doesn't exist.",
	[ERROR_PASSWORD_INCORRECT]            = "Incorrect username or password.",
	[ERROR_ACCOUNT_INACTIVE]              = "Please activate this account first.",	
	[ERROR_NEW_PASSWORD_FORMAT_ERROR]     = "Invalid password.",	-- used for modify password
	[ERROR_VERI_CODE_ERROR]               = "Incorrect verification code.",
	[ERROR_PASSWORD_FORMAT_ERROR]         = "Invalid password.",	-- used for reset password
--	[ERROR_ACCOUNT_DUPLICATED]            = "Account already exists.",	
	[ERROR_UNBIND_DEVICE_ERROR]           = "Unbind device error",
	
	[ERROR_EMAIL_FORMAT_ERROR]              = "Invalid email format.",
	[ERROR_USERNAME_FORMAT_ERROR]           = "Invalid username format.",
	[ERROR_EMAIL_EXISTED]                   = "Email already exists.",
	[ERROR_USERNAME_EXISTED]                = "Username already exists.",
	[ERROR_ACCOUNT_LOCKED]                = "account locked.",
	
	[ERROR_CLOUD_CLIENT_ISSUE]            = "Cloud client application do not start or other issue!",
	[ERROR_CONNECTING_TO_CLOUD_SERVER]    = "Can not Connect to cloud server",
	[ERROR_REQUEST_TIMEOUT]               = "Request timeout",
}

--- get error message from err_code
-- @param  err_code    
-- @return error message
function CloudAccount:get_cloud_err_msg(err_code)
	return cloud_err_msg[err_code] or cloud_err_msg["-10000"]
end

--- get error code from err_code, make sure that the err_code return to webpage could be recognized
-- @param  err_code    
-- @return error code
function CloudAccount:get_cloud_err_code(err_code)
	if cloud_err_msg[err_code] then
		return err_code
	else
		return ERROR_GENERIC
	end
end

--- check whether the cloud status section can be read
--- need to read cloud status after ubus operation finish and set the err_code and owner of the section
-- @param  	section
-- @return 			ubus call return value
local function is_cloudstatus_set(section)
	os.execute("sleep "..1)
	local owner = ucim.cursor():get(UCI_CLOUD_STATUS, section, "owner")
	local loopcount = 0
	while "0"~=owner and loopcount<7 do
		os.execute("sleep "..1)
		owner = ucim.cursor():get(UCI_CLOUD_STATUS, section, "owner")
		loopcount = loopcount + 1
	end
	if "0"==owner then
		return true	
	else
		return false
	end
end

--- call ubus operation 
-- @param  	method
-- @param  	args
-- @param  	path
-- @return 			ubus call return value
function CloudAccount:invoke(method, args, path)
    if type(args) ~= "table" then
        args = {args}
    end

    if not self.ubus then
        self.ubus = bus.connect()
        if not self.ubus then
            log(nlog.UBUS_CONN_FAILED)
        end
    end

    path = path or "cloudclient"

    return self.ubus:call(path, method, args)
end

--- cloud user login actuall do bind, log the user name to login section
-- @param  	N/A
-- @return 	username
function CloudAccount:get_login_username()
	local username = uci:get(UCI_CLOUD_CONFIG, "login", "username")
	return username
end

--- cloud user login actuall do bind, log the user name to login section
-- @param  	username
-- @return 	N/A
function CloudAccount:set_login_username(username)
	uci:set(UCI_CLOUD_CONFIG, "login", "username", username)
	uci:commit(UCI_CLOUD_CONFIG)	
end

--- get cloud user login status, which store in cloud-client variable
-- @param  	N/A	
-- @return 			if success return with login status(true/false), else only return {success:false}
function CloudAccount:get_user_login_status()
	local args = { }

	local login_status = self:invoke("get_user_login_status",  args)
	if nil ~= login_status then		
		return login_status["user_login_status"]
	else
		dbg.print("Error: check_login() fail to read the login status from ubus!")
		return false
	end
end

--- set cloud user login status, which store in cloud-client variable
-- @param  	login_status	new login status to be set, true/false
-- @return 	N/A
function CloudAccount:set_user_login_status(login_status)
	local args = {user_login_status = login_status}

	local set_status = self:invoke("set_user_login_status",  args)	
end

--- get the status whether the device has been logined with cloud account
-- @param  	N/A	
-- @return 	true/false
function CloudAccount:get_cloud_ever_login()
	if "1" == uci:get(UCI_CLOUD_CONFIG, "device_status", "ever_login") then
		return true
	else
		return false
	end
end

--- set the status whether the device has been logined with cloud account
-- @param  	true/false	
-- @return 	N/A
function CloudAccount:set_cloud_ever_login(status)
	if true == status then
		uci:set(UCI_CLOUD_CONFIG, "device_status", "ever_login", "1")
	else
		uci:set(UCI_CLOUD_CONFIG, "device_status", "ever_login", "0")
	end
	uci:commit(UCI_CLOUD_CONFIG)
end

--- cloud user login. 
--- the cloud server do not support user login operation currentyly, call user_bind actually
-- @param  	username
-- @param	password
-- @return 				err_code
function CloudAccount:user_login(username, password)
	local err_code = nil
	local args = {user_conf_path = "cloud_config"}
	-- uci set username, password
	uci:set(UCI_CLOUD_CONFIG, "bind", "username", username)
	uci:set(UCI_CLOUD_CONFIG, "bind", "password", password)
	uci:set(UCI_CLOUD_STATUS, "bind", "err_code", ERROR_CLOUD_CLIENT_ISSUE)
	uci:commit(UCI_CLOUD_CONFIG)
    uci:commit(UCI_CLOUD_STATUS)
	-- call ubus operation
	local ubuscallret = self:invoke("user_bind",  args)	
	if is_cloudstatus_set("bind") then
		-- call ucim.cursor() here, it could read new setting by cloud-client, not ERROR_CLOUD_CLIENT_ISSUE
		err_code = ucim.cursor():get(UCI_CLOUD_STATUS, "bind", "err_code")		
	else
		dbg.print("Error: wait to read cloud_status timeout")
		err_code = "-10000"
	end

	if "0" == err_code then
		self:set_login_username(username)
		self:set_user_login_status(true)
		self:set_cloud_ever_login(true)
	end
	
	return err_code
end

--- cloud user login. 
--- the cloud server do not support user login operation currentyly, call user_bind actually
-- @param  	username
-- @param	password
-- @return 				err_code
--function CloudAccount:user_unbind(username, password)
function CloudAccount:user_unbind()
	local err_code = nil
	local args = {user_conf_path = "cloud_config"}
	local username = uci:get(UCI_CLOUD_CONFIG, "bind", "username")
	local password = uci:get(UCI_CLOUD_CONFIG, "bind", "password")
	-- uci set username, password
	uci:set(UCI_CLOUD_CONFIG, "unbind", "username", username)
	uci:set(UCI_CLOUD_CONFIG, "unbind", "password", password)
    uci:set(UCI_CLOUD_STATUS, "unbind", "err_code", ERROR_CLOUD_CLIENT_ISSUE)
    uci:commit(UCI_CLOUD_STATUS)
	uci:commit(UCI_CLOUD_CONFIG)
	-- call ubus operation
	local ubuscallret = self:invoke("user_unbind",  args)	
	if is_cloudstatus_set("unbind") then
		err_code = ucim.cursor():get(UCI_CLOUD_STATUS, "unbind", "err_code")
	else
		dbg.print("Error: wait to read cloud_status timeout")
		err_code = "-10000"
	end

	if "0" == err_code then		
		self:set_user_login_status(false)		
	end
	
	return err_code
end

--- get user information: username and password
-- @param  	N/A
-- @return 				data with username and password
function CloudAccount:user_info()
    local data = {}	
	local err_code = nil
	local args = { 
		["cloud_username"] = uci:get(UCI_CLOUD_CONFIG, "login", "username")
	}
    uci:set(UCI_CLOUD_STATUS, "get_account_info", "err_code", ERROR_CLOUD_CLIENT_ISSUE)
    uci:commit(UCI_CLOUD_STATUS)
	self:invoke("get_cloud_account_info",  args)
	if is_cloudstatus_set("get_account_info") then
		err_code = ucim.cursor():get(UCI_CLOUD_STATUS, "get_account_info", "err_code")
	else
		err_code = "-10000"
	end
	data["err_code"] = err_code
	data["email"] = ucim.cursor():get(UCI_CLOUD_CONFIG, "get_account_info", "email")
	data["username"] = ucim.cursor():get(UCI_CLOUD_CONFIG, "get_account_info", "username")
	return data
end

function CloudAccount:get_reg_verify_code(username, account_type)
	local result = nil
	local args = {user_conf_path = "cloud_config"}
	-- uci set username, password
	uci:set(UCI_CLOUD_CONFIG, "get_reg_verify_code", "username", username)
	-- account_type: 0 mail, 1 tel, where to judge its correction?
	uci:set(UCI_CLOUD_CONFIG, "get_reg_verify_code", "account_type", account_type)
	uci:commit(UCI_CLOUD_CONFIG)
	-- call ubus operation
	result = self:invoke("get_reg_verify_code",  args)
	dbg.print("result=", result)
	return result
end

-- check whether the new register account has been mail verified
-- @param  	N/A
-- @return 			true/false
function CloudAccount:user_check_mail_verified()
	local err_code = nil
	local args = { 
		["cloud_username"] = uci:get(UCI_CLOUD_CONFIG, "register", "username")
	}
	uci:set(UCI_CLOUD_CONFIG, "device_status", "account_status", "0")
    uci:set(UCI_CLOUD_STATUS, "get_account_stat", "err_code", ERROR_CLOUD_CLIENT_ISSUE)
    uci:commit(UCI_CLOUD_STATUS)
	uci:commit(UCI_CLOUD_CONFIG)
	self:invoke("get_cloud_account_stat",  args)
	
	if is_cloudstatus_set("get_account_stat") then
		stat = ucim.cursor():get(UCI_CLOUD_CONFIG, "device_status", "account_status")
		-- TODO, replace the 1 with sth
		return "1" == stat
	else
		return false
	end
end

function CloudAccount:get_register_email()
	return uci:get(UCI_CLOUD_CONFIG, "register", "email")
end

function CloudAccount:get_register_username()
	return uci:get(UCI_CLOUD_CONFIG, "register", "username")
end

function CloudAccount:get_register_password()
	return uci:get(UCI_CLOUD_CONFIG, "register", "password")
end

function CloudAccount:user_register(email, username, password, account_type)
	local err_code = nil
	local args = {user_conf_path = "cloud_config"}
	-- uci set username, password
	uci:set(UCI_CLOUD_CONFIG, "register", "email", email)
	uci:set(UCI_CLOUD_CONFIG, "register", "username", username)
	uci:set(UCI_CLOUD_CONFIG, "register", "password", password)
	uci:set(UCI_CLOUD_CONFIG, "register", "account_type", account_type)
	-- reset the account status, or else register two account continuously, the second one may get wrong status
	uci:set(UCI_CLOUD_CONFIG, "device_status", "account_status", "0")
    uci:set(UCI_CLOUD_STATUS, "register", "err_code", ERROR_CLOUD_CLIENT_ISSUE)
    uci:commit(UCI_CLOUD_STATUS)
	uci:commit(UCI_CLOUD_CONFIG)
	-- call ubus operation
	self:invoke("user_register",  args)	
	if is_cloudstatus_set("register") then
		err_code = ucim.cursor():get(UCI_CLOUD_STATUS, "register", "err_code")
	else
		err_code = "-10000"
	end

	return err_code
end

function CloudAccount:user_register_resend()
	local err_code = nil
	local args = {email = self:get_register_email()}
    uci:set(UCI_CLOUD_STATUS, "resend_email", "err_code", ERROR_CLOUD_CLIENT_ISSUE)
    uci:commit(UCI_CLOUD_STATUS)
	-- call ubus operation
	self:invoke("resend_register_email",  args)	

	if is_cloudstatus_set("resend_email") then
		err_code = ucim.cursor():get(UCI_CLOUD_STATUS, "resend_email", "err_code")
	else
		err_code = "-10000"
	end

	return err_code
end

--- cloud server to send the verify code for password resetting
-- @param 	sername
-- @param 	verify_code
-- @param 	account_type
-- @return 				result of the operation
function CloudAccount:get_reset_pwd_email(username, account_type)
	local err_code = nil
	local args = {user_conf_path = "cloud_config"}
	-- uci set username, password
	uci:set(UCI_CLOUD_CONFIG, "reset_account_pwd", "username", username)
	-- account_type: 0 mail, 1 tel, where to judge its correction?
	uci:set(UCI_CLOUD_CONFIG, "reset_account_pwd", "account_type", account_type)
	uci:commit(UCI_CLOUD_CONFIG)
    uci:set(UCI_CLOUD_STATUS, "reset_account_pwd", "err_code", ERROR_CLOUD_CLIENT_ISSUE)
    uci:commit(UCI_CLOUD_STATUS)
	-- call ubus operation	
	self:invoke("reset_cloud_password",  args)
	if is_cloudstatus_set("reset_account_pwd") then
		err_code = ucim.cursor():get(UCI_CLOUD_STATUS, "reset_account_pwd", "err_code")
	else
		dbg.print("Error: wait to read reset_account_pwd status timeout")
		err_code = "-10000"
	end
	return err_code
end

--- reset the cloud accout password without old password
-- @param 	sername
-- @param 	password
-- @param 	verify_code
-- @param 	account_type
-- @return 				result of the operation
function CloudAccount:reset_cloud_password(username, password, verify_code, account_type)
	local err_code = nil
	local args = {user_conf_path = "cloud_config"}
	-- uci set username, password
	uci:set(UCI_CLOUD_CONFIG, "reset_account_pwd", "username", username)
	uci:set(UCI_CLOUD_CONFIG, "reset_account_pwd", "password", password)
	uci:set(UCI_CLOUD_CONFIG, "reset_account_pwd", "verify_code", verify_code)
	uci:set(UCI_CLOUD_CONFIG, "reset_account_pwd", "account_type", account_type)	
	uci:commit(UCI_CLOUD_CONFIG)
	-- call ubus operation
	self:invoke("reset_cloud_password",  args)
	
	if is_cloudstatus_set("reset_account_pwd") then
		err_code = ucim.cursor():get(UCI_CLOUD_STATUS, "reset_account_pwd", "err_code")
	else
		dbg.print("Error: wait to read reset_account_pwd status timeout")
		err_code = "-10000"
	end

	return err_code
end

--- modify the cloud accout password (old password needed)
-- @param 	sername
-- @param 	old_pwd
-- @param 	new_pwd
-- @return 				result of the operation
function CloudAccount:modify_cloud_password(username, old_pwd, new_pwd)
	local err_code = nil
	local args = {user_conf_path = "cloud_config"}
	-- uci set username, password
	uci:set(UCI_CLOUD_CONFIG, "modify_account_pwd", "username", username)
	uci:set(UCI_CLOUD_CONFIG, "modify_account_pwd", "old_pwd", old_pwd)
	uci:set(UCI_CLOUD_CONFIG, "modify_account_pwd", "new_pwd", new_pwd)
	uci:commit(UCI_CLOUD_CONFIG)
    uci:set(UCI_CLOUD_STATUS, "modify_account_pwd", "err_code", ERROR_CLOUD_CLIENT_ISSUE)
    uci:commit(UCI_CLOUD_STATUS)	
	-- call ubus operation
	self:invoke("modify_cloud_password",  args)
	
	if is_cloudstatus_set("modify_account_pwd") then
		err_code = ucim.cursor():get(UCI_CLOUD_STATUS, "modify_account_pwd", "err_code")
	else
		dbg.print("Error: wait to read cloud_status timeout")
		err_code = "-10000"
	end	
	
	return err_code
end

function CloudAccount:ddns_register(domain)
	local args = {user_conf_path = "cloud_config"}
    local err_code = nil

    if domain == "" then
        return nil
    end 
    
	uci:set(UCI_CLOUD_CONFIG, "ddns_register", "domain", domain)
	uci:commit(UCI_CLOUD_CONFIG)

    self:invoke("ddns_register",  args)
    
	if is_cloudstatus_set("ddns_register") then
		err_code = uci:get(UCI_CLOUD_STATUS, "ddns_register", "err_code")
	else
		err_code = "-10000"
	end
    
	return err_code  
end

function CloudAccount:ddns_bind(domain)
	local args = {user_conf_path = "cloud_config"}
    local err_code = nil
    
    if domain == "" then
        return nil
    end 
    
    uci:set(UCI_CLOUD_CONFIG, "ddns_bind", "domain", domain)
	uci:set(UCI_CLOUD_CONFIG, "ddns_bind", "force", 0)
	uci:commit(UCI_CLOUD_CONFIG)

	self:invoke("ddns_bind",  args)
    
	if is_cloudstatus_set("ddns_bind") then
    
		err_code = uci:get(UCI_CLOUD_STATUS, "ddns_bind", "err_code")
	else
		err_code = "-10000"
	end
    
	return err_code  
end

function CloudAccount:ddns_unbind(domain)
	local args = {user_conf_path = "cloud_config"}
    local err_code = nil
    uci:set(UCI_CLOUD_CONFIG, "ddns_bind", "domain", domain)
	uci:commit(UCI_CLOUD_CONFIG)
    
    self:invoke("ddns_unbind",  args)
    
	if is_cloudstatus_set("ddns_unbind") then
		err_code = uci:get(UCI_CLOUD_STATUS, "ddns_unbind", "err_code")
	else
		err_code = "-10000"
	end
    
	return err_code  
end

function CloudAccount:ddns_unbind_all()
	local result = nil
	local args = {user_conf_path = "cloud_config"}
    local err_code = nil

    self:invoke("ddns_unbind_all",  args)
    
	if is_cloudstatus_set("ddns_unbind_all") then
		err_code = uci:get(UCI_CLOUD_STATUS, "ddns_unbind_all", "err_code")
	else
		err_code = "-10000"
	end
    
	return err_code   
end

function CloudAccount:ddns_get_domain_list()
	local result = nil
	local args = {user_conf_path = "cloud_config"}
	local index = 0
	local data = {}
	local state

	result = self:invoke("ddns_get_domain_list",  args)

	if result == nil then
		return true
	end

	local num = result.listNum
	
	while index < num do
		local device = "alias" .. index
		local domain = "domain" .. index
		local time = "registerTime" .. index
		local status = "status" .. index
		local curDomain = "curDomain" .. index

		if tonumber(result[curDomain]) == 1 then 
			state = true
		else
			state = false
		end
        --local mac = "mac" .. index
        
		data[#data + 1] = {
		    device = result[device],
		    domain = result[domain],
		    time   = result[time],
		    status = result[status],
            current_domain = state
           -- mac    = result[mac]
		}
		index = index + 1
	end
	return data  
end

function CloudAccount:ddns_delete(domain)
	local args = {user_conf_path = "cloud_config"}
    local err_code = nil
    uci:set(UCI_CLOUD_CONFIG, "ddns_delete", "domains", domain)
	uci:commit(UCI_CLOUD_CONFIG)
    
    self:invoke("ddns_delete",  args)
    
	if is_cloudstatus_set("ddns_delete") then
		err_code = uci:get(UCI_CLOUD_STATUS, "ddns_delete", "err_code")
	else
		err_code = "-10000"
	end
    
	return err_code  
end

function CloudAccount:load_fw_ver()
	local SID = {
		["00000000"] = "UN",
		["55530000"] = "US",
		["45550000"] = "EU",
		["43410000"] = "CA",
	}
		
	local region = " ("..(SID[configtool.getsysinfo("special_id")] or "UN")..")"
	local fw_cur_ver = configtool.getsysinfo("SOFTVERSION")
    local res = {
        --res.detail = "This is the latest version"
        latest_version = fw_cur_ver,
        detail = "",
        latest_flag = true,
        note_flag = false
    }
        local cur_verX, cur_verY = string.match(fw_cur_ver, "(%d+)%.(%d+)%.")
	local fw_latest_ver = ucim.cursor():get(UCI_CLOUD_CONFIG, "upgrade_info", "version")                                                                                            
    if fw_latest_ver then                                                                                                                                                           
        res.latest_version = fw_latest_ver                                                                                                                                          
        res.detail = ucim.cursor():get(UCI_CLOUD_CONFIG, "upgrade_info", "release_log") or ""                                                                                       
        res.latest_flag = false
	local last_verX, last_verY = string.match(fw_latest_ver, "(%d+)%.(%d+)%.")
	if tonumber(last_verX) ~= tonumber(cur_verX) or tonumber(last_verY) ~= tonumber(cur_verY) then
		res.note_flag = true
	end                                                                                                                                                     
    end
	res.latest_version = res.latest_version..region
	return res
end

function CloudAccount:get_fw_ver()
	local SID = {
		["00000000"] = "UN",
		["55530000"] = "US",
		["45550000"] = "EU",
		["43410000"] = "CA",
	}
		
	local region = " ("..(SID[configtool.getsysinfo("special_id")] or "UN")..")"
    local fw_cur_ver = configtool.getsysinfo("SOFTVERSION")
    local res = {
        --res.detail = "This is the latest version"
        latest_version = fw_cur_ver,
        detail = "",
        latest_flag = true
    }
	
	local fw_latest_ver = ucim.cursor():get(UCI_CLOUD_CONFIG, "upgrade_info", "version")
	if fw_latest_ver then
		res.latest_version = fw_latest_ver
		res.detail = ucim.cursor():get(UCI_CLOUD_CONFIG, "upgrade_info", "release_log") or ""
		res.latest_flag = false
	else
        local args = {
            ["user_conf_path"] = "cloud_config"
        }
        self:invoke("check_fw_version",  args)
        if is_cloudstatus_set("check_fw_ver") then
            fw_latest_ver = ucim.cursor():get(UCI_CLOUD_CONFIG, "upgrade_info", "version")
			if fw_latest_ver then
                res.latest_version = fw_latest_ver
                res.detail = ucim.cursor():get(UCI_CLOUD_CONFIG, "upgrade_info", "release_log") or ""
                res.latest_flag = false
            end 
        end
	end
	res.latest_version = res.latest_version..region
    return res
end

function CloudAccount:check_need_upgrade()
	local result = 0

	result = uci:get(UCI_CLOUD_CONFIG, "new_firmware", "fw_new_notify")
    
    return result
end

function CloudAccount:fw_upgrade()
    local dl_url = uci:get(UCI_CLOUD_CONFIG, "upgrade_info", "download_url")
    
    if dl_url == nil then
        return false, "illegel download url"
    end
    
    local args = {download_url = dl_url}
    self:invoke("download_fw", args)
    
 	return true
end

function CloudAccount:__init__()

end

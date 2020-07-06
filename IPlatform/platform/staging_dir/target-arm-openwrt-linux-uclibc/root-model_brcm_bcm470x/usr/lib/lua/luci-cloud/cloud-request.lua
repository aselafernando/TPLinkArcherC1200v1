--[[
 Copyright (C), 2010-2013, TP-LINK TECHNOLOGIES CO., LTD.
 All rights reserved.

 File name   	: cloud-request.lua
 Version		: 1.0 
 Description	: this script work as communicate interface for cloud service and luci

 Author     	: Wang FuYu <wangfuyu@tp-link.net>
 Create Date	: 2014-03-21
 
 History    	: modified by liwei 2014-12-05 version=1
------------------------------------------------------------

01, 21Mar14, Wang FuYu, create file.
--]]

module("luci-cloud.cloud-request", package.seeall)

local cloudUciApi = require("luci-cloud.cloud-uci-iface")
local cloudCommon = require("luci-cloud.cloud-common")
local cloudError = require("luci-cloud.cloud-error")
local cloudRequest = require("luci-cloud.cloud-request")
local jsonlib = require("luci.json")
-- local websys = require("luci.websys")
-- local setting = require("luci.torchlight.setting")
-- local err = require("luci.torchlight.error")
-- local validator = require("luci.torchlight.validator")
-- local luciDs = require("luci.controller.ds")
local dbg = require("luci.tools.debug")
local sys = require("luci.sys")

UPDATE_FW_NORMAL_TIME = 90
BIND_STATUS_FAILED = '0'

CLOUD_CONFIG_FILE_NAME = "cloud_config"
CONFIG_STATUS_FILE_NANE = "cloud_status"
NETWORK_CONFIG_FILE_NAME = "network"
DEVICE_INFO_FILE_NAME = "device_info"

USER_ACTIVE = "userActive"
NEW_FIRMWARE = "newFirmware"
LEGAL_DEVICE = "legalDevice"
ILLEGAL_DEVICE = "illegalDevice"
STOP_CONNECT = "stopConnect"
EXIT_CLOUD_CLIENT = "/etc/init.d/cloud_client stop"

ACTION_OWNER = {
	ACT_OWN_BY_LUCI = 0,
	ACT_OWN_BY_CLOUD = 1,
	ACT_OWN_MAX = 2,
}

ACTION_STATUS = {
	ACT_STAT_FAILED = 0,
	ACT_STAT_IDLE = 1,
	ACT_STAT_PREPARE = 2,
	ACT_STAT_TRYING = 3,
	ACT_STAT_SUCCESS = 4,
	ACT_STAT_TIMEOUT = 5,
	ACT_STAT_MAX = 6,
}

ACTIVE_STATUS = {
	ACTIVE_IDLE = 0,
	ACTIVE_WAITING = 1,
	ACTIVE_SUCESSED = 2,
}

WAN_TYPE = {
	{name = 'dynamic', id = 0},
	{name = 'static', id = 1},
	{name = 'pppoe', id = 2},
	{name = 'pppoe+dynamic', id = 3},
	{name = 'pppoe+static', id = 4},
	{name = 'pptp+dynamic', id = 5},
	{name = 'pptp+static', id = 6},
	{name = 'l2tp+dynamic', id = 7},
	{name = 'l2tp+static', id = 8},
}

WIFI_SEC_TYPE = {
	{name = 'none', id = 0},
	{name = 'mixed-psk', id = 1},
}

local ERR_CODE = cloudError.ERR_CODE
local ERR_MSG = cloudError.ERR_MSG

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: getLanInfo()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	: get lan information                                                                                   
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]]
function getLanInfo()	
	local rv = {}
	local infodata = { 
		lanIp = { lan = "ipaddr" }, 
	}
	
	local result = cloudUciApi.getUciData(NETWORK_CONFIG_FILE_NAME, infodata)
	if result == nil or cloudCommon.isEmptyTable(result) then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_GET_DATA_FAILED[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_GET_DATA_FAILED[2] 
		cloudCommon.writeJson(rv)
		return nil
	end	
-- modified by liwei version=1

	local infodata1 = {
		mac = { info = "mac" },
	}
--	local _ubus = require "ubus".connect()
--	local lan_mac = _ubus:call("tddpServer", "getInfo", {infoMask=1, sep=":"})
	local lan_mac = cloudUciApi.getUciData(DEVICE_INFO_FILE_NAME, infodata1)
-- end by liwei version=1
	if lan_mac["mac"] then
		local tmpLanMac = string.upper(lan_mac["mac"])
		result["lanMac"] = string.gsub(tmpLanMac, ":", "")
	else
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_GET_DATA_FAILED[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_GET_DATA_FAILED[2] 
	end

	if rv[ERR_CODE] == nil then
		rv[ERR_CODE] = 0
		rv.result = result
	end
	
	cloudCommon.writeJson(rv)
end

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: getWanInfo()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	: get wan information                                                                                   
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]] 
function getWanInfo()
	local ntm = require "luci.model.network".init()
	local wan, device = ntm:mod_get_wan_net_and_dev()
	local rv = {}
	local result = {}

	if wan then
		result = {
			wanIp  		= wan:ipaddr(),
			wanGateway  	= wan:gwaddr(),
			wanMask 		= wan:netmask(),
			wanType   	= wan:proto(),
			dns  			= wan:dnsaddrs(),
		}
	end

	if result == nil or cloudCommon.isEmptyTable(result) then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_GET_DATA_FAILED[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_GET_DATA_FAILED[2] 
		cloudCommon.writeJson(rv)
		return nil
	end
	
	if (#result.dns >= 1) then
		result.dns1 = result.dns[1]
		result.dns2 = result.dns[2]
		result.dns = nil
	end

	local wanType = result.wanType
	for i,v in ipairs(WAN_TYPE) do 
		if wanType == v.name then
			result.wanType = nil
			result.wanTpye = v.id
			break
		end
	end

	if rv[ERR_CODE] == nil then
		rv[ERR_CODE] = 0
		rv.result = result
	end

	cloudCommon.writeJson(rv)
end


--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: TrimStr()                                                                             
* AUTHOR		: Teng Fei <tengfei@tp-link.net>                                                                       
* DESCRIPTION	: trim string                                                                               
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]]
local function TrimStr(str)
	--local tmpstr = string.upper(str)
	local tmpstr = str
	tmpstr = string.gsub(tmpstr, "-", "")
	str = string.match(tmpstr, "%w+")
	str = str:upper()
	return str
end

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: getDeviceInfo()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	: get device information                                                                                   
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]]
function getDeviceInfo()
	local rv = {}
	local infodata = {
		bindStatus = { device_status = "bind_status" },
		cloudUserName = { bind = "username" },
        alias = {info = "alias"}
	}
	local devInfo = {}

	devInfo.deviceMac = TrimStr(sys.exec("getfirm MAC"))
	dbg("devInfo.deviceMac == " .. devInfo.deviceMac)
	devInfo.deviceId = TrimStr(sys.exec("getfirm DEV_ID"))
	devInfo.hwId = TrimStr(sys.exec("getfirm HW_ID"))
	devInfo.fwId = TrimStr(sys.exec("getfirm FW_ID"))
	devInfo.deviceName = string.gsub(sys.exec("getfirm MODEL"), "%c", "")
	devInfo.deviceModel = devInfo.deviceName
	devInfo.deviceHwVer = string.gsub(sys.exec("getfirm HARDVERSION"), "%c", "")
	devInfo.fwVer = string.gsub(sys.exec("getfirm SOFTVERSION"), "%c", "")

	if devInfo == nil or cloudCommon.isEmptyTable(devInfo) then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_GET_DATA_FAILED[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_GET_DATA_FAILED[2] 
		cloudCommon.writeJson(rv)
		return nil
	end

	result = devInfo;
	local rv1 = cloudUciApi.getUciData(CLOUD_CONFIG_FILE_NAME, infodata)
	if rv1.bindStatus == "1" and rv1.cloudUserName ~= nil then
		result.cloudUserName = rv1.cloudUserName
	else
		result.cloudUserName = ""
	end
    result.alias = rv1.alias

	if rv[ERR_CODE] == nil then
		rv[ERR_CODE] = 0
		rv.result = result
	end
	
	cloudCommon.writeJson(rv)
end

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: getFwCurrentVer()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	: get current firmware version                                                                                  
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]]
function getFwCurrentVer()
	local rv = {}

-- modified by liwei version=1
--	local ubus = require("ubus")
--	local _ubus = ubus.connect()

	local infodata = {
		fwVer = { info = "sw_version" },
	}

	local infodata1 = {
		fw_cur_id = { info = "fw_cur_id" },
	}

	--infoMask, 1:mac, 2:pin, 4:deviceId, 8:hwId, 16:hwIdDes, 32:fwCurrentId
--	local args = {infoMask = 32, sep = ':'}
--	local devInfo = _ubus:call("tddpServer", "getInfo", args)

	local devInfo = cloudUciApi.getUciData(DEVICE_INFO_FILE_NAME, infodata1)

-- end by liwei version=1
	if devInfo.fw_cur_id == nil then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_GET_DATA_FAILED[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_GET_DATA_FAILED[2] 
		cloudCommon.writeJson(rv)
		return nil
	end

	local result = cloudUciApi.getUciData(DEVICE_INFO_FILE_NAME, infodata)
	if result == nil or cloudCommon.isEmptyTable(result) then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_GET_DATA_FAILED[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_GET_DATA_FAILED[2] 
		cloudCommon.writeJson(rv)
		return nil
	end

	result.fwId = devInfo.fw_cur_id

	if rv[ERR_CODE] == nil then
		rv[ERR_CODE] = 0
		rv.result = result
	end
	
	cloudCommon.writeJson(rv)
end

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: getWifiBasic()                                                                             
* AUTHOR 		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	: get wifi basic information                                                                                   
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]]
function getWifiBasic(wifiBasicPara)
	local rv = {} 
	local result = {}
	local infodata = {}
	
	if wifiBasicPara == nil then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[2] 
		cloudCommon.writeJson(rv)
		return nil
	end
	
	local wifiBand = wifiBasicPara.wifiBand
	local wifiDevice = ""
	local wifiIface = ""
	
	if wifiBand == 0 then
		wifiDevice = "wifi0"
		wifiIface = "ath0"
	elseif wifiBand == 1 then
		wifiDevice = "wifi1"
		wifiIface = "ath1"
	else
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_INVALID_PARAMS[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_INVALID_PARAMS[2] 
		cloudCommon.writeJson(rv)
		return nil
	end
	
	infodata = {
		wifiState = { [wifiDevice] = "disabled" },
		wifiSsid = { [wifiIface] = "ssid" },
		wifiSecType = { [wifiIface] = "encryption" },
		wifiKey = { [wifiIface] = "key" }
	}

	result = cloudUciApi.getUciData("wireless", infodata)
	if nil ~= result.wifiState then
		local wifiState = tonumber(result.wifiState)
		result.wifiState = nil
		result.wifiState = wifiState
	end
	
	if result.wifiState == 0 then
		result.wifiState = 1
	elseif result.wifiState == 1 then
		result.wifiState = 0
	end

	if result == nil or cloudCommon.isEmptyTable(result) then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_GET_DATA_FAILED[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_GET_DATA_FAILED[2] 
	end

	local wifiSecType = result.wifiSecType
	for i,v in ipairs(WIFI_SEC_TYPE) do 
		if wifiSecType == v.name then
			result.wifiSecType = nil
			result.wifiSecType = v.id
			break
		end
	end

	if rv[ERR_CODE] == nil then
		rv[ERR_CODE] = 0
		rv.result = result
	end
	
	cloudCommon.writeJson(rv)
end

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: setAlias()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	:                                                                                   
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                   
******************************************************************************                                                        
--]]
function setAlias(aliasPara)
	local rv = {}
	
	if aliasPara == nil then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[2] 
		cloudCommon.writeJson(rv)
		return nil
	end
	
	local alias = aliasPara.alias
	local infodata = {
		info = { alias = alias }
	}

	rv = cloudUciApi.setUciData(CLOUD_CONFIG_FILE_NAME, infodata)	
	cloudCommon.writeJson(rv)
end

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: setDeviceLoginPasswd()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	: change device's login password                                                                                  
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                  
******************************************************************************                                                        
--]]
function setDeviceLoginPasswd(deviceLoginPara)
	local rv = {}
	local result = {}
	
	if deviceLoginPara == nil then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[2] 
		cloudCommon.writeJson(rv)
		return nil
	end
	
	local username = setting.DEFAULT_USER
	local oldDevicePasswd = deviceLoginPara.oldDevicePasswd
	local devicePasswd = deviceLoginPara.devicePasswd

	oldDevicePasswd = websys.encrypt_passwd(oldDevicePasswd)
	devicePasswd = websys.encrypt_passwd(devicePasswd)
	
	if false == websys.check_webpasswd(username, oldDevicePasswd) then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_PASSWORD_FORMAT_ERROR[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_PASSWORD_FORMAT_ERROR[2] 
	else
		local errCode =  validator.check_passwd(devicePasswd)
		if errCode == err.ENONE then
			if true == websys.set_webpasswd(username, devicePasswd) then
				rv = 0
			else
				rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_GENERIC[1]
				rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_GENERIC[2] 
			end
		else
			rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_PASSWORD_INVALID[1]
			rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_PASSWORD_INVALID[2] 
		end
	end

	if rv == 0 then 
		result[ERR_CODE] = 0
	end
	
	cloudCommon.writeJson(result)	
end

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: setWifiBasic()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                                                                                                          
* DESCRIPTION	: set wifi basic information                                                                                   
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                    
******************************************************************************                                                        
--]]
function setWifiBasic(wifiBasicPara)
	local rv = {}	
	
	if wifiBasicPara == nil then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[2] 
		cloudCommon.writeJson(rv)
		return nil
	end
	
	local wifiBand = wifiBasicPara.wifiBand
	local wifiState = wifiBasicPara.wifiState
	local wifiSsid = wifiBasicPara.wifiSsid
	local wifiSecType = wifiBasicPara.wifiSecType
	local wifiKey = wifiBasicPara.wifiKey
	
	local wifiDevice = ""
	local wifiIface = ""
	
	if wifiBand == 0 then
		wifiDevice = "wifi0"
		wifiIface = "ath0"
	elseif wifiBand == 1 then
		wifiDevice = "wifi1"
		wifiIface = "ath1"
	else
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_INVALID_PARAMS[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_INVALID_PARAMS[2] 
		cloudCommon.writeJson(rv)
		return nil
	end

	if wifiState == 1 then
		disabled = "0"
	elseif wifiState == 0 then
		disabled = "1"
	else
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_INVALID_PARAMS[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_INVALID_PARAMS[2] 
		cloudCommon.writeJson(rv)
		return nil
	end

	for i,v in ipairs(WIFI_SEC_TYPE) do 
		if wifiSecType == v.id then
			wifiSecType = nil
			wifiSecType = v.name
			break
		end
	end
	
	local infodata = {
		[wifiDevice] = { disabled = disabled }, 
		[wifiIface] = { ssid = wifiSsid,  encryption = wifiSecType, key = wifiKey }
	}

--[[
	for secName, OptKwargs in pairs(infodata) do
		for optName, optValue in pairs(OptKwargs) do
			print("hhhhhhhh---"..secName.."---"..optName.."---"..optValue.."----")
		end
	end
--]]

	rv = cloudUciApi.setUciData("wireless", infodata)
	cloudCommon.writeJson(rv)
end

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: callFwUpdate()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	:                                                                                   
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                
******************************************************************************                                                        
--]]
--[[  TODO 
function callFwUpdate(fwUpdatePara)
	local rv = {}

	if fwUpdatePara == nil then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[2] 
		cloudCommon.writeJson(rv)
		return nil
	end
	
	local download_url = fwUpdatePara.fwUrl

	rv.updateTime = UPDATE_FW_NORMAL_TIME;
	
	--download firmware
	--local cmdString = string.format("ubus call cloudclient download_fw '{\"download_url\":%s}'", download_url)
	--os.execute(cmdString)

	--while true do	
		--os.execute("sleep(1)")	
		--update
	--end
	
	cloudCommon.writeJson(rv)	
end
--]]
--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: getFwDownloadProgress()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	:                                                                                   
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                  
******************************************************************************                                                        
--]]
function getFwDownloadProgress()
	local rv = {}
	local infodata = {
		progress = { client_info = "fw_download_progress" },
	}

	local result = cloudUciApi.getUciData(CONFIG_STATUS_FILE_NANE, infodata)
	if result == nil or cloudCommon.isEmptyTable(result) then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_GET_DATA_FAILED[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_GET_DATA_FAILED[2] 
		cloudCommon.writeJson(rv)
		return nil
	end

	local progress = tonumber(result.progress)
	result.progress = nil
	result.progress = progress

	--status:0 means download failed, 1 means downloading, 2 means download end, 3 means not start
	if result.progress == 0 then
		result.status = 0
	elseif result.progress == 100 then
		result.status = 2
	elseif result.progress > 0 and result.progress < 100 then
		result.status = 1
	else
		result.status = 3
	end

	if rv[ERR_CODE] == nil then
		rv[ERR_CODE] = 0
		rv.result = result
	end
	
	cloudCommon.writeJson(rv)
end

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: notifyEvent()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	: process notify event which receive from cloud-server                                                                                  
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]]
function notifyEvent(eventPara)
	local rv = {}
	local exitCloudClientFlag = 0
	
	if eventPara == nil then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[2] 
		cloudCommon.writeJson(rv)
		return nil
	end

	local event = eventPara.event
	local attribute = eventPara.attribute
	if event == USER_ACTIVE then
		local updata = {
			account_stat = { account_stat = ACTIVE_STATUS.ACTIVE_SUCESSED, }
		}

		rv = cloudUciApi.setUciData(CLOUD_CONFIG_FILE_NAME, updata)		
	elseif event == NEW_FIRMWARE then
		if attribute == nil or attribute.fwUpdateType == nil then
			rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[1]
			rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[2] 
			cloudCommon.writeJson(rv)
			return nil
		end
	
		local updata = {
			new_firmware = { fw_new_notify = 1, 
							fw_update_type = attribute.fwUpdateType, 
			}
		}

		rv = cloudUciApi.setUciData(CLOUD_CONFIG_FILE_NAME, updata)
	elseif event == LEGAL_DEVICE then
		local updata = {
			device_legality = { illegal = 0, }
		}
		
		rv = cloudUciApi.setUciData(CLOUD_CONFIG_FILE_NAME, updata)
	elseif event == ILLEGAL_DEVICE then
		if attribute == nil or attribute.illegalType == nil then
			rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[1]
			rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[2] 
			cloudCommon.writeJson(rv)
			return nil
		end
	
		local updata = {
			device_legality = { illegal = 1, 
							illegal_type = attribute.illegalType, 
			}
		}

		rv = cloudUciApi.setUciData(CLOUD_CONFIG_FILE_NAME, updata)
		exitCloudClientFlag = 1
	elseif event == STOP_CONNECT then
		if attribute == nil or attribute.reconnectTime == nil or attribute.reason == nil then
			rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[1]
			rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[2] 
			cloudCommon.writeJson(rv)
			return nil
		end
	
		local updata = {
			client_info = { connect_status = 0, 
							reconnect_time = attribute.reconnectTime, 
							disconnect_reason = attribute.reason,							
			}
		}

		rv = cloudUciApi.setUciData(CONFIG_STATUS_FILE_NANE, updata)
	else
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[2] 
		cloudCommon.writeJson(rv)
		return nil
	end
	
	cloudCommon.writeJson(rv)

	if exitCloudClientFlag == 1 and rv[ERR_CODE] == 0 then
		os.execute(EXIT_CLOUD_CLIENT)
	end
end

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: checkDeviceId()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	: check cloud device's ID                                                                                   
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]]
function checkDeviceId(deviceId)
	local rv = {}
-- modified by liwei version=1
--	local ubus = require("ubus")
--	local _ubus = ubus.connect()

	if deviceId == nil then
		return false
	end

	local infodata = {
		dev_id = { info = "dev_id" },
	}

	--infoMask, 1:mac, 2:pin, 4:deviceId, 8:hwId, 16:fwCurrentId
--	local args = {infoMask = 4}
--	local devInfo = _ubus:call("tddpServer", "getInfo", args)
	local devInfo = cloudUciApi.getUciData(DEVICE_INFO_FILE_NAME, infodata)

-- end by liwei version=1
	if devInfo.dev_id ~= deviceId then
		return false
	end

	return true
end

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: checkCloudUserName()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	: check cloud user name(cloud-device bound)                                                                                  
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]]
function checkCloudUserName(cloudUserName)
	if cloudUserName == nil then
		return false
	end

	local rv = {}
	local infodata = {
		cloudUserName = { bind = "username" }
	}

	rv = cloudUciApi.getUciData(CLOUD_CONFIG_FILE_NAME, infodata)
	if rv.cloudUserName ~= cloudUserName then
		return false
	end

	return true
end

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: unbindDevice()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	:                                                                                   
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]]
function unbindDevice(unbindPara)
	local rv = {}
	
	if unbindPara == nil then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[2] 
		cloudCommon.writeJson(rv)
		return nil
	end

	local deviceId = unbindPara.deviceId
	local cloudUserName = unbindPara.cloudUserName

	-- check deviceId
	if deviceId == nil or checkDeviceId(deviceId) == false then 
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_DEVICE_ID_ERROR[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_DEVICE_ID_ERROR[2] 
		cloudCommon.writeJson(rv)
		return nil
	end

	-- check cloud username
	if cloudUserName == nil or checkCloudUserName(cloudUserName) == false then 
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_ACCOUNT_NAME_ERROR[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_ACCOUNT_NAME_ERROR[2] 
		cloudCommon.writeJson(rv)
		return nil
	end	

	local updata = {
			bind = {
				"username",
				"password",
			}
		}

	--delete bind information from cloud config file
	local rvUciDelete = cloudUciApi.deleteUciData(CLOUD_CONFIG_FILE_NAME, updata)
	--uci delete failed
	if rvUciDelete ~= 0 then
		cloudCommon.writeJson(rvUciDelete)
		return nil
	end

	local updata2 = {
		bind_status = { bind_status = BIND_STATUS_FAILED }
	}
	
	rv = cloudUciApi.setUciData(CLOUD_CONFIG_FILE_NAME, updata2)
	cloudCommon.writeJson(rv)
end

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: getProtocolVer()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	: get cloud device supported version suite                                                                                   
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]]
function getProtocolVer()
	local result = {}
	local rv = {}
	local arr = {}
	
	table.insert(arr, "V1.0")
	table.insert(arr, "V2.0")

	result.supportedVerionSuite = arr
	
	if rv[ERR_CODE] == nil then
		rv[ERR_CODE] = 0
		rv.result = result
	end
	
	cloudCommon.writeJson(rv)
end

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: setProtocolVer()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	: set communication protocol version suite                                                                                   
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]]
function setProtocolVer()
	local rv = {}

	rv[ERR_CODE] = 0
	cloudCommon.writeJson(rv)
end

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: getCipherSuite()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	: get cloud device supported cipher suite                                                                                   
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]]
function getCipherSuite()
	local rv = {}
	local result = {}
	
	result.supportedCipherSuite = 1
	
	if rv[ERR_CODE] == nil then
		rv[ERR_CODE] = 0
		rv.result = result
	end
	
	cloudCommon.writeJson(rv)
end

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: passthroughHandler()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	: process app request, which passthrough cloud                                                                                  
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]]
function passthroughHandler(jsonRawData)
	if jsonRawData == nil then
		return nil
	end
	return jsonRawData
--[[	
	local jsondata = jsonRawData
	local method = jsondata[luciDs.KEY_METHOD]
	local method_map = {
		[luciDs.METHOD_DO] = luciDs.do_action,
		[luciDs.METHOD_ADD] = luciDs.set_data,
		[luciDs.METHOD_DELETE] = luciDs.set_data,
		[luciDs.METHOD_MODIFY] = luciDs.set_data,
		[luciDs.METHOD_GET] = luciDs.get_data
	}
	
	-- delete method data from jsonRawData, tranmit data to func only
	jsondata[luciDs.KEY_METHOD] = nil
	local func = method_map[method]

	if func then
		local jsonRespData = func(jsondata, method)
 do not modify response data from web-server
		if jsonRespData.err_code ~= 0 then
			jsonRespData = nil
		else
			jsonRespData.err_code = nil
		end

		return jsonRespData
	else
		return nil
	end
--]]
end

--[[ -- data service interface for passthrough                                                                                                                               
******************************************************************************                                                 
* FUNCTION		: passthrough()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	: cloud-server passthrough request data to cloud-device from app                                                                                   
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]]
function passthrough(passthroughPara)
--	local dispatcher = require("luci.dispatcher")
--	local ctx = dispatcher.context
--	ctx.path = {}

	require("luci.http")
	local req = luci.http.Request(
		{}, nil, nil
	)
	luci.http.context.request = req

	-- register call back func
--    if not ctx.datacbs then
 --       dispatcher.create_datacbs() 
--    end

    -- create func tree
--	local c = ctx.tree
--	if not c then
--		dispatcher.createtree()
--	end

	local rv = {}
	
	if passthroughPara == nil then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[2] 
		cloudCommon.writeJson(rv)
		return nil
	end

	local requestData = passthroughPara.requestData
	if requestData == nil then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_PARAMETER_INVALID[2] 
		cloudCommon.writeJson(rv)
		return nil
	end
	
	local respData = passthroughHandler(requestData)
	if respData == nil then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_GENERIC[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_GENERIC[2] 
		cloudCommon.writeJson(rv)
		return nil
	end

	local result = {}
	result.responseData = respData

	if rv[ERR_CODE] == nil then
		rv[ERR_CODE] = 0
		rv.result = result
	end
	
	cloudCommon.writeJson(rv)
end

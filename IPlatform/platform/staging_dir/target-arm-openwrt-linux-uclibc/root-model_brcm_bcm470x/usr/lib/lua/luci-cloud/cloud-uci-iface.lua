--[[
 Copyright (C), 2010-2013, TP-LINK TECHNOLOGIES CO., LTD.
 All rights reserved.

 File name   	: cloud-uci-iface.lua
 Version		: 1.0 
 Description	: cloud uci interface, get/set uci data

 Author     	: Wang FuYu <wangfuyu@tp-link.net>
 Create Date	: 2014-03-21
 
 History    	: 
------------------------------------------------------------

01, 21Mar14, Wang FuYu, create file.
--]]

local uci = require("luci.model.uci")
-- local validator = require("luci.torchlight.validator")
local cloudCommon = require("luci-cloud.cloud-common")
local cloudError = require("luci-cloud.cloud-error")

module("luci-cloud.cloud-uci-iface", package.seeall)

local ERR_CODE = cloudError.ERR_CODE
local ERR_MSG = cloudError.ERR_MSG

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: getUciData()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	: get UCI data from config file                                                                                   
* INPUT			: @uciname:<config>
    			  @keySecOptKwargs = { key = { <section> = <option> }, ...}
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]]
function getUciData(uciname, keySecOptKwargs)
	if type(uciname) ~= "string" or type(keySecOptKwargs) ~= "table" then
		return nil
	end
	
	local rv = {}
	local uciCursor = uci.cursor()

	for key, secOptKwargs in pairs(keySecOptKwargs) do
		for secname, optname in pairs(secOptKwargs) do
			rv[key] = uciCursor:get(uciname, secname, optname)
			--local cmdString = string.format("echo getUCiData pkgName is %s, secName is %s, optName is %s, %s>> /dev/ttyS0", uciname, secname, optname, type(rv[key]))
			--os.execute(cmdString)
		end
	end

	return rv
end

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: setUciData()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	: set UCI data to config file                                                                                   
* INPUT			: @uciname:<config>
    			  @keySecOptKwargs = { key = { <section> = <option> }, ...}
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]]
function setUciData(uciName, keySecOptKwargs)
	local rv = {}

	if type(uciName) ~= "string" or type(keySecOptKwargs) ~= "table" then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_CONFIGURATE_FAILED[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_CONFIGURATE_FAILED[2] 
		return rv
	end
	
	local uciCursor = uci.cursor()

	for secName, OptKwargs in pairs(keySecOptKwargs) do
		for optName, optValue in pairs(OptKwargs) do
			--print("---"..uciName.."---"..secName.."---"..optName.."---"..optValue.."----")
			if not uciCursor:set(uciName, secName, optName, optValue) then
				rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_CONFIGURATE_FAILED[1]
				rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_CONFIGURATE_FAILED[2] 
				return rv
			end
		end
	end

	if not uciCursor:commit(uciName) then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_CONFIGURATE_FAILED[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_CONFIGURATE_FAILED[2] 
		return rv
	end

	if not uciCursor:apply(uciName) then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_GENERIC[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_GENERIC[2] 
		return rv
	end

	--set uci data successed
	if cloudCommon.isEmptyTable(rv) and rv[ERR_CODE] == nil then
		--print("successed, rv is a empty table, i.e.{}")
		rv[ERR_CODE] = 0
	end
	
	return rv
end

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: deleteUciData()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	: delete UCI data from config file                                                                                   
* INPUT			: @uciname:<config>
    			  @keySecOptKwargs = { key = { <section> = <option> }, ...}
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]]
function deleteUciData(uciName, SecOptKwargs)
	local rv = {}

	if type(uciName) ~= "string" or type(SecOptKwargs) ~= "table" then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_CONFIGURATE_FAILED[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_CONFIGURATE_FAILED[2] 
		return rv
	end
	
	local uciCursor = uci.cursor()
	
	for secName, OptKwargs in pairs(SecOptKwargs) do
		for _, optName in pairs(OptKwargs) do
			--print("---"..uciName.."---"..secName.."---"..optName.."---")
			if not uciCursor:delete(uciName, secName, optName) then
				rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_CONFIGURATE_FAILED[1]
				rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_CONFIGURATE_FAILED[2] 
				return rv
			end
		end
	end

	if not uciCursor:commit(uciName) then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_CONFIGURATE_FAILED[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_CONFIGURATE_FAILED[2] 
		return rv
	end

	if not uciCursor:apply(uciName) then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_GENERIC[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_GENERIC[2] 
		return rv
	end

	--delete uci data successed
	if cloudCommon.isEmptyTable(rv) and rv[ERR_CODE] == nil then
		--print("successed, rv is a empty table, i.e.{}")
		rv = 0
	end
	
	return rv
end

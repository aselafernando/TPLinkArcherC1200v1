--[[
 Copyright (C), 2010-2013, TP-LINK TECHNOLOGIES CO., LTD.
 All rights reserved.

 File name   	: cloud-luci.lua
 Version		: 1.0 
 Description	: this script work as communicate interface for cloud service and luci

 Author     	: Wang FuYu <wangfuyu@tp-link.net>
 Create Date	: 2014-02-19
 
 History    	: 
------------------------------------------------------------

01, 19Feb14, Wang FuYu, create file.
--]]

local cloudError = require("luci-cloud.cloud-error")
local cloudRequest = require("luci-cloud.cloud-request")
local cloudCommon = require("luci-cloud.cloud-common")
local jsonlib = require("luci.json")

module_name = "luci-cloud.cloud-luci"
module(module_name, package.seeall)

local ERR_CODE = cloudError.ERR_CODE
local ERR_MSG = cloudError.ERR_MSG

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: run()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	: cloud-luci entry                                                                                   
* INPUT 		:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]]
function run()
	local rv = {}	
	local requestJsonString = os.getenv("requestJsonString")
	if requestJsonString == nil then
		--rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_PARSE_JSON_NULL[1]
		--rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_PARSE_JSON_NULL[2] 
		--cloudCommon.writeJson(rv)
		return nil
	end
		
	local requestJsonTable = jsonlib.decode(requestJsonString)
	if requestJsonTable == nil then
		--rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_PARSE_JSON[1]
		--rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_PARSE_JSON[2] 
		--cloudCommon.writeJson(rv)
		return nil
	end		

	local id = requestJsonTable.id
	if id == nil then
		--rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_PARSE_JSON_ID[1]
		--rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_PARSE_JSON_ID[2] 
		--cloudCommon.writeJson(rv)
		return nil
	end
	
	cloudCommon.REQUEST_ID = id
	
	local methodName = requestJsonTable.method
	if methodName == nil then
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_PARSE_JSON[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_PARSE_JSON[2] 
		cloudCommon.writeJson(rv)
		return nil
	end
	
	local requestParaTable = requestJsonTable.params
	if cloudRequest[methodName] then
		cloudRequest[methodName](requestParaTable)
	else
		rv[ERR_CODE] = cloudError.ERROR_MSG.ERROR_METHOD_NOT_FOUND[1]
		rv[ERR_MSG] = cloudError.ERROR_MSG.ERROR_METHOD_NOT_FOUND[2] 
		cloudCommon.writeJson(rv)
		return nil
	end
end


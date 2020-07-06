--[[
 Copyright (C), 2010-2013, TP-LINK TECHNOLOGIES CO., LTD.
 All rights reserved.

 File name   	: cloud-common.lua
 Version		: 1.0 
 Description	: cloud common module

 Author     	: Wang FuYu <wangfuyu@tp-link.net>
 Create Date	: 2014-02-19
 
 History    	: 
------------------------------------------------------------

01, 19Feb14, Wang FuYu, create file.
--]]

local jsonlib = require("luci.json")
REQUEST_ID = 0

module("luci-cloud.cloud-common", package.seeall)

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: writeJson()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	: create json string, and then write it to pipe                                                                                   
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]]
function writeJson(rv)
	local retJson = rv
	retJson.id = REQUEST_ID
	
	local jsonstr = jsonlib.encode(retJson)
	print(jsonstr)
end

--[[                                                                                                                                
******************************************************************************                                                 
* FUNCTION		: isEmptyTable()                                                                             
* AUTHOR		: Wang FuYu <wangfuyu@tp-link.net>                                                                       
* DESCRIPTION	: checks if a table is empty, i.e. {}                                                                                
* INPUT			:                                                                                                     
* OUTPUT		:                                                                                                      
* RETURN		:                                                                 
******************************************************************************                                                        
--]]
function isEmptyTable(rv)
	return _G.next(rv) == nil
end
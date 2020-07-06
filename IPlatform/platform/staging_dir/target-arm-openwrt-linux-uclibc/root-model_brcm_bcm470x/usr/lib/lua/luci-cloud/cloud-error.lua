--[[
 Copyright (C), 2010-2013, TP-LINK TECHNOLOGIES CO., LTD.
 All rights reserved.

 File name   	: cloud-error.lua
 Version		: 1.0 
 Description	: cloud error message

 Author     	: Wang FuYu <wangfuyu@tp-link.net>
 Create Date	: 2014-02-19
 
 History    	: 
------------------------------------------------------------

01, 19Feb14, Wang FuYu, create file.
--]]

module("luci-cloud.cloud-error", package.seeall)

--Error Table
ERROR_MSG = {
	ERROR_NONE = {0, "Everything is OK"},
	ERROR_GENERIC = {-10000, "Generic error"},
	ERROR_PARSE_JSON = {-10100, "JSON format error"},
	ERROR_PARSE_JSON_NULL = {-10101, "JSON body is NULL"},
	ERROR_EMAIL_FORMAT_ERROR = {-51201, "Email format error"},
	ERROR_EMAIL_LENGTH_ERROR = {-51202, "Email length error"},
	ERROR_EMAIL_PASSWORD_LENGTH_ERROR = {-51203, "Email password length error"},
	ERROR_CLIENT_INTERNAL_ERROR = {-51204, "Cloud client internal error"},
	ERROR_REQUEST_ID_NOT_FOUND = {-51205, "Request id not found"},
	ERROR_METHOD_NOT_FOUND = {-51206, "The method does not exist / is not available"},
	ERROR_PARAMETER_INVALID = {-51207, "Method parameter invalid"},
	ERROR_GET_DATA_FAILED = {-51208, "Get data failed"},
	ERROR_URL_INVALID = {-51209, "URL invalid"},
	ERROR_PASSWORD_INVALID = {-51210, "Password invalid"},
	ERROR_DOWNLOAD_FW_FAILED = {-51211, "Download firmware failed"},
	ERROR_UPGRADE_FW_FAILED = {-51212, "Upgrade firmware failed"},
	ERROR_CONFIGURATE_FAILED = {-51213, "Configurate failed"},
	ERROR_PERMISSION_DENIED = {-51214, "Permission denied"},
	ERROR_REQUEST_TIMEOUT = {-51215, "Timeout"},
	ERROR_MEMORY_OUT = {-51216, "Memory out"},
	ERROR_SEND_REQ_MSG_FAILED = {-51217, "Send request message to cloud failed"},
	ERROR_CONNECTING_TO_CLOUD_SERVER = {-51218, "Connecting to cloud server"},
	ERROR_WAITTING_LAST_REQUEST = {-51219, "Waiting last request"},
	ERROR_ACCOUNT_FORMAT_ERROR = {-51220, "Account format error"},
	ERROR_VERIFY_CODE_FORMAT_ERROR = {-51221, "Verify code format error"},
	ERROR_NEW_PASSWORD_INVALID = {-51222, "New password invalid"},
	ERROR_ACCOUNT_NAME_ERROR = {-51223, "Account name error"},
	ERROR_DEVICE_ID_ERROR = {-51224, "Device id error"},
	ERROR_MSG_ID_MAX = {-51000, NULL}
}

ERR_CODE = "error_code"
ERR_MSG = "msg"


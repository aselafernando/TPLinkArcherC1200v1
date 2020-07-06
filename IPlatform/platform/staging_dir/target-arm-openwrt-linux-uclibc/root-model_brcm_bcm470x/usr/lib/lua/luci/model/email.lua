--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  email.lua
Details :  Api for email sending.
Author  :  Zhang Zhongwei <zhangzhongwei@tp-link.net>
Version :  1.0.0
Date    :  03 Apr, 2014
]]--

module("luci.model.email", package.seeall)


local dbg     = require "luci.tools.debug"
local smtp    = require "socket.smtp"
local smtps   = require "luci.model.smtps"
local ubus    = require "ubus"
local uci_r   = require("luci.model.uci").cursor()

local MODEL   = uci_r:get("locale", "sysinfo", "model") or "ArcherC9"

function check_not_nil(params)
    if not params["from"] or not params["rcpt"]
        or not params["server"]
    then
        return false
    end

    if (params["user"] and not params["password"])
        or (not params["user"] and params["password"])
    then
        return false
    end
    return true
end

-- Change "XX@XX.com" to "<XX@XX.com>" format
function format_addr(addr)   
    if not string.find(addr or "", ".*<.*>.*") then
        addr = "<" .. addr .. ">"
    end
    return addr
end

function check_for_tls(params)
    if string.find(params["from"]:lower() or "", "hotmail") then
        params["port"] = 587

    elseif string.find(params["from"]:lower() or "", "gmail") 
        or string.find(params["from"]:lower() or "", "yahoo")
    then
        params["port"] = 465
        params["create"] = smtps.tcp(params)
    end
    return params
end

function format_params(params)
    if type(params) ~= "table" or not check_not_nil(params) then
        return false
    end

    params["domain"] = luci.sys.hostname() or MODEL
    params["from"]   = format_addr(params["from"])
    if type(params["rcpt"]) == "table" then
        for k, _ in pairs(params["rcpt"]) do
            params["rcpt"][k] = format_addr(params["rcpt"][k])
        end
    else
        params["rcpt"] = format_addr(params["rcpt"])
    end

    params = check_for_tls(params)
    return params
end

function format_source(params, source)
    if type(source) ~= "table" or not source["headers"] 
        or not source["body"] 
    then
        return false
    end

    source["headers"]["from"]     = MODEL .. " " .. params["from"]
    source["headers"]["to"]       = params["rcpt"]
    source["headers"]["X-Mailer"] = "webMail"   -- for sina

    return source
end

function check_network()
    local _ubus = ubus.connect()

    if _ubus then
        local data = _ubus:call("network.interface.wan", "status", {})
        if data then
            if data["linkstate"] and data["state"] == "connected" then
                return true
            end
        end
    end
    return false
end

function send(mailt, source)
    if not check_network() then
        return false, "Not avaliable network"
    end    

    mailt = format_params(mailt)
    if not mailt then 
        return false, "Invalid email addresses!"
    end

    source = format_source(mailt, source)
    if not source then 
        return false, "Invalid message!"
    end
    mailt["source"] = smtp.message(source)

    -- Send the email
    local ret, err = smtp.send(mailt)

    return ret, err
end

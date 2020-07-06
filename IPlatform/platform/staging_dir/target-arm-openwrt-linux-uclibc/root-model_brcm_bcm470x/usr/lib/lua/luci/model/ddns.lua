--[[
Copyright(c) 2013 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  ddns.lua
Details :  model for dynamic dns 
Author  :  Guo Dongxian <guodongxian@tp-link.net>
Version :  1.0.1
Date    :  09April, 2014
]]--

module("luci.model.ddns", package.seeall)

local sys = require "luci.sys"
local uci = require "luci.model.uci"
local utl = require "luci.util"

DDNS_SHELL = "/usr/sbin/ddns "

function dbg(str)
    sys.call("echo %s >/dev/console 2>&1" % ("[debug]:" .. str))
end

function err(str)
    sys.call("echo %s >/dev/console 2>&1" % ("[error]:" .. str))
end

--- If network changed connection type, then need to call the api
--- The function will change source of ip address
-- @param null
function change_iface()
    local uci_c = uci.cursor()
    
    local wan_type = uci_c:get("network", "wan", "wan_type")
    
    local iface
    if wan_type == "static" or wan_type == "dhcp" then
        iface = "wan"
    else
        iface = "internet"
    end
    -- Update dyndns 
    uci_c:set("ddns", "dyndns", "interface", iface)
    uci_c:set("ddns", "dyndns", "ip_network", iface)
    -- Update noip 
    uci_c:set("ddns", "noip", "interface", iface)
    uci_c:set("ddns", "noip", "ip_network", iface)

    uci_c:commit("ddns")
end

DDNS_INST = utl.class()

function DDNS_INST:__init__()
    self.uci = uci.cursor()
    self.module = "ddns"
    -- Dynamic dns provider, dyndns/noip, of course we can support other provdiers
    self.provider = { "dyndns", "noip" }
end

function DDNS_INST:exec(cmd)
    os.execute(DDNS_SHELL .. cmd)
end

function DDNS_INST:dup_check(provider, info)
    local old = self.uci:get_all(self.module, provider)
    local status = self:get_status(provider)
    if not old then
        return true   
    end
    -- Preventing login frequently
    if old.enabled == "on" and info.username == old.username 
        and info.password == old.password
            and info.domain == old.domain 
                and info.check_interval == old.check_interval
                    and info.check_unit == old.check_unit
			and ( status == 0 or status == 2)
    then
        if provider == "noip" and  info.wan_bind ~= old.wan_bind then
            return false
        end
        return true        
    end
    
    return false
end

function DDNS_INST:login(provider)
    -- Turn on
    self.uci:set(self.module, provider, "enabled", "on")
    self.uci:commit(self.module)
   
    self:exec("stop " .. provider)
    self:exec("start " .. provider)
end

function DDNS_INST:logout(provider)
    -- Turn off
    self.uci:set(self.module, provider, "enabled", "off")
    self.uci:commit(self.module)
   
    self:exec("stop " .. provider)
end

function DDNS_INST:get_status(provider)
    local state = sys.exec(DDNS_SHELL .. "state " .. tostring(provider))
    -- Return state code
    return tonumber(state)
end

function DDNS_INST:logined(provider)
    local logined = sys.exec(DDNS_SHELL .. "logined " .. tostring(provider))
    -- Return service logined or not
    return tonumber(logined)
end

--- Get ddns information from config file
function DDNS_INST:get(provider)
    local res = {
        username = "",
        password = "",
        domain = "",
        update_time = "",
        logined = 0,
        status = false
    }

    for _, p in ipairs(self.provider) do
        if p == provider then
            local chk_int  = self.uci:get(self.module, p, "check_interval")
            
            if chk_int and chk_int ~= "never" then
                local chk_unit = self.uci:get(self.module, p, "check_unit")
                local units = { seconds = "s", minutes = "m", hours = "h", days = "d" }
                local u = units[chk_unit]
                if not u then return false end
                if u == "s" then 
                    res.update_time = tostring(chk_int / (60 * 60)) .. "h"
                elseif u == "m" then 
                    res.update_time = tostring(chk_int / 60) .. "h"
                else
                    res.update_time = tostring(chk_int) .. u
                end
            else
                res.update_time = "never"
            end
            res.username = self.uci:get(self.module, p, "username") or ""
            res.password = self.uci:get(self.module, p, "password") or ""
            res.domain = self.uci:get(self.module, p, "domain") or ""
            -- Get state of ddns
            res.status = self:get_status(p)
            -- Get ddns logined or not
            res.logined = self:logined(p)
            if p == "noip" then 
                res.wan_bind = self.uci:get(self.module, p, "wan_bind") or "disable"
            end
        end
    end

    return res
end

--- Save ddns information to config file
function DDNS_INST:set(provider, info)
    if type(info) ~= "table" then
        return false
    end
    
    for _, p in ipairs(self.provider) do
        if p == provider then
            local old = self:get(p)
            self.uci:set(self.module, p, "username", info.username or old.username)
            self.uci:set(self.module, p, "password", info.password or old.password)
            self.uci:set(self.module, p, "domain", info.domain or old.domain)

            local chk_int  = self.uci:get(self.module, p, "check_interval")
            local chk_unit = self.uci:get(self.module, p, "check_unit")
            self.uci:set(self.module, p, "check_interval", info.check_interval or chk_int)
            self.uci:set(self.module, p, "check_unit", info.check_unit or chk_unit)
            self.uci:set(self.module, p, "force_interval", info.check_interval or chk_int)
            self.uci:set(self.module, p, "force_unit", info.check_unit or chk_unit)            
            if p == "noip" then 
                self.uci:set(self.module, p, "wan_bind", info.wan_bind or old.wan_bind)
            end
            self.uci:commit(self.module)       
        end    
    end
    -- Set succeed, then return UI original value and ddns state
    info.status = self:get_status(provider)
    info.logined = self:logined(provider)
    return info
end

function DDNS_INST:status_refresh(provider)
    return self:get_status(provider)
end

--- Dynamic dns default config set, it can not display or operation for UI
function DDNS_INST:set_default(section, option, value)
    self.uci:set(self.module, section, option, value)
    self.uci:commit(self.module)
end

--- Set service name
function DDNS_INST:svrname_set(section, value)
    self:set_default(section, "service_name", value)
end

--- Set force interval
function DDNS_INST:force_interval_set(section, value)
    self:set_default(section, "force_interval", value)
end

--- Set force unit
function DDNS_INST:force_unit_set(section, value)
    self:set_default(section, "force_unit", value)
end

--- Set check interval
function DDNS_INST:check_interval_set(section, value)
    self:set_default(section, "check_interval", value)
end

--- Set check unit
function DDNS_INST:check_unit_set(section, value)
    self:set_default(section, "check_unit", value)
end

--- Set retry interval
function DDNS_INST:retry_interval_set(section, value)
    self:set_default(section, "retry_interval", value)
end

--- Set retry unit
function DDNS_INST:retry_unit_set(section, value)
    self:set_default(section, "retry_unit", value)
end


--[[
Copyright(c) 2013 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  security_settings.lua
Details :  controller for security_settings.html webpage (basic_security & dos_protection module)
Author  :  Hu Luyao <huluyao@tp-link.net>
Version :  1.0.0
Date    :  24Mar, 2014
]]--
module("luci.controller.admin.security_settings", package.seeall)

local uci               = require "luci.model.uci"
local sys               = require "luci.sys" 
local dbg               = require "luci.tools.debug"
local util              = require "luci.util"
local ctl               = require "luci.model.controller"
local uci_r = uci.cursor()
require "luci.json"

local function custom_delete(cfg, stype, key, index, user_mac)
    if not cfg or not stype or not key or not index then
        return false
    end
    
    local keys  = (type(key)=="table") and key or {key}
    local index = (type(index)=="table") and index or {index}
    local items = {}

    uci_r:set_confdir("/var/state/")
    uci_r:foreach(cfg, stype,
        function(section)
            items[#items + 1] = section[".name"]
        end
    )   
    
    local rets = {}
    for k, v in ipairs(index) do
        local tmp = false
        if items[v+1] ~= nil then
            tmp = uci_r:delete(cfg, items[v+1]) and true or false
        end

        local ret = {
            key     = keys[k],
            index   = v,
            success = tmp
        }
        rets[#rets+1] = ret
    end

    uci_r:commit(cfg)
    uci_r:set_confdir("/etc/config/")
    return rets
end
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
function read_security_settings_spi_enable()
    local ret = {enable = uci_r:get("basic_security", "settings", "spi_enable")}
    return ret
end

-- Public API for other modules call
-- Start/Stop SPI Firewall
function security_spi_operation(on_off)
    local u = uci.cursor()
    local enable = u:get("basic_security", "settings", "spi_enable")

    on_off = on_off and on_off or "on"    

    if enable ~= on_off then
        u:set("basic_security", "settings", "spi_enable", on_off)
        u:commit("basic_security")
        -- Reload, make SPI Rule work at once
        sys.fork_exec("/etc/init.d/basic_security reload")
    end
end

function write_security_settings_spi_enable(http_form)
    uci_r:set("basic_security", "settings", "spi_enable", http_form.enable)
    uci_r:commit("basic_security")
    return read_security_settings_spi_enable()
end
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
function read_dos_para_settings()
    local ret = {}
    ret.syn_high   = uci_r:get("dos_protection", "settings", "syn_high")
    ret.syn_middle = uci_r:get("dos_protection", "settings", "syn_middle")
    ret.syn_low    = uci_r:get("dos_protection", "settings", "syn_low")

    ret.udp_high   = uci_r:get("dos_protection", "settings", "udp_high")
    ret.udp_middle = uci_r:get("dos_protection", "settings", "udp_middle")
    ret.udp_low    = uci_r:get("dos_protection", "settings", "udp_low")

    ret.icmp_high   = uci_r:get("dos_protection", "settings", "icmp_high")
    ret.icmp_middle = uci_r:get("dos_protection", "settings", "icmp_middle")
    ret.icmp_low    = uci_r:get("dos_protection", "settings", "icmp_low")

    return ret
end

function write_dos_para_settings(http_form)
    local ret = {}
  
    local syn_high   = http_form.syn_high
    local syn_middle = http_form.syn_middle
    local syn_low    = http_form.syn_low

    local udp_high   = http_form.udp_high
    local udp_middle = http_form.udp_middle
    local udp_low    = http_form.udp_low

    local icmp_high   = http_form.icmp_high
    local icmp_middle = http_form.icmp_middle
    local icmp_low    = http_form.icmp_low

    local syn_level  = uci_r:get("dos_protection", "settings", "syn_level")
    local udp_level  = uci_r:get("dos_protection", "settings", "udp_level")
    local icmp_level = uci_r:get("dos_protection", "settings", "icmp_level")

    local syn_curr
    local udp_curr
    local icmp_curr

    if syn_level == 'high' then
        syn_curr = syn_high
    elseif syn_level == 'middle' then
        syn_curr = syn_middle
    elseif syn_level == 'low' then
        syn_curr = syn_low 
    else
        syn_curr = 0 
    end

    if udp_level == 'high' then
        udp_curr = udp_high
    elseif udp_level == 'middle' then
        udp_curr = udp_middle
    elseif udp_level == 'low' then
        udp_curr = udp_low
    else
        udp_curr = 0 
    end

    if icmp_level == 'high' then
        icmp_curr = icmp_high
    elseif icmp_level == 'middle' then
        icmp_curr = icmp_middle
    elseif icmp_level == 'low' then
        icmp_curr = icmp_low 
    else
        icmp_curr = 0
    end

    uci_r:set("dos_protection", "settings", "syn_high", syn_high)
    uci_r:set("dos_protection", "settings", "syn_middle", syn_middle)
    uci_r:set("dos_protection", "settings", "syn_low", syn_low)
    uci_r:set("dos_protection", "settings", "syn_curr", syn_curr)

    uci_r:set("dos_protection", "settings", "udp_high", udp_high)
    uci_r:set("dos_protection", "settings", "udp_middle", udp_middle)
    uci_r:set("dos_protection", "settings", "udp_low", udp_low)
    uci_r:set("dos_protection", "settings", "udp_curr", udp_curr)

    uci_r:set("dos_protection", "settings", "icmp_high", icmp_high)
    uci_r:set("dos_protection", "settings", "icmp_middle", icmp_middle)
    uci_r:set("dos_protection", "settings", "icmp_low", icmp_low)
    uci_r:set("dos_protection", "settings", "icmp_curr", icmp_curr)

    uci_r:commit("dos_protection") 
    return read_dos_para_settings()
end


-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
function read_security_settings_enable()
    local ret = {enable = uci_r:get("dos_protection", "settings", "enable")}
    return ret
end

function write_security_settings_enable(http_form)
    uci_r:set("dos_protection", "settings", "enable", http_form.enable)
    uci_r:commit("dos_protection")
    return read_security_settings_enable()
end
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
function read_security_settings_dos()
    local ret = {}
    ret.icmp_level = uci_r:get("dos_protection", "settings", "icmp_level")
    ret.syn_level  = uci_r:get("dos_protection", "settings", "syn_level")
    ret.udp_level  = uci_r:get("dos_protection", "settings", "udp_level")
    return ret
end

function write_security_settings_dos(http_form)
    local icmp_level = http_form.icmp_level
    local syn_level  = http_form.syn_level
    local udp_level  = http_form.udp_level
    local syn_curr
    local udp_curr
    local icmp_curr

    if syn_level == 'high' then
        syn_curr = uci_r:get("dos_protection", "settings", "syn_high")
    elseif syn_level == 'middle' then
        syn_curr = uci_r:get("dos_protection", "settings", "syn_middle")
    elseif syn_level == 'low' then
        syn_curr = uci_r:get("dos_protection", "settings", "syn_low")
    else
        syn_curr = 0 
    end

    if udp_level == 'high' then
        udp_curr = uci_r:get("dos_protection", "settings", "udp_high")
    elseif udp_level == 'middle' then
        udp_curr = uci_r:get("dos_protection", "settings", "udp_middle")
    elseif udp_level == 'low' then
        udp_curr = uci_r:get("dos_protection", "settings", "udp_low")
    else
        udp_curr = 0 
    end

    if icmp_level == 'high' then
        icmp_curr = uci_r:get("dos_protection", "settings", "icmp_high")
    elseif icmp_level == 'middle' then
        icmp_curr = uci_r:get("dos_protection", "settings", "icmp_middle")
    elseif icmp_level == 'low' then
        icmp_curr = uci_r:get("dos_protection", "settings", "icmp_low")
    else
        icmp_curr = 0
    end

    uci_r:set("dos_protection", "settings", "icmp_level", icmp_level)
    uci_r:set("dos_protection", "settings", "syn_level", syn_level)
    uci_r:set("dos_protection", "settings", "udp_level", udp_level)

    uci_r:set("dos_protection", "settings", "syn_curr", syn_curr)
    uci_r:set("dos_protection", "settings", "udp_curr", udp_curr)
    uci_r:set("dos_protection", "settings", "icmp_curr", icmp_curr)

    uci_r:commit("dos_protection")
    return read_security_settings_dos()
end
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------

function read_security_settings_ping()
    local ret = {}
    ret.wan_ping = uci_r:get("basic_security", "settings", "wan_ping")
    ret.lan_ping = uci_r:get("basic_security", "settings", "lan_ping")
    return ret
end

function write_security_settings_ping(http_form)
    uci_r:set("basic_security", "settings", "wan_ping", http_form.wan_ping)
    uci_r:set("basic_security", "settings", "lan_ping", http_form.lan_ping)
    uci_r:commit("basic_security")
    return read_security_settings_ping()
end
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
function read_security_settings_list()
    local list = {}

    uci_r:set_confdir("/var/state/")
    uci_r:foreach("dos_protection", "dos_entry",
        function(section)
            list[#list + 1] = uci_r:get_all("dos_protection", section[".name"]) 
            list[#list].mac = list[#list].mac:gsub(":", "-"):upper()
        end
    )
    uci_r:set_confdir("/etc/config/")

    return list
end

function remove_security_settings_list(http_form)
    local ret  = {}
    local key   = http_form.key
    local index = http_form.index
    ret = custom_delete("dos_protection", "dos_entry", key, index)

    return ret

end

-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
local dispatch_tbl = {
    spi_enable = {
        ["read"]  = { cb  = read_security_settings_spi_enable },
        ["write"] = { cb  = write_security_settings_spi_enable,
                      cmd = "/etc/init.d/basic_security reload" }        
    },

    enable = {
        ["read"]  = { cb  = read_security_settings_enable },
        ["write"] = { cb  = write_security_settings_enable,
                      cmd = "/etc/init.d/dos_protection reload" }        
    },

    dos = {
        ["read"]  = { cb  = read_security_settings_dos },
        ["write"] = { cb  = write_security_settings_dos,
                      cmd = "/etc/init.d/dos_protection reload" }        
    },

    ping = {
        ["read"]  = { cb  = read_security_settings_ping },
        ["write"] = { cb  = write_security_settings_ping,
                      cmd = "/etc/init.d/basic_security reload" }        
    },

    list = {
        ["load"]   = { cb  = read_security_settings_list },
        ["remove"] = { cb  = remove_security_settings_list,
                      cmd = "/etc/init.d/dos_protection reload" }        
    },

    dos_setting = {
        ["read"]  = { cb  = read_dos_para_settings },
        ["write"] = { cb  = write_dos_para_settings,
                      cmd = "/etc/init.d/dos_protection reload" }        
    }
}

function dispatch(http_form)
    local cmds = {}
    local function hook_cb(success, action)
        if success and action.cmd then
            cmds[action.cmd] = true
        end
        return true
    end
    local result = ctl.dispatch(dispatch_tbl, http_form, {post_hook = hook_cb})
    for cmd, _ in pairs(cmds) do
        sys.fork_exec(cmd)
    end
    return result
end

function _index()
    return ctl._index(dispatch)
end

function index()
    entry({"admin", "security_settings"}, call("_index")).leaf = true
end
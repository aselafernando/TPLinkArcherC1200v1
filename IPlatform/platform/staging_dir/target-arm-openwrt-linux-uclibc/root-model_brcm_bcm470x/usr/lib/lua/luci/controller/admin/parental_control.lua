--[[
Copyright(c) 2013 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  parental_control.lua
Details :  controller for parental_control.html webpage
Author  :  Hu Luyao <huluyao@tp-link.net>
Version :  1.0.0
Date    :  24Mar, 2014
]]--
module("luci.controller.admin.parental_control", package.seeall)

local ubus              = require "ubus"
local uci               = require "luci.model.uci"
local sys               = require "luci.sys" 
local form              = require "luci.tools.form"
local dbg               = require "luci.tools.debug"
local ctl               = require "luci.model.controller"

local uci_r = uci.cursor()
form = form.Form(uci_r, {"mac"})
require "luci.json"

local function get_user_mac()
    local user_ip = sys.getenv("REMOTE_ADDR") -- get user ipaddr from uhttpd
    local user_mac

    for _, v in ipairs(luci.sys.net.arptable()) do
        if user_ip and user_ip == v["IP address"] then
            user_mac = v["HW address"]
            break
        end
    end

    if user_mac == nil then
        user_mac = "00-00-00-00-00-00"
    end

    return user_mac
end
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
function read_parental_control_settings()
    local ret = {enable = uci_r:get("parental_control", "settings", "enable"), 
                 host_mac = get_user_mac():gsub(":", "-"):upper(),
				 max_client = uci_r:get_profile("parental_control", "max_dev")}
    return ret
end

function write_parental_control_settings(http_form)
    uci_r:set("parental_control", "settings", "enable", http_form.enable)
    uci_r:commit("parental_control")
    return read_parental_control_settings()
end
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
function load_device()
    local device_list = {}

    uci_r:foreach("parental_control", "device",
        function(section)
            device_list[#device_list + 1] = uci_r:get_all("parental_control", section[".name"]) 
            device_list[#device_list].mac = device_list[#device_list].mac:gsub(":", "-"):upper()
            if device_list[#device_list].calendar == nil then
                device_list[#device_list].calendar = "{}"
            end
        end
    )

    return device_list
end

function tmp_load_device()
    local device_list = {}

    uci_r:foreach("parental_control", "device",
        function(section)
            device_list[#device_list + 1] = uci_r:get_all("parental_control", section[".name"]) 
            device_list[#device_list].mac = device_list[#device_list].mac:gsub(":", "-"):upper()
            if device_list[#device_list].calendar == nil then
                device_list[#device_list].calendar = "{}"    
            end
            device_list[#device_list].calendar = lua_time_to_tmp_time(device_list[#device_list].calendar)
        end
    )

    return device_list
end

function get_max_dev()
    local others = {}
    others.max_rules = uci_r:get_profile("parental_control", "max_dev")
    return others
end

function insert_device(http_form)
    -- get data
    local ret = {}
    local new  = luci.json.decode(http_form.new)

    local user_mac = get_user_mac():gsub("-", ":"):upper()
    new.mac  = (new.mac):gsub("-", ":"):upper()

    local count = form:count("parental_control", "device")
    local max_count = uci_r:get_profile("parental_control", "max_dev")

    if new.mac ~= user_mac then
        if count < max_count then
            ret = form:insert("parental_control", "device", new)
            if ret then
                ret.mac = (ret.mac):gsub(":", "-")
                ret.success = true
            else
                ret.success = false
            end
        end
    end

    uci_r:commit("parental_control")
    return ret

end

function update_device(http_form)
    local ret = false
    local old = luci.json.decode(http_form.old)
    local new = luci.json.decode(http_form.new)
  
    local user_mac = get_user_mac():gsub("-", ":"):upper()
    new.mac = (new.mac):gsub("-", ":"):upper()
    old.mac = (old.mac):gsub("-", ":"):upper()

    if new.mac ~= user_mac then
        ret = form:update("parental_control", "device", old, new)
        if ret then
            ret.mac = (ret.mac):gsub(":", "-")
        end
    end

    uci_r:commit("parental_control")
    return ret
end

function remove_device(http_form)
    local key   = http_form.key
    local index = http_form.index
    local ret   = form:delete("parental_control", "device", key, index)
    
    uci_r:commit("parental_control")
    return ret

end

function lua_time_to_tmp_time(calendar)
    local tsched = require "tsched_conf"
    return tsched.calendar_to_bytestring(calendar)
end
function tmp_time_to_lua_time(calendar)
    local tsched = require "tsched_conf"
    return tsched.bytestring_to_calendar(calendar)
end

function tmp_insert_device(lua_form)
    local ret = {}
    lua_form.calendar = tmp_time_to_lua_time(lua_form.calendar)
    local new = lua_form

    local user_mac = get_user_mac():gsub("-", ":"):upper()
    new.mac  = (new.mac):gsub("-", ":"):upper()

    local count = form:count("parental_control", "device")
    local max_count = uci_r:get_profile("parental_control", "max_dev")

    if new.mac ~= user_mac then
        if count < max_count then
            ret = form:insert("parental_control", "device", new)
            if ret then
                ret.mac = (ret.mac):gsub(":", "-")
                ret.success = true
            else
                ret.success = false
            end
        end
    end

    uci_r:commit("parental_control")

    return ret.success
end

function tmp_update_device(lua_form)
    local ret = {}
    lua_form.calendar = tmp_time_to_lua_time(lua_form.calendar)
    lua_form.mac = lua_form.mac:gsub(":", "-"):upper()

    uci_r:foreach("parental_control", "device",
        function(section)
            old_dev = uci_r:get_all("parental_control", section[".name"])
            old_dev.mac = old_dev.mac:gsub(":", "-"):upper()
            if old_dev.mac == lua_form.mac then
                ret.old = old_dev
                ret.new = lua_form
            end
        end
    )

    if ret.new ~= nil then
        dbg.dumptable(ret)
        
        local user_mac = get_user_mac():gsub("-", ":"):upper()
        ret.new.mac = (ret.new.mac):gsub("-", ":"):upper()
        ret.old.mac = (ret.old.mac):gsub("-", ":"):upper()

        if ret.new.mac ~= user_mac then
            ret = form:update("parental_control", "device", ret.old, ret.new)
            if ret then
                ret.mac = (ret.mac):gsub(":", "-")
            end
        end

        uci_r:commit("parental_control")
        return true
    else
        return false
    end
end

function tmp_remove_device(lua_form)
    lua_form.mac = lua_form.mac:gsub(":", "-"):upper()
    uci_r:delete_all("parental_control", "device",
        function(section)
            old_dev = uci_r:get_all("parental_control", section[".name"])
            old_dev.mac = old_dev.mac:gsub(":", "-"):upper()
            return old_dev.mac == lua_form.mac
        end
    )
    uci_r:commit("parental_control")
    return true
end


-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
function read_parental_control_mode()
    local ret = {}
    local access_mode = uci_r:get("parental_control", "settings", "access_mode")

    local ret_black = "["
    local ret_white = "["
    
    local black = uci_r:get_list("parental_control", "black_list", "key")
    local white = uci_r:get_list("parental_control", "white_list", "url")

    for _, rt in ipairs(black) do
        ret_black = ret_black .. "\""..rt.. "\"".. ','
     
    end
    ret_black = ret_black .. ']'
    ret_black = ret_black:gsub(",]", "]")

    for _, rt in ipairs(white) do
        ret_white = ret_white .. "\""..rt.. "\"".. ','
    end
    ret_white = ret_white .. ']'
    ret_white = ret_white:gsub(",]", "]")

    ret.access_mode = access_mode or "black"
    ret.black = ret_black or ""
    ret.white = ret_white or ""

    local others = {}
    others.max_rules_black = uci_r:get_profile("parental_control", "max_key")
    others.max_rules_white = uci_r:get_profile("parental_control", "max_url")

    -- warning: mode & urls is not a form, here is specicl "others" in data limited by UI
    ret.others = others
    
    return ret
end

function update_parental_control_list(http_form)
    local ret   = {}
    local access_mode       = http_form.access_mode
    local http_black_list   = http_form.black
    local http_white_list   = http_form.white
    local black_list        = luci.json.decode(http_black_list)
    local white_list        = luci.json.decode(http_white_list)
    local max_count         = uci_r:get_profile("parental_control", "max_url")
    local max_len           = 255
    local len_flag          = 1
   
    if black_list ~= nil then 
        num_black_list = #black_list
        -- check profile limit 
        for _, v in ipairs(black_list) do
            if string.len(v) > max_len then
                len_flag = 0
            end
        end

        if (num_black_list <= max_count) and (len_flag == 1) then
            uci_r:set("parental_control", "settings", "access_mode", access_mode)
            uci_r:delete("parental_control", "black_list", "key")

            if num_black_list > 0 then
                uci_r:set_list("parental_control", "black_list", "key", black_list)        
            end
        end

    elseif white_list ~= nil then
        num_white_list = #white_list
        for _, v in ipairs(white_list) do
            if string.len(v) > max_len then
                len_flag = 0
            end
        end

        if (num_white_list <= max_count) and (len_flag == 1) then
            uci_r:set("parental_control", "settings", "access_mode", access_mode)
            uci_r:delete("parental_control", "white_list", "url")

            if num_white_list > 0 then
                uci_r:set_list("parental_control", "white_list", "url", white_list)
            end
        end

    else
        return false, "can not get data from http"
    end
    
    ret.access_mode = access_mode or "off"
    ret.black = http_black_list
    ret.white = http_white_list
    uci_r:commit("parental_control")
    return ret
end

-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------

local dispatch_tbl = {
    enable = {
        ["read"]  = { cb  = read_parental_control_settings },
        ["write"] = { cb  = write_parental_control_settings,
                      cmd = "/etc/init.d/parental_control reload" }        
    },

    mode = {
        ["read"]  = { cb  = read_parental_control_mode },
        ["write"] = { cb  = update_parental_control_list,
                      cmd = "/etc/init.d/parental_control reload" }        
    },

    device = {
        ["load"]   = { cb  = load_device,
                       others = get_max_dev },
        ["update"] = { cb  = update_device,
                       cmd = "/etc/init.d/parental_control reload" },
        ["insert"] = { cb  = insert_device,
                       cmd = "/etc/init.d/parental_control reload" }, 
        ["remove"] = { cb  = remove_device,
                       cmd = "/etc/init.d/parental_control reload" },

        ["tmp_load"]   = { cb  = tmp_load_device },
        ["tmp_update"]   = { cb  = tmp_update_device, 
                             cmd = "/etc/init.d/parental_control reload" },
        ["tmp_insert"]   = { cb  = tmp_insert_device,
                             cmd = "/etc/init.d/parental_control reload" },
        ["tmp_remove"]   = { cb  = tmp_remove_device,
                             cmd = "/etc/init.d/parental_control reload" }
    }
}

function dispatch(http_form)
    local function hook_cb(success, action)
        if success and action.cmd then
            sys.fork_exec(action.cmd)
        end
        return true
    end
    return ctl.dispatch(dispatch_tbl, http_form, {post_hook = hook_cb})
end

function _index()
    return ctl._index(dispatch)
end

function index()
    entry({"admin", "parental_control"}, call("_index")).leaf = true
end
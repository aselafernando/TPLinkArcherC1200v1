--[[
Copyright(c) 2013 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  qos.lua
Details :  controller for qos.html webpage
Author  :  Hu Luyao <huluyao@tp-link.net>
Version :  1.0.0
Date    :  24Mar, 2014
]]--
module("luci.controller.admin.qos", package.seeall)

local ubus              = require "ubus"
local uci               = require "luci.model.uci"
local sys               = require "luci.sys" 
local form              = require "luci.tools.form"
local dbg               = require "luci.tools.debug"
local ctl               = require "luci.model.controller"

local uci_r = uci.cursor()
form = form.Form(uci_r, {"mac","proto","port","ipaddr","phy","app","type"})
require "luci.json"

local function get_mtd( part_name )
end 

local function mac_to_ip( mac )
    -- get ip from arp table by mac
    mac = mac:gsub("-", ":"):lower()
    for _, v in ipairs(luci.sys.net.arptable()) do
        if mac and mac == v["HW address"] then
            return v["IP address"]
        end
    end

    return "none"
end

local function http_to_uci( new , rule_num)
    local ret = {type = "", priority = new.priority, enable = "on"}

    if new.method == "device" then
        ret.type = ret.type .. "mac "
        ret.dev_name = new.device_name
        ret.mac = new.mac
    end

    if rule_num ~= nil and rule_num ~= "" then
        if rule_num ~= "custom" then
            ret.app = ""
            ret.type = ret.type .. "app "
            uci_r:foreach("qos", "app",
                        function(section)
                            local uci_app = uci_r:get_all("qos", section[".name"])
                            if string.find(rule_num, uci_app.id) ~= nil then
                                ret.app = ret.app .. uci_app.name .. " "
                            end
                        end
                    )
        end

        -- custom
        if rule_num == "custom" then
            ret.type = ret.type .. "custom "
            ret.port = new.port
            ret.name = new.custom_name
            ret.proto = new.proto
        end
    end

    if new.method == "phy" then
        ret.type = ret.type .. "phy "
        ret.phy = new.phy
    end

    return ret
end

local function uci_to_http( cfg )
    local ret = {type = "", priority = cfg.priority}
    if cfg.type ~= nil then
        if string.find(cfg.type, "mac") ~= nil then
            ret.type = "device"
            ret.mac = cfg.mac
            ret.name = cfg.dev_name
            ret.ip = mac_to_ip(cfg.mac)
        end 
        if string.find(cfg.type, "phy") ~= nil then
            ret.type = "port"
            ret.phy = cfg.phy
        end

        if string.find(cfg.type, "app") ~= nil then
            ret.type = "app"
            ret.app = ""
            if cfg.app ~= nil then
                uci_r:foreach("qos", "app",
                    function(section)
                        local uci_app = uci_r:get_all("qos", section[".name"])
                        if string.find(cfg.app, uci_app.name .. " ") ~= nil then
                            ret.app = ret.app .. uci_app.id .. ','
                        end
                    end
                )

                if ret.app ~= "" and ret.app ~= nil then
                    ret.app = string.sub(ret.app, 1, -2)
                end
            end
        end

        if string.find(cfg.type, "custom") ~= nil then
            ret.type = "app"
            ret.custom = "on"
            ret.name = cfg.name
            ret.proto = cfg.proto
            ret.port = cfg.port
        end
    end

    return ret
end

-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
function read_qos_settings()
    local ret = {}
    ret.enable    = uci_r:get("qos", "settings", "enable") or "off"
    ret.up_band   = uci_r:get("qos", "settings", "up_band") or ""
    ret.down_band = uci_r:get("qos", "settings", "down_band") or ""
    ret.up_unit   = uci_r:get("qos", "settings", "up_unit") or "kbps"
    ret.down_unit = uci_r:get("qos", "settings", "down_unit") or "kbps"
    ret.high      = uci_r:get("qos", "settings", "high") or "60"
    ret.middle    = uci_r:get("qos", "settings", "middle") or "30"
    ret.low       = uci_r:get("qos", "settings", "low") or "10"
    ret.time      = uci_r:get("qos", "settings", "time") or "10"
    ret.enable_phy= uci_r:get_profile("qos", "by_phy") ~= 0 and "on" or "off"
    ret.enable_app= uci_r:get_profile("qos", "by_app") ~= 0 and "on" or "off"
    
    return ret
end

function write_qos_settings(http_form)
    local ret = {}
    local settings = {"enable", "up_band", "down_band", "up_unit", "down_unit", "high", "middle", "low"}
   
    for _, set in ipairs(settings) do
        local val = http_form[set]
        if val ~= nil then
             uci_r:set("qos", "settings", set, val)
        end
    end

    uci_r:commit("qos")
    return read_qos_settings()
end

-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
function load_rule()
    local rule_list = {}

    uci_r:foreach("qos", "rule",
        function(section)
            rule_list[#rule_list + 1] = uci_r:get_all("qos", section[".name"])
            if rule_list[#rule_list].mac ~= nil then
                rule_list[#rule_list].mac = (rule_list[#rule_list].mac):gsub(":", "-"):upper()
            end
            rule_list[#rule_list]     = uci_to_http(rule_list[#rule_list])
        end
    )

    -- fix lua bug, empty array should be [], not {}
    if #rule_list == 0 then
        rule_list = "[]"
    end

    return rule_list
end

function get_max_dev()
      -- profile check
    local others = {}
    others.max_rules = uci_r:get_profile("qos", "max_rules") or "32"
    return others
end

function insert_rule(http_form)
    local ret = {}
    local new = http_form
    if new.mac ~= nil then
        new.mac = (new.mac):gsub("-", ":"):upper()
    end

    local count     = form:count("qos", "rule")
    local max_count = uci_r:get_profile("qos", "max_rules")

    local new_rules = {}

    if new.method == "app" then
        -- app
        if new.rule_app ~= "" and new.rule_app ~= nil then
            local app_list = luci.util.split(new.rule_app, ',')
            for i,v in ipairs(app_list) do
                if v ~= "" then
                    new_rules[#new_rules + 1] = v
                end
            end
        end

        -- custom
        if new.custom_name ~= "" and new.port ~= "" and new.proto ~= ""
            and new.custom_name ~= nil and new.port ~= nil and new.proto ~= nil
        then
            new_rules[#new_rules + 1] = "custom"
        end
    else
        -- for non-app rule
        new_rules[#new_rules + 1] = ""
    end


    -- check profile limit
    for i, rule_num in ipairs(new_rules) do
        if count < max_count then
            ret = http_to_uci(new, rule_num)
            ret = form:insert("qos", "rule", ret)
            if not ret then
                return false
            end
        else
            return false
        end
    end
    
    uci_r:commit("qos")

    if new.mac ~= nil then
        new.mac = (new.mac):gsub(":", "-"):upper()
    end
    return load_rule()
end

function remove_rule(http_form)
    -- get data
    local ret   = {}
    local key   = http_form.key or "default"
    local index = http_form.index
    
    ret = form:delete("qos", "rule", key, index)
    uci_r:commit("qos")
    return load_rule()

end

function uci_to_app( uci_app )
    local ret = {}
    ret.name = uci_app.name
    ret.id   = uci_app.id
    return ret
end

function load_list_app( )
    local ret = {
                    {   name      = "APP", 
                        id        = 1,
                        children  = {}
                    }
                    -- ,

                    -- {   name      = "GAME", 
                    --     id        = 2,
                    --     children  = {}
                    -- }
                }

    local rule_list = {}
    uci_r:foreach("qos", "app",
        function(section)
            rule_list[#rule_list + 1] = uci_r:get_all("qos", section[".name"])
            local tmp = ret[1].children
            ret[1].children[#ret[1].children + 1] = uci_to_app(rule_list[#rule_list])
        end
    )

    return ret
end

function read_db_version()
    local ret = {}
    local version = uci_r:get("qos", "settings", "version") or "unknown"
    ret.firmware_version = "Qos database " .. version
    ret.totaltime = "5"
    return ret
end

function database_check(filepath)
    local cry = require "luci.model.crypto"

    if nixio.fs.access(filepath) then
        luci.sys.exec('mkdir /tmp/check')
        luci.sys.exec("tail -c +34 " .. filepath .. " > /tmp/check/check.cry")
        luci.sys.exec('head -c 32 ' .. filepath .. ' > /tmp/check/check.md5')
        luci.sys.exec('echo "  /tmp/check/check.cry" >> /tmp/check/check.md5')

        local result = luci.sys.call('md5sum -c /tmp/check/check.md5 >/dev/null 2>&1')
        if result ~= 0 then
            dbg.printf("error:check first md5 failed:" .. tostring(result))
            luci.sys.exec("rm /tmp/check -rf >/dev/null 2&1")
            return false
        end

        dbg.printf("decry")
        local cryfunc = cry.dec_file("/tmp/check/check.cry", "777")
        cry.dump_to_file(cryfunc,"/tmp/check/check.tar")

        dbg.printf("untar")
        result = luci.sys.call("tar -xvf /tmp/check/check.tar -C /tmp/check/ >/dev/null 2>&1")
        if result ~= 0 then
            dbg.printf("error:untar fail:".. result)
            luci.sys.exec("rm /tmp/check -rf >/dev/null 2&1")
            return false
        end
        
        dbg.printf("image_check")
        if nixio.fs.access("/tmp/check/check") then
            result = luci.sys.call("cd /tmp/check;md5sum -c check >/dev/null 2>&1;cd -")

            if result == 0 then
                -- debug.printf("success")
                luci.sys.exec("cp /tmp/check/database.bin /tmp;rm /tmp/database.tar;rm /tmp/check -rf >/dev/null 2>&1")
                return true
            else
                dbg.printf("result:" .. tostring(result))
                --debug.printf("error:check second md5 failed")
                luci.sys.exec("rm /tmp/check -rf >/dev/null 2&1")
                            return false
            end
        else
            dbg.printf("error:no target file")
            luci.sys.exec("rm /tmp/check -rf >/dev/null 2&1")
            return false
        end
    end

    return false
end

function db_create_config( file )

    if file == nil then
        sys.exec("nvrammanager -r /tmp/database.bin -p qos-db")
    end

    local ret = true
    sys.exec("iqos-db-parser " .. "/tmp/database.bin" .. " /tmp/db_id_name")

    if nixio.fs.access("/tmp/db_id_name") then
        local f = io.input("/tmp/db_id_name")
        if nil == f then
            return false
        end

        local maxid = 0

        -- Delete old appid --
        uci_r:foreach("qos", "app",
            function(section)
                if section["custom"] == "appid" and section["name"] ~= nil then
                    uci_r:delete("qos", section["name"])
                end
            end
        )

        uci_r:foreach("qos", "app",
            function(section)
                if tonumber(section["id"]) > maxid then
                    maxid = tonumber(section["id"])
                end
            end
        )

        local lines, id, name
        local info = {}
        lines = f:read("*line")
        uci_r:set("qos", "settings", "version", lines)
        while true do
            lines = f:read("*line")
            if not lines then break end
            id, name = string.match(lines, "(%d+):([%w%s_]+)")

            -- FIXME: Write id and name to config --
            if id ~= nil and name ~= nil then
                maxid = maxid + 1
                info = { name = name, id = maxid, custom = "appid", appid = id }
                uci_r:section("qos","app", name, info)
            end 
        end

        uci_r:commit_without_write_flash("qos")
        sys.fork_exec("rm /tmp/db_id_name")
    end

    return true
    -- body
end

function db_update()
    local ret = true

    if database_check("/tmp/database.tar") == false then
        return false, "err_check"
    end

    ret = db_create_config("/tmp/database.bin")

    -- Delete old rule --
    uci_r:delete_all("qos", "rule")

    if nixio.fs.access("/tmp/database.bin") then
        sys.fork_exec("cd /tmp/; nvrammanager -e -p qos-db; \
		nvrammanager -w database.bin -p qos-db; rm database.bin; \
		echo 'qos database upgrade' > /dev/console;")
    end

    return ret
end

-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
local dispatch_tbl = {
    settings = {
        ["read"]  = { cb  = read_qos_settings },
        ["write"] = { cb  = write_qos_settings,
                      cmd = "/etc/init.d/qos restart" }        
    },

    add = {
        ["add"]    = { cb  = insert_rule,
                       cmd = "/etc/init.d/qos reload" },
    },

    list = {
        ["read"]   = { cb  = load_rule,
                       others = get_max_dev },
        ["del"] = { cb  = remove_rule,
                       cmd = "/etc/init.d/qos reload" }
    },

    applist = {
        ["load"]   = { cb = load_list_app },
    },

    upgrade = {
        ["read"]   = { cb = read_db_version },
        ["check"]  = { cb = db_update,
                       cmd = "/etc/init.d/qos reload" }
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
    local fp
    local image_tmp   = "/tmp/database.tar"
    luci.http.setfilehandler(
        function(meta, chunk, eof)
            if not fp then
                fp = io.open(image_tmp, "w")
            end
            if chunk then
                fp:write(chunk)
            end
            if eof then
                fp:close()
            end
        end
    )
    return ctl._index(dispatch)
end

function index()
    entry({"admin", "qos"}, call("_index")).leaf = true
end

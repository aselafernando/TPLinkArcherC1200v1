--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  timesetting.lua
Details :  controller for time settings webpage
Author  :  Wen Kun <wenkun@tp-link.net>
Version :  1.0.0
Date    :  17Mar, 2014
]]--

local uci               = require "luci.model.uci"
local fs                = require "luci.fs"
local dbg               = require "luci.tools.debug"
local dt                = require "luci.tools.datatypes"
local nixio             = require "nixio"
local sys               = require "luci.sys"
local json              = require "luci.json"
local ctl               = require "luci.model.controller"

local uci_r = uci.cursor()

module("luci.controller.admin.timesetting", package.seeall)

local TIME_SETTINGS_SHELL = "/etc/init.d/time_settings"
local RELOAD = "reload"

local NTP_STATUS_TBL = {success = "747301", failed = "747302", waiting = "747303"}

local TIMEZONE_PATH = "/www/webpages/data/timezone.json"

local TZ = {
    {"0", 'GMT-12:00', 'GMT+12:00'},
    {"60", 'GMT-11:00', 'GMT+11:00'},
    {"120", 'GMT-10:00', 'GMT+10:00'},
    {"180", 'GMT-09:00', 'GMT+09:00'},
    {"240", 'GMT-08:00', 'GMT+08:00'},
    {"300", 'GMT-07:00', 'GMT+07:00'},
    {"360", 'GMT-06:00', 'GMT+06:00'},
    {"420", 'GMT-05:00', 'GMT+05:00'},
    {"450", 'GMT-04:30', 'GMT+04:30'},
    {"480", 'GMT-04:00', 'GMT+04:00'},
    {"510", 'GMT-03:30', 'GMT+03:30'},
    {"540", 'GMT-03:00', 'GMT+03:00'},
    {"600", 'GMT-02:00', 'GMT+02:00'},
    {"660", 'GMT-01:00', 'GMT+01:00'},
    {"720", 'GMT-00:00', 'GMT+00:00'},
    {"780", 'GMT+01:00', 'GMT-01:00'},
    {"840", 'GMT+02:00', 'GMT-02:00'},
    {"900", 'GMT+03:00', 'GMT-03:00'},
    {"930", 'GMT+03:30', 'GMT-03:30'},
    {"960", 'GMT+04:00', 'GMT-04:00'},
    {"990", 'GMT+04:30', 'GMT-04:30'},
    {"1020", 'GMT+05:00', 'GMT-05:00'},
    {"1050", 'GMT+05:30', 'GMT-05:30'},
    {"1065", 'GMT+05:45', 'GMT-05:45'},
    {"1080", 'GMT+06:00', 'GMT-06:00'},
    {"1110", 'GMT+06:30', 'GMT-06:30'},
    {"1140", 'GMT+07:00', 'GMT-07:00'},
    {"1200", 'GMT+08:00', 'GMT-08:00'},
    {"1260", 'GMT+09:00', 'GMT-09:00'},
    {"1290", 'GMT+09:30', 'GMT-09:30'},
    {"1320", 'GMT+10:00', 'GMT-10:00'},
    {"1380", 'GMT+11:00', 'GMT-11:00'},
    {"1440", 'GMT+12:00', 'GMT-12:00'},
    {"1500", 'GMT+13:00', 'GMT-13:00'},
}

--- Time settings parameter table
local TS_TBL  = {'timezone', 'date', 'time', 'ntp_svr1', 'ntp_svr2', 'gmt_status',}

--- Daylight saving parameter table
local DST_TBL = {'dst_enable', 'start_month', 'start_week', 'start_day', 'start_hour',
                'end_month', 'end_week', 'end_day', 'end_hour',}
local WEEK_TBL = {['1st']=1, ['2nd']=2, ['3rd']=3, ['4th']=4, ['5th']=5}
local DAY_TBL  = {Mon=1, Tues=2, Wed=3, Thur=4, Fri=5, Sat=6, Sun=0}
local MON_TBL  = {Jan=1, Feb=2, Mar=3, Apr=4, May=5, Jun=6, Jul=7, Aug=8, Sep=9, Oct=10, Nov=11, Dec=12}

local function parse_timezone(zone, cmd)
    if not zone or not cmd then
        return false
    end

    local index = (cmd == "tonumber") and 2 or 1
    local ret

    for k, v in ipairs(TZ) do
        if v[index] == zone then
            ret = (cmd == "tonumber") and v[1] or v
            break
        end
    end
    return ret
end

local function check_ntpserver(ntpsvr)
    local ntps = (ntpsvr and type(ntpsvr)=="table") and ntpsvr or {ntpsvr}
    local ret = {}

    for _, v in ipairs(ntps) do
        ret[#ret+1] = dt.host(v) and v or nil
    end
    return (#ret > 0) and ret or false
end

local function ts_set_time(date, time)
    local month, day, year = string.match(date, "(%d+)/(%d+)/(%d+)")
    local hour, min, sec = string.match(time, "(%d+):(%d+):(%d+)")
    if month and day and year and hour and min and sec then
        luci.sys.call("date -s '%04d-%02d-%02d %02d:%02d:%02d'" %{
            year, month, day, hour, min, sec
        })
    else
        return false
    end
    return true
end

local function ts_parse_hours(hour)
    local h, s = hour:match("(%d+)(%S+)")

    if tonumber(h) > 12 or s ~= "am" and s ~= "pm" then
        return false
    end

    if s == "pm" then
        local h = h + 12
        if h > 24 then h = 0 end
        return tostring(h)
    else
        return h
    end
end

--- Load time settings
-- @param N/A
-- @return data return settings
function ts_load_settings()
    local sets = {}
    -- get current type
    sets.type = uci_r:get("system", "ntp", "type")

    -- get current system time
    local cur_time = os.time()
    sets.date = os.date("%m/%d/%Y", cur_time)       -- get date of string format, month/day/year
    sets.time = os.date("%X", cur_time)             -- get time of string format, 23:48:10

    -- get system ntp server
    local ntp_table = uci_r:get("system", "ntp", "server")
    sets.ntp_svr1 = ntp_table and ntp_table[1] or ""
    sets.ntp_svr2 = ntp_table and ntp_table[2] or ""

    -- get time zone
    local sname = uci_r:get_first("system", "system", nil, nil)
    local timezone = uci_r:get("system", sname, "timezone")
    timezone = (timezone == "UTC" or timezone == nil) and "GMT" or timezone
    timezone = parse_timezone(timezone, "tonumber")
    if not timezone then return false, "timezone is error" end
    sets.timezone = timezone
    -- Get week day
    sets.day = sys.exec("date"):match("^%a+") or ""

    return sets
end

--- Save time settings
-- @param content settings from webpage
-- @return data return settings
function ts_save_settings(content)
    local ret
    if content.type then
        uci_r:set("system", "ntp", "type", content.type)
    end

    if content.type == "auto" or not content.type then
        -- uci set timezone
        local timezone = parse_timezone(content.timezone, "tostring")
        if not timezone then return false, "timezone is error!" end
        local sname = uci_r:get_first("system", "system", nil, nil)
        local cur_tz = uci_r:get("system", sname, "timezone")
        if cur_tz ~= timezone[2] then
            uci_r:set("system", sname, "timezone", timezone[2])
        end
    end

    -- auto mode, set ntp server
    if content.type == "auto" then
        -- uci set ntp server
        local ntpsvr = check_ntpserver({content.ntp_svr1, content.ntp_svr2})
        if (content.ntp_svr1 == "" and content.ntp_svr2 == "") or ntpsvr then
            uci_r:delete("system", "ntp", "server")   -- FIX uci set list bug, do delete before set
            uci_r:set("system", "ntp", "server", ntpsvr or "")      
        end

    elseif content.type == "manual" then
        -- set time (time is not in uci config)
        if content.date and content.time then
            ret = ts_set_time(content.date, content.time)
            if not ret then
                return false, "date or time format is error"
            end
            luci.sys.call("echo 1 > /tmp/ledpm_enable")
        end

    end

    -- save uci config & reload
    uci_r:commit("system") -- save time settings
    luci.sys.call("%s %s" %{TIME_SETTINGS_SHELL, RELOAD})

    -- ntpd restart
    if content.type == "auto" then
        sys.fork_exec("env -i /etc/init.d/sysntpd restart >/dev/null")
    elseif content.type == "manual" then
        sys.fork_exec("env -i /etc/init.d/sysntpd stop >/dev/null")
    end
   
    return ts_load_settings()

end

--- Load daylight saving settings
-- @param N/A
-- @return data return settings
function ts_load_dst()
    local dstdata = {}
    local defaults = {'off', 'Jan', '1st', 'Mon', '1am', 'Jan', '1st', 'Mon', '1am'}
    local cfg_data = uci_r:get_all("system", "dst")

    for k, v in ipairs(DST_TBL) do
        dstdata[v] = cfg_data and cfg_data[v] or defaults[k]
        --dbg.printf("load: " .. v .. "," .. dstdata[v])
    end

    local cur_time = os.time()
    dstdata.dst_enable = uci_r:get("system", "dst", "dst_enable")
    if dstdata.dst_enable == "off" then
        dstdata.dst_status = ""
    else
        dstdata.dst_status = (os.date("*t", cur_time)).isdst and "up" or "down"
    end

    return dstdata
end

--- Save daylight saving settings
-- @param content settings from webpage
-- @return data return settings
function ts_save_dst(content)

    -- get time zone settings from config
    local sname = uci_r:get_first("system", "system", nil, nil)
    local tz = uci_r:get("system", sname, "timezone")
    tz = tz:find("+") and tz:gsub("+", "-") or tz:gsub("-", "+")

    uci_r:section("system", "dst", "dst", content)
    uci_r:commit("system")

    -- phase parameters
    luci.sys.call("%s %s" %{TIME_SETTINGS_SHELL, RELOAD})
    return ts_load_dst()
end

local function write_status_to_file(str)
    local fd = io.open("/tmp/gmt_status.log", "w")
    if type(str) == "string" then
        fd:write(str .. "\n")
    end

    fd:close()
end

--- Get GMT
-- @param content settings from webpage
-- @return data
function ts_act_gmt(content)
    local ntp1 = content.ntp_svr1
    local ntp2 = content.ntp_svr2
    local ntpsvr = check_ntpserver({ntp1, ntp2})

    if ntpsvr then
        local cur_ntpsvr = uci_r:get_list("system", "ntp", "server")
        if (ntpsvr[1] ~= cur_ntpsvr[1] or ntpsvr[2] ~= cur_ntpsvr[2]) then
            uci_r:delete("system", "ntp", "server")   -- FIX uci set list bug, do delete before set
            uci_r:set_list("system", "ntp", "server", ntpsvr)
        end
    else
        return false, "NTP Server is invalid."
    end

    local server_flag = uci_r:get("system", "ntp", "enable_server")
    -- set ntpd to client mode
    if server_flag ~= "off" then
        uci_r:set("system", "ntp", "enable_server", "off")
    end

    uci_r:commit("system")

    -- restart nptd
    luci.sys.call("env -i /etc/init.d/sysntpd stop >/dev/null")

    -- call ntpd cammand to sync time
    if nixio.fork() == 0 then
        local i = nixio.open("/dev/null", "r")
        local o = nixio.open("/dev/null", "w")
        nixio.dup(i, nixio.stdin)
        nixio.dup(o, nixio.stdout)
        i:close()
        o:close()

        local call_ret = 0

          if ntpsvr[1] ~= nil then
            local cmd = string.format("ntpd -p %s -qNn" % ntpsvr[1])
            dbg.printf(cmd)
            call_ret = luci.sys.call(cmd)
            dbg.printf(tostring(call_ret))
        else
            call_ret = 1
        end

        if call_ret == 0 then
            write_status_to_file(NTP_STATUS_TBL.success)
        elseif ntpsvr[2] ~= nil then
            dbg.printf(ntpsvr[2])
            local cmd = string.format("ntpd -p %s -qNn" % ntpsvr[2])
            dbg.printf(cmd)
            local call_ret = luci.sys.call(cmd)
            dbg.printf(tostring(call_ret))
            if call_ret == 0 then
                write_status_to_file(NTP_STATUS_TBL.success)
            else
                write_status_to_file(NTP_STATUS_TBL.failed)
            end
        else
            write_status_to_file(NTP_STATUS_TBL.failed)
        end
        
    else
        write_status_to_file(NTP_STATUS_TBL.waiting)  
    end

    sys.fork_exec("env -i /etc/init.d/sysntpd start >/dev/null")
    
    return ts_save_settings(content)
end

function ts_refresh_gmt(content)
    local status
    if nixio.fs.access("/tmp/gmt_status.log") then
        for line in io.lines("/tmp/gmt_status.log") do
            status = string.match(line, "(%x+)")
        end
    end
    local data, errorcode = ts_load_settings()
    if data then
        data["status"] = status or NTP_STATUS_TBL.failed
        return data
    else
        return false, errorcode
    end
end

function ts_load_timezone(content)
    local file = io.open(TIMEZONE_PATH, "r")
    if file then
        local jsondata = file:read("*a")
        file:close()

        local tabledata = json.decode(jsondata or "")
        return tabledata and tabledata.data or false
    else
        return false
    end
end

-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------

local dispatch_tbl = {
    dst = {
        ["read"]  = { cb  = ts_load_dst },
        ["write"] = { cb  = ts_save_dst }        
    },

    settings = {
        ["read"]      = { cb  = ts_load_settings },
        ["write"]     = { cb  = ts_save_settings },
        ["gmt"]       = { cb  = ts_act_gmt }, 
        ["refresh"]   = { cb  = ts_refresh_gmt }
    },

    timezone = {
        ["read"] = { cb = ts_load_timezone }
    }
}

function dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end

function _index()
    return ctl._index(dispatch)
end

function index()
    entry({"admin", "time"}, call("_index")).leaf = true
end
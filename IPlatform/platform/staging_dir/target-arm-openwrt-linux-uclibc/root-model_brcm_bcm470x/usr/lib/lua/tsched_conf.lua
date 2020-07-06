--[[
Copyright(c) 2008-2013 Shenzhen TP-LINK Technologies Co.Ltd.

File    : tsched_conf.lua
Details : Lua interface for tsched configuration
Author  : Ye Qianchuan <yeqianchuan@tp-link.net>
Version : 1.0
Date    : 4 Mar, 2014
]]--

module("tsched_conf", package.seeall)

local CONF_DIR = "/etc/tsched.conf.d/"
local UPDATE_DIR = CONF_DIR .. "update.d/"
local SAVE_DIR = "/tmp/.tsched/"
local STYPE = "entry"

local util = require "luci.util"
local uci = require "luci.model.uci"
local json = require "luci.json"
local bit = require "bit"

local function check_args(...)
    for i = 1, select('#', ...) do
        local v = select(i, ...)
        assert(type(v) == "string" and string.len(v) > 0,
               "invalid arguments")
    end
end

local function exist(mname)
    local fs = require "nixio.fs"
    return fs.access(UPDATE_DIR .. mname, "f")
end

local function touch(path)
    local file, msg = io.open(path, "w")
    if not file then
        return false, msg
    end
    file:close()
    return true
end

TschedConf = util.class()

--- Class initialization.
-- @param mname		Module name, non-empty string.
function TschedConf:__init__(mname)
    check_args(mname)

    self.mname = mname
    self.uci_r = uci.cursor(CONF_DIR, SAVE_DIR)
end

--- Show the entries of a module, DEBUG ONLY.
-- @return			Boolean
function TschedConf:show()
    local mname = self.mname
    local uci_r = self.uci_r
    print(mname .. ":")
    return uci_r:foreach(mname, STYPE,
                        function(section)
                            print(section.entry_arg, section.ranges)
                        end), "not existent"
end

--- Delete all entries from a module.
-- @return			Boolean
function TschedConf:del_all()
    local mname = self.mname
    local uci_r = self.uci_r

    if not exist(mname) then
        return true
    end

    -- FIXME: delete_all don't raise any error, is it safe?
    uci_r:delete_all(mname, STYPE)
    return true
end

--- Add an entry to a module.
-- @param entry_arg	Entry argument, non-empty string.
-- @param ranges	Time ranges where to be active, non-empty string.
-- @return			Boolean
function TschedConf:add_entry(entry_arg, ranges)
    check_args(entry_arg, ranges)
    local mname = self.mname
    local uci_r = self.uci_r

    local values = {
        ["entry_arg"] = entry_arg,
        ["ranges"] = ranges
    }

    if not exist(mname) then
        -- the config file is missing, create it.
        local ret, msg = touch(CONF_DIR .. mname)
        if not ret then
            return false, msg
        end
    end

    -- FIXME: May be better return a UCI error string.
    return uci_r:section(mname, STYPE, nil, values)
end

--- Delete an entry from a module.
-- @param entry_arg	Entry argument, non-empty string.
-- @param ranges	Time ranges where to be active, non-empty string.
-- @return			Boolean
function TschedConf:del_entry(entry_arg, ranges)
    check_args(entry_arg)
    local mname = self.mname
    local uci_r = self.uci_r

    if not exist(mname) then
        return true
    end

    local values = {
        ["entry_arg"] = entry_arg,
        ["ranges"] = ranges
    }
    -- FIXME: delete_all don't raise any error, is it safe?
    uci_r:delete_all(mname, STYPE, values)
    return true
end

--- Save configs.
-- @param no_err	Ignore save error.
-- @return			Boolean
function TschedConf:save(no_err)
    local mname = self.mname
    local uci_r = self.uci_r
    return no_err or uci_r:save(mname)
end

--- Update a module, reload its configs and refresh it.
-- @return			Boolean
function TschedConf:update()
    local mname = self.mname
    local uci_r = self.uci_r

    if not exist(mname) then
        -- the config file is missing, create it.
        local ret, msg = touch(CONF_DIR .. mname)
        if not ret then
            return false, msg
        end
    end

    local ret, msg = uci_r:rawcommit(mname)
    if not ret then
        return false, msg
    end

    return touch(UPDATE_DIR .. mname)
end

local weekdays = {
    "sun", "mon", "tue", "wed", "thu", "fri", "sat"
}

--- Convert a calendar json string received
--- from http form to a ranges string.
-- @param calendar	The calendar json string.
-- @return			The ranges string if success, or false.
function convert_calendar(calendar)
    check_args(calendar)

    local cld = json.decode(calendar)
    for k, v in pairs(cld) do
        if not util.contains(weekdays, k) then
            return false
        end

        -- Assume ranges array is sorted.
        local s = -1
        for _, range in ipairs(v) do
            if s >= range[1] or range[1] >= range[2] then
                return false
            end
            s = range[2]
        end
        if 0 > v[1][1] or s > 24 then
            return false
        end
    end

    return calendar
end

function calendar_to_bytestring(cld)
    check_args(cld)

    cld = json.decode(cld)

    local bs = {}
    for i=1,24 do
        bs[i] = 0
    end

    for wi, wd in ipairs(weekdays) do
        local ranges = cld[wd] or {}
        for _, range in ipairs(ranges) do
            for h = range[1]+1, range[2] do
                bs[h] = bit.set(bs[h], bit.lshift(1, wi-1))
            end
        end
    end

    return string.char(unpack(bs))
end

function bytestring_to_calendar(bs)
    check_args(bs)
    assert(#bs ==  24)

    bs = { bs:byte(1, #bs) }
    bs[25] = 0
    local cld = {}
    for wi, wd in ipairs(weekdays) do
        local state = 0
        local s = 0
        for i, v in ipairs(bs) do
            if bit.check(bs[i], bit.lshift(1, wi-1)) then
                if state == 0 then
                    s = i - 1
                    state = 1
                end
            else
                if state ~= 0 then
                    cld[wd] = cld[wd] or {}
                    cld[wd][#cld[wd] + 1] = {s, i-1}
                    state = 0
                end
            end
        end
        assert(state == 0)
    end

    return json.encode(cld)
end

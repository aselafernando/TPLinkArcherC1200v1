--[[
Copyright(c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  tfstats.lua
Details :  traffic statistics  model
Author  :  Wen Kun <wenkun@tp-link.net>
Version :  1.0.0
Date    :  31Mar, 2014
]]--

local sys = require "luci.sys"
local utl = require "luci.util"
local uci = require "luci.model.uci"
local ip  = require "luci.ip"
local bus = require "ubus"
local form = require "luci.tools.form"
local dtypes = require "luci.tools.datatypes"
local dbg = require "luci.tools.debug"

module("luci.model.tfstats", package.seeall)

TFS_INST = utl.class()

function TFS_INST:__init__()
    self.module = "tfstats"
    self.uci    = uci.cursor()
    self.cfg    = "tfstats"
    self.sec    = "switch"
    self.ubus   = bus.connect()
    self.state  = false

    for _, obj in ipairs(self.ubus:objects()) do
        if obj:match(self.module) then
            self.state = true
            break
        end
    end
end

function clear_stats(stats_tbl, index)
    local stats_list = stats_tbl

    for idx, stats in ipairs(stats_list) do
        for k, vt in pairs(stats) do
            if k ~= "ip" and k ~= "mac" then
                stats[k] = 0
            end
        end
        if index and index == idx then
            break
        end
    end
    return (index and stats_list[index] or stats_list)
end

function fix_stats_uint32(stats_tbl)
    for _, stats in ipairs(stats_tbl) do
        for k, vt in pairs(stats) do
            if k ~= "ip" and k ~= "mac" then
               stats[k] = (vt >= 0) and vt or (0x100000000 + vt)
            end
        end
    end
    return stats_tbl
end

--- Ubus call tfstas module
-- @param method  method for ubus call, get/set/delete/reset
-- @param data    param for ubus call
-- @return ubus return value
function TFS_INST:ubus_invoke(method, data)
    local ret

    if type(data) ~= "table" then
        data = { data }
    end

    if self.state then
        ret = self.ubus:call(self.module, method, data)
    else
        dbg.printf("tfstats module is not ready!")
        ret = false
    end

    return ret
end

--- Load switch status of traffic statistics.
-- @return status, on or off
function TFS_INST:load_enable()
    local sw = self.uci:get(self.cfg, self.sec, "enable")
    if sw and sw == "on" or sw == "off" then
        return {enable = sw}
    end
    return false
end

--- Set switch of traffic statistics.
-- @param status on or off
-- @return status, or errorcode if error accured.
function TFS_INST:set_enable(status)
    local sw = self:load_enable()
    local cmd = "stop"

    if status ~= "on" and status ~= "off" then
        return {errorcode = "Invalid argument."}
    elseif sw.enable and status ~= sw.enable then
        self.uci:set(self.cfg, self.sec, "enable", status)
        self.uci:commit(self.cfg)
        cmd = (status == "on") and "start" or "stop"
        sys.fork_exec("/etc/init.d/tfstats %s" % cmd .. "&")
    end
    return {enable = status}
end

--- Get all statistics
-- @return stats table list of statistics; total number.
function TFS_INST:load_stats()
    local stats = {}
    local total = 0

    local ret = self:ubus_invoke("get", {})
    if ret then
        total = ret.total
        stats = ret.tfslist
    end
    for _, v in pairs(stats) do
        v.ip = v.ip .. "/ " .. (v.mac:upper())
    end
    return fix_stats_uint32(stats), total
end

function TFS_INST:load_all_stats()
    return self:load_stats()
end

--- Reset all of statistics
-- @return stats table list of statistics.
function TFS_INST:reset_all_stats()
   -- local stats, total = self:load_stats()
    self:ubus_invoke("reset", {ip = "all"})
    return self:load_stats()
    --return {data = clear_stats(stats, nil)}
end

function TFS_INST:set_net(ipaddr, netmask)
    local ret
    if dtypes.ipaddr(ipaddr) and dtypes.ipaddr(netmask) then
        self:ubus_invoke("set_net", {ip = ipaddr, mask = netmask})
        ret = true
    else
        ret = false
    end
    return ret 
end
--- Reset specified ip statistics
-- @param num index of statistics that will be reset
-- @return stats statistics of that ip.
function TFS_INST:reset_one_stats(num)
    local stats, total = self:load_stats()
    local num  = num and tonumber(num) or nil

    if not stats or not total then
        return {errorcode = "Load statistics failed."}
    end

    if num and num < total and num >= 0 then
        local ipaddr = stats[num+1].ip
        dbg.printf("reset ip:" .. ipaddr)
        local ret = self:ubus_invoke("reset", {ip = ipaddr})
        -- stats = self:load_stats()
        -- return {data = stats[num]}
        return clear_stats(stats, num+1)
    end

    return false, "Invalid argument."
end

--- Delete of statistics
-- @param data argument table, for example :{key = "key-0", index = "1"}
-- @return rets results of delete operations.
function TFS_INST:delete_stats(data)
    local rets = {}
    local keys  = data.key
    local index = data.index

    if not keys or not index then
        return {errorcode = "Invalid argument of index or key."}
    end

    if type(index) == "table" then
        self:ubus_invoke("delete", {ip = "all"})
    else
        local stats, total = self:load_stats()
        local index = tonumber(index)
        if index < total and index >= 0 then
            local ipaddr = stats[index+1].ip
            self:ubus_invoke("delete", {ip = ipaddr})
        end
    end

    keys  = type(keys) == "table" and keys or {keys}
    index = type(index) == "table" and index or {index}

    for k, v in ipairs(index) do
        local ret = {
            key     = keys[k],
            index   = v,
            success = true
        }
        rets[#rets+1] = ret
    end

    return rets
end


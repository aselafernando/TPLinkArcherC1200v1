--[[
Copyright(c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  imb.lua
Details :  ip&mac binding configure model
Author  :  Guo Dongxian <guodongxian@tp-link.net>
Version :  1.0.0
Date    :  21Mar, 2014
]]--

module("luci.model.imb", package.seeall)

local sys = require "luci.sys"
local utl = require "luci.util"
local uci = require "luci.model.uci"
local ip  = require "luci.ip"
local bus = require "ubus"
local form = require "luci.tools.form"
local tool = require "luci.tools.datatypes"

IMB_MAX_CNT = 64

function dbg(str)
    sys.call("echo %s >/dev/console 2>&1" % ("[debug]:" .. str))
end

function err(str)
    sys.call("echo %s >/dev/console 2>&1" % ("[error]:" .. str))
end

-- Encapsulate ip&mac binding module, class
IMB_INST = utl.class()

function IMB_INST:__init__()
    self.module = "imb"
    self.config = "imb"
    self.stype  = "imb_rule"
    self.uci = uci.cursor()

    self.form = form.Form(self.uci)

    local state
    self.ubus = bus.connect()
    
    for _, obj in ipairs(self.ubus:objects()) do
        local imb = obj:match(self.module)
        if imb then
            state = true
            break
        end
    end

    self.state = state
end

--- Ubus call imb module
-- @param method  method for ubus call,get/set/del
-- @param data    param for ubus call
-- @return ubus return value
function IMB_INST:ubus_invoke(method, data)
    local res

    if type(data) ~= "table" then
        data = { data }    
    end    

    if self.state then
        res = self.ubus:call(self.module, method, data)
    end
    
    return res
end

--- Check imb entry valid
-- @param data 
-- @return bool true or false
function IMB_INST:arplist_check(data)
    if not data or type(data) ~= "table" then
        return false    
    end
    local mac = data.mac and data.mac:gsub('-', ':') or ""
    if data.ipaddr and ip.IPv4(data.ipaddr) 
        and mac:match("^%x[02468aceACE]:%x%x:%x%x:%x%x:%x%x:%x%x$") 
    then
        return true
    end
    return false
end

function IMB_INST:max_cnt_get()
    return self.uci:get_profile(self.config, "max_cnt")
end
--- Check imb max entry
function IMB_INST:max_cnt_check()
    local max = self:max_cnt_get() or IMB_MAX_CNT
    local cur = self.form:count(self.config, self.stype)
    
    if cur >= max then
        return false
    end
    return true
end

function IMB_INST:form_commit()
    self.uci:commit(self.config)
end

--- Get ip&mac binding items from config
-- @param N/A
-- @return table of all items
function IMB_INST:bandlist_read()
    local res = {}
    local data = {}    

    self.uci:foreach(self.config, self.stype, 
        function(s)
            data[#data+1] = self.uci:get_all(self.config, s[".name"])            
        end)
    
    for i, e in ipairs(data) do
        local tmp = {ipaddr = "", mac = "", enable = "", description = ""}
        tmp.ipaddr = e.ipaddr
        tmp.mac = e.mac:gsub(':', '-')
        tmp.mac = tmp.mac:upper()
        tmp.enable = (e.enable == "on") and "on" or "off"
        tmp.description = e.description or ""
        res[i] = tmp
    end
    
    return res
end

--- Get arp system entries from arp cache table
-- @param N/A
-- @return table of system arp entries
-- @return number of system arp
function IMB_INST:arplist_read()
    local arplist
    local number

    local result = self:ubus_invoke("get", {})
    if result then
        number = result.number
        arplist = result.arplist        
    end

    return arplist, (number or 0)
end

--- Check ip&mac binding function on/off
function IMB_INST:imb_enable()
    local switch = self.uci:get(self.config, "switch", "enable")
    return (switch == "on" and true or false)
end

--- Insert new ip&mac binding entry
-- @param new  data of new entry
-- @return current operated entry
function IMB_INST:arplist_insert(new)
    local key = {"ipaddr"}
    local res
    
    if not new or type(new) ~= "table" then
        return false
    end

    if not self:max_cnt_check() then
        return false
    end
    
    local bind = (new.enable == "on") and "on" or "off"
    bind = (bind == "on" and true) or false
    if self:arplist_check(new) then
        local data = {ip = new.ipaddr, mac = new.mac:gsub('-', ':'), bind = bind}
            
        res = self.form:insert(self.config, self.stype, new, key)
        
        if res then
            self:form_commit()
            if bind and self:imb_enable() then
                self:ubus_invoke("set", data)
            end
        end
    end
    
    return res
end

function IMB_INST:arplist_bind(new)
    local data = {}

    if not new or type(new) ~= "table" then
        return false    
    end

    self.uci:foreach(self.config, self.stype,
        function(s)
            data[#data+1] = self.uci:get_all(self.config, s[".name"])
        end)
    
    local find = false
    for _, e in ipairs(data) do
        if new.ipaddr == e.ipaddr then
            find = true
            -- Make the entry enabled 
            e.enable = "on" 
            e.mac = new.mac -- Make the mac of old entry be same to new entry
            self.uci:section(self.config, self.stype, e[".name"], e)
            self.uci:commit(self.config)                 
        end
    end

    -- Insert new arp entry to config and kernel
    if not find then
        if not self:max_cnt_check() then
            -- Reach max ip&mac binding entry, can not bound anymore
            return false
        end

        local c = {ipaddr = new.ipaddr, mac = new.mac, enable = new.enable}
        self.uci:section_first(self.config, self.stype, nil, c)
        self.uci:commit(self.config)     
    end
    -- Set ip&mac binding entry to arp cache table
    local d = {ip = new.ipaddr, mac = new.mac:gsub('-', ':'), bind = true} 
    self:ubus_invoke("set", d)
    
    return true
end

--- Update ip&mac bind entry
-- @param old
-- @param new
function IMB_INST:arplist_update(old, new)
    local key = {"ipaddr"}
    if not old or not new or type(old) ~= "table"
        or type(new) ~= "table" then
        return false
    end
    
    local find = false
    if self:arplist_check(old) then
        self.uci:foreach(self.config, self.stype,
		    function(s)
			    if s.ipaddr == old.ipaddr then
				    find = true
			    end
		    end)
    end
    
    local res
    if find and self:arplist_check(new) then
        -- Update config
        res = self.form:update(self.config, self.stype, old, new, key)
        if res then
            self:form_commit()
            if self:imb_enable() then 
                -- Delete old entry from arp cache table
                if old.enable and old.enable == "on" then
                    self:ubus_invoke("del", {ip = old.ipaddr})            
                end
                -- Set modified entry to arp cache table
                if new.enable and new.enable == "on" then
                    self:ubus_invoke("set", {ip = new.ipaddr, mac = new.mac:gsub('-', ':'), bind = true})
                end
            end
        end        
    end
    
    return res
end

--- Delete ip&mac binding entry from config and cache table
-- @param key
-- @param index
-- @param old  when the param is not nil, then param key and index must nil
function IMB_INST:arplist_del(key, index, old)
    local data = {}
    local res = {}

    self.uci:foreach(self.config, self.stype,
        function(s)
            data[#data+1] = self.uci:get_all(self.config, s[".name"])
        end)
    -- When old is not nil, it means the operation from arp list
    -- Special operation
    if old and type(old) == "table" then
        local find = false
        res = true
        for _, e in ipairs(data) do
            if old.ipaddr == e.ipaddr then
                find = true
                -- Modify the entry from config, let it unbound
                e.enable = "off"
                res = self.uci:section(self.config, self.stype, e[".name"], e)
                self.uci:commit(self.config)
                -- Delete this entry from arp cache table
                if res and old.enable == "on" and self:imb_enable() then
                    self:ubus_invoke("del", {ip = old.ipaddr})
                end
                break            
            end        
        end
        
        if not find and self:imb_enable() then
            self:ubus_invoke("del", {ip = old.ipaddr})
        end
    else -- Operation from bind list       
        if index and type(index) == "table" then
            res = self.form:delete(self.config, self.stype, key, index)
            if res then
                self:form_commit()
                if self:imb_enable() then
                    for _, idx in ipairs(index) do
                        local e = data[idx+1]
                        if e and e.enable == "on" then
                            self:ubus_invoke("del", {ip = e.ipaddr})
                        end
                    end
                end
            end                
        end
    end
    
    return res
end

--- Remove arp entry from arp cache table
function IMB_INST:arplist_remove(old)
    return self:arplist_del(nil, nil, old)
end

--- Remove arp entry from config and cache
function IMB_INST:bandlist_remove(key, index)
    return self:arplist_del(key, index, nil)
end

--- Bind all ip&mac binding entries to arp cache
-- @param N/A
-- @return bool true or false
function IMB_INST:arplist_enall()
    local data = {}

    self.uci:foreach(self.config, self.stype,
        function(s)
            data[#data+1] = self.uci:get_all(self.config, s[".name"])
        end)

    for _, e in ipairs(data) do
        if e.enable == "on" then
            self:ubus_invoke("set", {ip = e.ipaddr, mac = e.mac:gsub('-', ':'), bind = true})        
        end
    end

    return true
end

--- Unbound all ip&mac binding entries to arp cache
-- @param N/A
-- @return bool true or false
function IMB_INST:arplist_disall()
    local sys_arplst, num = self:arplist_read()

    if num <= 0 then
        return true
    end

    for _, e in ipairs(sys_arplst) do
        -- Delete permanent arp entry
        if e.flags and tonumber(e.flags) == 0x06 then
            self:ubus_invoke("del", {ip = e.ip})        
        end
    end

    return true
end


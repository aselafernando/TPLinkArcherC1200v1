--[[
Copyright(c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  imb.lua
Details :  ip&mac binding http response operation
Author  :  Guo Dongxian <guodongxian@tp-link.net>
Version :  1.0.0
Date    :  21Mar, 2014
]]--

module("luci.controller.admin.imb", package.seeall)

local imb  = require "luci.model.imb"
local uci  = require "luci.model.uci"
local net  = require "luci.model.network"
local http = require "luci.http"
local ip   = require "luci.ip"
local json = require "luci.json"
local tool = require "luci.tools.datatypes" 
local bit  = require "bit"
local ctl  = require "luci.model.controller"

-- ARP Flags
local ATF_COM  = 0x02 --Complete
local ATF_PERM = 0x04 --Permanent

-- ERROR Code for operation
local ERR_TBL = {
    ["#001"] = "general error",
    ["#002"] = "bind list operation error",
    ["#003"] = "arp list operation error"
}

local Imb = imb.IMB_INST()

local function on_off(val)
    local opt = { on = true, off = true }
    return opt[val]
end

local function comm_read()
    local uci_c = uci.cursor()
    local en = uci_c:get("imb", "switch", "enable")

    en = (en == "on") and "on" or "off"    
    return { enable = en }
end

local function comm_write(formvalue)
    local old = comm_read()
    local uci_c = uci.cursor()
    local en = formvalue["enable"]

    if old.enable == en then
        return old 
    end

    -- Set option of imb switch section
    uci_c:set("imb", "switch", "enable", en)
    uci_c:commit("imb")    

    local res
    local log = require("luci.model.log").Log(218)
    if en == "on" then
        log(11)
        res = Imb:arplist_enall()
    else
        log(12)
        res = Imb:arplist_disall()
    end
    
    if res then
        return {enable = en}
    else
        return false, ERR_TBL["#001"]  --Operation failed, return error code  
    end
end

local function form_parse(formvalue)
    local tbl = {
        [1] = {},
        [2] = {},
        [3] = false --If true then operation is remove    
    }

    local n_str = formvalue["new"]
    local o_str = formvalue["old"]

    local new = n_str and json.decode(n_str) or false
    local old = o_str and json.decode(o_str) or false
    
    if new then
        if old and old ~= "add" then
            tbl[1] = new
            tbl[2] = old
        else
            tbl[1] = new
            tbl[2] = "add"
        end
    else 
        local index = formvalue["index"] or false
        local key = formvalue["key"] or false       

        tbl[1] = (type(key) == "table" and key) or {key}
        tbl[2] = (type(index) == "table" and index) or {index}
        tbl[3] = true
    end

    return tbl
end

--- Parse URL=bind_list form
-- @param  formvalue
-- @return bool parse success or not
-- @return url value, table
local function blst_parse(formvalue)
    local form = {}

    form = form_parse(formvalue)

    if not form[1] or not form[2] then
        return false, false
    end 

    if form[3] then
        return true, form
    end

    for _, e in ipairs(form) do
        if type(e) == "table" and e.ipaddr and ip.IPv4(e.ipaddr) 
            and e.mac and e.mac:gsub('-', ':'):match("^%x[02468aceACE]:%x%x:%x%x:%x%x:%x%x:%x%x$") 
                and on_off(e.enable) 
        then
            e.description = e.description or ""
        elseif type(e) == "string" and e == "add" then
            break
        elseif not e then
            break
        else
            return false, false
        end    
    end

   return true, form
end

--- Check new entry wether duplication or not
-- @param new the entry to be check
-- @return bool true or false
local function arp_dup_check(new)
    local old_lst = Imb:bandlist_read()
    
    if not old_lst then
        return false    
    end

    for _, e in ipairs(old_lst) do
        if new.ipaddr == e.ipaddr then
            return true        
        end    
    end

    return false
end

local function blst_max()
    return {max_rules = Imb:max_cnt_get()}
end

local function blst_load()
    return Imb:bandlist_read()
end

local function blst_insert(formvalue)
    local ret, form = blst_parse(formvalue)
    
    if not ret then
        return false, ERR_TBL["#002"]    
    end    
    
    if type(form) ~= "table" or type(form[1]) ~= "table" then
        return false, ERR_TBL["#002"]    
    end
    
    -- Duplication checking
    if arp_dup_check(form[1]) then
        return false, ERR_TBL["#002"]        
    end

    return Imb:arplist_insert(form[1])
end

local function blst_update(formvalue)
    local ret, form = blst_parse(formvalue)

    if not ret then
        return false, ERR_TBL["#002"]    
    end 

    if type(form) ~= "table" or type(form[1]) ~= "table" 
        or type(form[2]) ~= "table" 
    then
        return false, ERR_TBL["#002"]    
    end
    return Imb:arplist_update(form[2], form[1])
end

local function blst_remove(formvalue)
    local ret, form = blst_parse(formvalue)

    if not ret then
        return false, ERR_TBL["#002"]    
    end 

    if type(form) ~= "table" or type(form[1]) ~= "table" 
        or type(form[2]) ~= "table" 
    then
        return false, ERR_TBL["#002"]        
    end
    return Imb:bandlist_remove(form[1], form[2])
end

--- Parse URL=arp_list form
-- @param N/A
-- @return bool parse success or not
-- @return url value, table
local function arplst_parse(formvalue)
    local s_data = formvalue["data"]

    local data = s_data and json.decode(s_data) or false
    local form = {
        [1] = {},
        [2] = false    
    }

    if not data then
        return false, data    
    end

    data = (#data == 0) and {data} or data    

    for i, e in ipairs(data) do
        if not ip.IPv4(e.ipaddr) 
            or not e.mac:gsub('-', ':'):match("^%x[02468aceACE]:%x%x:%x%x:%x%x:%x%x:%x%x$") 
                or not on_off(e.enable) 
        then
            return false, false
        end
        e.enable = "on"
        form[1][i] = e    
    end

    return true, form
end

local function lan_subnet(lanip, arpip, mask)
    local lan_net = ip.IPv4(lanip, mask)
    local arp_net = ip.IPv4(arpip, mask)
    
    if lan_net and arp_net then
        return lan_net:contains(arp_net)
    end

    return false
end

local function bit_isset(flag, bits)
    if bit.band(tonumber(flag), bits) > 0 then
        return true    
    else
        return false
    end
end

local function arplst_load()
    local arp, num = Imb:arplist_read()
    
    if num <= 0 then
        return false    
    end
    
    local nw = net.init()
	local network = nw:get_network("lan")
    -- Get ip and mask of lan
	local lanip = network:ipaddr()
	local mask = network:netmask() or uci.cursor():get("network", "lan", "netmask")

    local data = {}
    for _, e in ipairs(arp) do
        if e.flags and bit_isset(e.flags, ATF_COM) then
            if e.ip and lan_subnet(lanip, e.ip, mask) then
                local tmp = {ipaddr = "", mac = "", enable = ""}
                tmp.ipaddr = e.ip
                tmp.mac = e.mac:gsub(':', '-'):upper()
                tmp.enable = bit_isset(e.flags, ATF_PERM) and "on" or "off"
                data[#data+1] = tmp
            end
        end
    end
    
    return data
end

--- Bind ip&mac operation from arp list
local function arplst_bind(formvalue)
    local ret, form = arplst_parse(formvalue)

    if not ret then
        return false, ERR_TBL["#003"]    
    end

    if type(form) ~= "table" or type(form[1]) ~= "table" then
        return false, ERR_TBL["#003"]    
    end

    local switch = comm_read()
    if not switch or switch.enable == "off" then
        return form[1]    
    end    
    
    for _, e in ipairs(form[1]) do
        local res = Imb:arplist_bind(e)
        e.success = res and true or false
    end

    return form[1]
end

--- Remove ip&mac operation from arp list
local function arplst_remove(formvalue)
    local ret, form = arplst_parse(formvalue)

    if not ret then
        return false, ERR_TBL["#003"]    
    end
    if type(form) ~= "table" or type(form[1]) ~= "table" then
        return form[1]
    end
    
    for _, e in ipairs(form[1]) do
        local res = Imb:arplist_remove(e)
        e.success = res and true or false
    end
    
    return form[1]
end

local dispatch_tbl = {
    ["setting"] = {
        ["read"]  = {cb = comm_read},
        ["write"] = {cb = comm_write}
    },

    ["bind_list"] = {
        ["load"] = {cb = blst_load, others = blst_max},
        ["insert"] = {cb = blst_insert, others = blst_max},
        ["update"] = {cb = blst_update, others = blst_max},
        ["remove"] = {cb = blst_remove, others = blst_max}
    },

    ["arp_list"] = {
        ["load"] = {cb = arplst_load, others = blst_max},  
        ["bind"] = {cb = arplst_bind, others = blst_max},
        ["remove"] = {cb = arplst_remove, others = blst_max}
    }
}

function dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end

function _index()
    return ctl._index(dispatch)
end

function index()
    entry({"admin", "imb"}, call("_index")).leaf = true
end


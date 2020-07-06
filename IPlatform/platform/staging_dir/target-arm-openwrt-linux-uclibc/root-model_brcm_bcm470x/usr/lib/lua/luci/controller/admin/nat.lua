--[[
Copyright(c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  nat.lua
Details :  controller for nat webpage
Author  :  Guo Dongxian <guodongxian@tp-link.net>
Version :  1.0.0
Date    :  05Mar14
History :  1.0.0  05Mar14  create the file 
]]--

module("luci.controller.admin.nat", package.seeall)

local nat_m  = require "luci.model.nat"
local json_m = require "luci.json"
local dt_m   = require "luci.tools.datatypes"
local utl    = require "luci.util"
local ctl    = require "luci.model.controller"

--- Check protocol: all/tcp/udp
local function nat_proto_chk(proto)
    if not proto then
        return false    
    end

    proto = proto:lower()
    if proto ~= "all" and proto ~= "tcp" and proto ~= "udp" then
        return false
    end
    return true
end

local function forward_url_parse(formvalue)
    local tbl = {
        [1] = {},
        [2] = {},
        [3] = false    
    }

    local n_str = formvalue["new"]
    local o_str = formvalue["old"]

    local new = n_str and json_m.decode(n_str) or false
    local old = o_str and json_m.decode(o_str) or false
    
    if new then
        if old and old ~= "add" then
            tbl[1] = new
            tbl[2] = old
        else
            tbl[1] = new
            tbl[2] = "add"
        end
    else 
        local index = formvalue["index"] or {}
        local key = formvalue["key"] or {}
        
        tbl[1] = (type(key) == "table") and key or {key} 
        tbl[2] = (type(index) == "table") and index or {index}
        tbl[3] = true -- Operation is remove
    end
    
    return tbl
end

--- Parse forward virsual server url
local function vs_url_parse(formvalue)
    local cfg = {}
    
    cfg = forward_url_parse(formvalue)
    
    -- Not parsing remove operation
    if cfg[3] then
        return true, cfg    
    end    
    
    -- Operation add/update
    local check
    for i, c in ipairs(cfg) do
        if not c or c == "add" then
            break        
        end

        if c.external_port and c.protocol and c.enable
        then
            c.name = c.name or ""
            --c.internal_port = c.internal_port or ""
            if not dt_m.port(c.external_port) 
                and not dt_m.portrange(c.external_port) 
            then
                nat_m.err("invalid external port.")
                return false, c             
            end

            if not c.internal_port or not dt_m.port(c.internal_port) 
            then
                c.internal_port = c.external_port
            end                

            if not dt_m.ip4addr(c.ipaddr) 
            then
                nat_m.err("invalid ipv4 address.")
                return false, c                
            end
            
            if not nat_proto_chk(c.protocol) 
            then
                nat_m.err("invalid protocol.")
                return false, c
            end
            check = true
        end
    end 

    if check then
        return true, cfg
    else
        return false, cfg
    end
end

--- Parse forward port trigger url
local function pt_url_parse(formvalue)
    local cfg = {}

    cfg = forward_url_parse(formvalue)

     -- Not parsing remove operation
    if cfg[3] then
        return true, cfg    
    end

    local check
    for _, c in ipairs(cfg) do

        if not c or c == "add" then
            break
        end

        if c.external_port and c.trigger_port then
            if not dt_m.port(c.trigger_port) then
                return false, c
            end

            if not nat_proto_chk(c.trigger_protocol) 
                or not nat_proto_chk(c.external_protocol) then
                nat_m.err("invalid protocol.")
                return false, c
            end

            c.external_port = utl.split(c.external_port, ",")
            for i = 1,5 do
                if c.external_port[i] and not dt_m.port(c.external_port[i])
                    and not dt_m.portrange(c.external_port[i]) then      
                    nat_m.err("invalid external port.")
                    return false, c                    
                end          
            end
            -- Temp operation for uci bug
            c.external_port[#(c.external_port) + 1] = ""
            check = true
        end        
    end
 
    if check then
        return true, cfg
    else
        return false, cfg
    end
end

local Nat = nat_m.NAT_INST()

local function get_global()
    return Nat:get("global")
end

local function set_global(formvalue)
    local cfg = { enable = "", hw_enable = "" }
    local en = formvalue["enable"]
    local hw_en = formvalue["boost_enable"]
    
    if not en or not hw_en then
        return false, "Invalid form value"
    end

    cfg.enable = en
    cfg.hw_enable = hw_en

    return Nat:operate("global", "write", cfg)
end

local function max_vs_rules()
    return {max_rules = Nat:max_rules("vs")}
end

local function get_vs_rules()
    return Nat:get("vs")
end

local function vs_do(formvalue, option)
    local ret, data = vs_url_parse(formvalue)
    if not ret then
        return false, "URL parsing fail"
    end
    return Nat:operate("vs", option, data[1], data[2])
end

local function insert_vs_rule(formvalue)
    return vs_do(formvalue, "insert")
end

local function update_vs_rule(formvalue)
    return vs_do(formvalue, "update")
end

local function remove_vs_rule(formvalue)
    return vs_do(formvalue, "remove")
end

local function max_pt_rules()
    return {max_rules = Nat:max_rules("pt")}
end

local function get_pt_rules()
    return Nat:get("pt")
end

local function pt_do(formvalue, option)
    local ret, data = pt_url_parse(formvalue)
    if not ret then
        return false, "URL parsing failed"
    end
    return Nat:operate("pt", option, data[1], data[2])
end

local function insert_pt_rule(formvalue)
    return pt_do(formvalue, "insert")
end

local function update_pt_rule(formvalue)
    return pt_do(formvalue, "update")
end

local function remove_pt_rule(formvalue)
    return pt_do(formvalue, "remove")
end

local function get_dmz()
    return Nat:get("dmz")
end

local function set_dmz(formvalue)
    local cfg = { enable = false, ipaddr = false }
    local old = get_dmz()

    local en = formvalue["enable"] or old.enable
    local ipaddr = formvalue["ipaddr"] or old.ipaddr

    if ipaddr and ipaddr ~= "" 
        and not dt_m.ip4addr(ipaddr) 
    then
        nat_m.err("Invalid ipv4 address")
        return false, "URL parsing failed"
    end

    if en and ipaddr then   
        cfg.enable = en
        cfg.ipaddr = ipaddr
    end

    return Nat:operate("dmz", "write", cfg)
end

local function get_alg()
    return Nat:get("alg")
end

local function set_alg(formvalue)
    local old = get_alg()

    if not old then
        return false, "Unknown error"    
    end

    local ftp   = formvalue["ftp"] or old.ftp
    local tftp  = formvalue["tftp"] or old.tftp
    local h323  = formvalue["h323"] or old.h323
    local rtsp  = formvalue["rtsp"] or old.rtsp
    local pptp  = formvalue["pptp"] or old.pptp
    local l2tp  = formvalue["l2tp"] or old.l2tp
    local ipsec = formvalue["ipsec"] or old.ipsec
    
    local cfg = { ["ftp"] = ftp, ["tftp"] = tftp, 
        ["h323"] = h323, ["rtsp"] = rtsp, ["pptp"] = pptp, 
        ["l2tp"] = l2tp, ["ipsec"] = ipsec }

    return Nat:operate("alg", "write", cfg)
end

local function apply(which)
    Nat:apply(which)
end

function index()
    entry({"admin", "nat"}, call("_index")).leaf = true
end

local dispatch_tbl = {
    setting = {
        ["read"]  = {cb = get_global},
        ["write"] = {cb = set_global, which = "nat"},    
    },

    vs = {
        ["read"] = {cb = get_vs_rules}, -- Just for other modules UI call
        ["load"] = {cb = get_vs_rules, others = max_vs_rules},
        ["insert"] = {cb = insert_vs_rule, which = "vs", others = max_vs_rules},
        ["update"] = {cb = update_vs_rule, which = "vs", others = max_vs_rules},
        ["remove"] = {cb = remove_vs_rule, which = "vs", others = max_vs_rules},
    },

    pt = {
        ["load"] = {cb = get_pt_rules, others = max_vs_rules},
        ["insert"] = {cb = insert_pt_rule, which = "pt", others = max_pt_rules},
        ["update"] = {cb = update_pt_rule, which = "pt", others = max_pt_rules},
        ["remove"] = {cb = remove_pt_rule, which = "pt", others = max_pt_rules},
    },

    dmz = {
        ["read"]  = {cb = get_dmz},
        ["write"] = {cb = set_dmz, which = "dmz"},
    },

    alg = {
        ["read"]  = {cb = get_alg},
        ["write"] = {cb = set_alg, which = "alg"},     
    }
}

function dispatch(http_form)
    local function _hook(success, action)
        if success and action.which then
            apply(action.which)
        end
        return true
    end

    return ctl.dispatch(dispatch_tbl, http_form, {post_hook = _hook})
end

function _index()
    return ctl._index(dispatch)
end


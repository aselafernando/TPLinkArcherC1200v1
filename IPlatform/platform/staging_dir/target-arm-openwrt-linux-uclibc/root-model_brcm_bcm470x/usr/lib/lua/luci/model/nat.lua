--[[
Copyright(c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  nat.lua
Details :  nat model api
Author  :  Guo Dongxian <guodongxian@tp-link.net>
Version :  1.0.0
Date    :  05Mar14
History :  1.0.0  05Mar14  create the file 
]]--

module("luci.model.nat", package.seeall)

local uci_m   = require "luci.model.uci"
local sys_m   = require "luci.sys"
local utl_m   = require "luci.util"
local http_m  = require "luci.http"
local form_m  = require "luci.tools.form"
local valid_m = require "luci.tools.datatypes"

local module_name = "nat"
local NAT_SHELL = "/etc/init.d/nat "
local FW_SHELL = "/etc/init.d/firewall "
local UPNP_SHELL      = "/etc/init.d/miniupnpd "
local NAT_DEV_NULL = " >/dev/null 2>&1"

local nat_uci = uci_m.cursor()
local form_m  = form_m.Form(nat_uci)

NAT_VS_MAX_RULES = 32
NAT_PT_MAX_RULES = 32

-- Keys for form operation
local NAT_FORWARD_KEYS = {
    vs = {"enable", "external_port", "protocol", "ipaddr"},
    pt = {"enable", "trigger_port", "trigger_protocol", "external_protocol"}
}

function dbg(str)
    sys_m.call("echo %s >/dev/console 2>&1" % (str))
end

function err(str)
    sys_m.call("echo %s >/dev/console 2>&1" % (str))
end

local function nat_on_off(val) return (val == "on") and "on" or "off" end
local function nat_proto(val) 
    local tmp = val

    if not tmp then
        return "ALL"
    end   
    
    local val_l = val:lower()
    if val_l ~= "all" and val_l ~= "tcp" and val_l ~= "udp" then
        return "ALL"
    end

    return tmp
end

local function nat_uci_cursor()
    --local nat_uci = uci_m.cursor()
    --form_m.set_uci_cursor(nat_uci)
    return nat_uci
end

local function max_rules_get(mod, prof)
    local uci = nat_uci_cursor()
    return uci:get_profile(mod, prof)
end

--- Check max rules
-- @param mod  config name
-- @param cfg  section type
-- @param prof spec name of profile
local function max_cnt_check(mod, cfg, prof)
    local max = max_rules_get(mod, prof) or 32
    local cur = form_m:count(mod, cfg)
    
    if cur >= max then
        return false
    end
    return true
end
 
--- Get config by section type
-- @param mod config name
-- @param cfg section type
-- @return table of all config
local function nat_uci_get(mod, cfg)
    local data = {}
    local uci = nat_uci_cursor()

    uci:foreach(mod, cfg, 
        function(section)
            data[#data+1] = uci:get_all(mod, section[".name"])            
        end    
    )

    return data
end

--- Set nat on/off
-- @param cfg_type section type
-- @param enable value on/off
-- @return bool
local function nat_set(cfg,...)
    local en = (cfg.enable == "on") and "on" or "off"
    local hw_en = (cfg.hw_enable == "on") and "on" or "off"

    local uci = nat_uci_cursor()

    local config = {}
    config = nat_uci_get(module_name, "nat_global")

    for _, item in ipairs(config) do
        if item.enable ~= en or item.hw_enable ~= hw_en then
            uci:set(module_name, item[".name"], "enable", en)
            uci:set(module_name, item[".name"], "hw_enable", hw_en)
            uci:commit(module_name)
            -- NAT Enable or Disable would make SPI Firewall On or Off
            local bsec = require "luci.controller.admin.security_settings"
            bsec.security_spi_operation(en)
            return cfg      
        end
    end

    return cfg
end


local function nat_profile_get(name)
    local uci = nat_uci_cursor()
    return uci:get_profile(module_name, name)
end

--- Get nat switch: on/off
-- @param cfg_type section type
local function nat_get(ctype)
    local data = { enable = "", hw_enable = "", reboot_time = 75, boost_iptv_compatible = "" }
    
    local uci = nat_uci_cursor()
    data.reboot_time = uci:get_profile("global", "reboot_time") or 75

    local cfg = {}
    cfg = nat_uci_get(module_name, ctype)

    for _, item in ipairs(cfg) do
        data.enable = item.enable or "on"
        data.hw_enable = item.hw_enable or "on"
        break
    end
    
	data.boost_iptv_compatible = nat_profile_get("boost_iptv_compatible") or "yes";
	
    return data
end

--- Get forward virtual server rules
local function forward_vs_rule_get(ctype)
    local data = {}

    local rules = {}
    rules = nat_uci_get(module_name, ctype)

    for i, r in ipairs(rules) do
        local tmp = {
            name = "",
            enable = "",
            external_port = "",
            internal_port = "",
            protocol = "",
            ipaddr = ""
        }
        tmp.name = r.name or ""
        tmp.enable = nat_on_off(r.enable)
        tmp.external_port = r.external_port
        tmp.internal_port = r.internal_port or ""
        tmp.protocol = nat_proto(r.protocol)
        tmp.ipaddr = valid_m.ip4addr(r.ipaddr) and r.ipaddr or "0.0.0.0"

        data[i] = tmp
    end

    return data
end

local function conflict_ports(p1, p2)
    local lo, hi = p1:match("(%d+)-(%d+)")
    if not lo then
        lo = p1
        hi = lo        
    end 

    if p2 >= lo and p2 <= hi then
        return true
    end
    
    return false
end

local function vs_conflict_remote_mngr(data)
    local uci = nat_uci_cursor()
    local en = uci:get("administration", "remote", "enable")
    if en ~= "on" or data.protocol:upper() == "UDP" then
        return false    
    end
    return conflict_ports(data.external_port, uci:get("administration", "remote", "port"))
end

function remote_mngr_conflict(port)  
    if not port then
        return false
    end

    local data = forward_vs_rule_get("rule_vs")
    for _, d in ipairs(data) do
        if d.protocol:upper() ~= "UDP" then
            local res = conflict_ports(d.external_port, port)
            if res then
                return true
            end
        end
    end
    return false
end

--- Add one rule of virtual server
local function forward_vs_rule_add(new, ...)
    if not max_cnt_check(module_name, "rule_vs", "max_vs_rules") then
        err("Reach max virtual server rules")
        return false, "Reach max virtual server rules"
    end

    --if vs_conflict_remote_mngr(new) then
    --    return false, "The entry is conflicted with remote manager"
    --end

    local uci = nat_uci_cursor()
    local res = form_m:insert(module_name, "rule_vs", new, NAT_FORWARD_KEYS.vs)
    if res then
        uci:commit(module_name)
    end
    return res
end

--- Update one rule of virtual server
local function forward_vs_rule_update(new, old)
    --if vs_conflict_remote_mngr(new) then
    --    return false, "The entry is conflicted with remote manager"
    --end    
    
    local uci = nat_uci_cursor()
    local res = form_m:update(module_name, "rule_vs", old, new, NAT_FORWARD_KEYS.vs)
    if res then
        uci:commit(module_name)
    end
    return res
end

--- Delete one or more rules of virtual server
local function forward_vs_rule_del(key, index)
    local uci = nat_uci_cursor()
    local res = form_m:delete(module_name, "rule_vs", key, index)
    if res then
        uci:commit(module_name)
    end
    return res
end

--- Convert external port table to UI string
local function pt_ex_port_tbl2str(ports)
    local port_str = ""

    if type(ports) == "table" then
        for _, v in ipairs(ports) do
            if v ~= "" then
                port_str = port_str .. v .. ","
            end
        end
        port_str = string.gsub(port_str, ",$", "")
    else
        port_str = ports
    end

    return port_str
end

--- Get port trigger rules
local function forward_pt_rule_get(ctype)
    local data = {}

    local rules = {} 
    rules = nat_uci_get(module_name, ctype)

    for i, r in ipairs(rules) do
        local tmp = {
            name = "",
            enable = "",
            trigger_port = "",
            trigger_protocol = "",
            external_port = "",
            external_protocol = ""
        }
        tmp.name = r.name or ""
        tmp.enable = nat_on_off(r.enable)
        tmp.trigger_port = valid_m.port(r.trigger_port) and r.trigger_port or ""
        tmp.trigger_protocol = nat_proto(r.trigger_protocol)
        tmp.external_port = pt_ex_port_tbl2str(r.external_port)
        tmp.external_protocol = nat_proto(r.external_protocol)

        data[i] = tmp
    end
    
    return data
end

--- Add one rule of port trigger
local function forward_pt_rule_add(new, ...)
    local data = {}

    if not max_cnt_check(module_name, "rule_pt", "max_pt_rules") then
        err("Reach max port triggering rules")
        return false
    end

    local uci = nat_uci_cursor()

    data = form_m:insert(module_name, "rule_pt", new,  NAT_FORWARD_KEYS.pt)
    if data then
        uci:commit(module_name)
        if next(data) then    
            data.external_port = pt_ex_port_tbl2str(data.external_port)
        end
    end

    return data
end

--- Update one rule of port trigger
local function forward_pt_rule_update(new, old)
    local data = {} 
    
    local uci = nat_uci_cursor()
   
    data = form_m:update(module_name, "rule_pt", old, new,  NAT_FORWARD_KEYS.pt)

    if data then
        uci:commit(module_name)
        if next(data) then    
            -- Convert external port to UI format
            data.external_port = pt_ex_port_tbl2str(data.external_port)
        end
    end

    return data
end

--- Delete one or more rules of port trigger
local function forward_pt_rule_del(key, index)
    local uci  = nat_uci_cursor()
    local data = form_m:delete(module_name, "rule_pt", key, index)
    if data then
        uci:commit(module_name)
    end
    return data
end

--- Get nat dmz config
local function dmz_get(ctype)
    local data = {
        enable = "", ipaddr = ""    
    }
    
    local cfg = {} 
    cfg = nat_uci_get(module_name, ctype)

    for _, c in ipairs(cfg) do
        data.enable = nat_on_off(c.enable)
        data.ipaddr = valid_m.ip4addr(c.ipaddr) and c.ipaddr or ""
        data.ipaddr = data.ipaddr == "0.0.0.0" and "" or data.ipaddr
        break
    end
    
    return data
end

--- Set nat dmz config
local function dmz_set(cfg, ...)
    local uci = nat_uci_cursor()

    if type(cfg) ~= "table" or not cfg.ipaddr then
        return false
    end

    local config = {}
    config = nat_uci_get(module_name, "nat_dmz")

    for _, c in ipairs(config) do
        if cfg.enable ~= c.enable or cfg.ipaddr ~= c.ipaddr then
            uci:set(module_name, c[".name"], "enable", nat_on_off(cfg.enable))
            uci:set(module_name, c[".name"], "ipaddr", cfg.ipaddr)
            uci:commit(module_name)
            return cfg        
        end     
    end

    return cfg
end

--- Get nat alg config
local function alg_get(ctype)
    local data = {
        ftp = "",  tftp = "",  h323 = "", rtsp = "",
        pptp = "", l2tp = "", ipsec = ""    
    }
    
    local cfg = {} 
    cfg = nat_uci_get(module_name, ctype)

    for _, c in ipairs(cfg) do
        data.ftp   = nat_on_off(c.ftp)
        data.tftp  = nat_on_off(c.tftp)
        data.h323  = nat_on_off(c.h323)
        data.rtsp  = nat_on_off(c.rtsp)
        data.pptp  = nat_on_off(c.pptp)
        data.l2tp  = nat_on_off(c.l2tp)
        data.ipsec = nat_on_off(c.ipsec)
        break
    end
    
    return data
end

--- Set nat alg config
local function alg_set(cfg, ...)
    local uci = nat_uci_cursor()

    if type(cfg) ~= "table" then
        return false
    end

    local config = {}
    config = nat_uci_get(module_name, "nat_alg")

    for _, c in ipairs(config) do
        if cfg.ftp ~= c.ftp or cfg.tftp ~= c.tftp 
            or cfg.h323 ~= c.h323 or cfg.rtsp ~= c.rtsp 
                or cfg.pptp ~= c.pptp or cfg.l2tp ~= c.l2tp 
                    or cfg.ipsec ~= c.ipsec 
        then
            uci:set(module_name, c[".name"], "ftp", nat_on_off(cfg.ftp))
            uci:set(module_name, c[".name"], "tftp", nat_on_off(cfg.tftp))
            uci:set(module_name, c[".name"], "h323", nat_on_off(cfg.h323))
            uci:set(module_name, c[".name"], "rtsp", nat_on_off(cfg.rtsp))
            uci:set(module_name, c[".name"], "pptp", nat_on_off(cfg.pptp))
            uci:set(module_name, c[".name"], "l2tp", nat_on_off(cfg.l2tp))
            uci:set(module_name, c[".name"], "ipsec", nat_on_off(cfg.ipsec))
            uci:commit(module_name)
            return cfg        
        end     
    end

    return cfg
end

-- Class of nat
NAT_INST = utl_m.class()

-- Mapping nat function operation to url request
local NAT_CFG_MAP = {
    -- Top switch to control nat
    ["global"] = { 
        ctype = "nat_global",
        max_cnt = 1,
        check = false,
        opt = {
            get = nat_get,
            write = nat_set       
        }    
    },
    -- Forward rule of virtual server
    ["vs"] = {
        ctype = "rule_vs",
        max_cnt = max_rules_get, -- max rules?
        check = false,
        opt = {
            get = forward_vs_rule_get,
            set = false,
            insert = forward_vs_rule_add,
            update = forward_vs_rule_update,
            remove = forward_vs_rule_del
        }
    },
    -- Forward rule of port trigger
    ["pt"] = {
        ctype = "rule_pt",
        max_cnt = max_rules_get, -- max rules?
        check = false,
        opt = {
            get = forward_pt_rule_get,
            insert = forward_pt_rule_add,
            update = forward_pt_rule_update,
            remove = forward_pt_rule_del
        }  
    },
    -- Nat DMZ
    ["dmz"] = {
        ctype = "nat_dmz",
        max_cnt = 1,
        check = false,
        opt = {
            get = dmz_get,
            write = dmz_set
        }   
    },
    -- Nat ALG
    ["alg"] = {
        ctype = "nat_alg",
        max_cnt = 1,
        check = false,
        opt = {
            get = alg_get,
            write = alg_set
        }
    }
}

function NAT_INST:__init__()
    self.module = "nat"

    self.uci = nat_uci_cursor()
    self.uci_get = self.uci.get
    self.uci_set = self.uci.set
    
    self.data = NAT_CFG_MAP
end

function NAT_INST:apply(mod, syn)
    --sys_m.call(NAT_SHELL .. "stop" .. " >/dev/null 2>&1")
    --sys_m.call(FW_SHELL .. "restart")
    local which = mod and mod or ""
    local asyn = syn and "" or " &"
    -- If nat setting operation: on/off, it needs to restart all nat modules
    which = (which == "nat") and "" or which
    if which ~= "" then
    os.execute(NAT_SHELL .. "restart " .. which .. NAT_DEV_NULL .. asyn)
    else
        os.execute("{ " .. NAT_SHELL .. "restart; " .. UPNP_SHELL .. "restart; }" .. NAT_DEV_NULL .. asyn)
    end
end

function NAT_INST:max_rules(form)
    local prof = {
        vs = {
            default = NAT_VS_MAX_RULES,
            conf = "nat",
            cont = "max_vs_rules"        
        },

        pt = {
            default = NAT_PT_MAX_RULES,
            conf = "nat",
            cont = "max_pt_rules" 
        }
    }
    if form then
        return self.data[form].max_cnt(prof[form].conf, prof[form].cont)
    else
        return prof[form].default
    end
end

function NAT_INST:get(form)
    if form then
        return self.data[form].opt.get(self.data[form].ctype)    
    end
end

--- Operation for nat and forwarding functions
-- @param form which module
-- @param opt operation of form
-- @param cfg form value
function NAT_INST:operate(form, op, ...)
    local oper = self.data[form].opt[op]
    if not oper then
        return false, "Invalid operation"
    end

    local args = {}
    for _, tbl in ipairs({...}) do
        args[#args + 1] = tbl
    end
    
    return oper(args[1] or {}, args[2] or {})
end


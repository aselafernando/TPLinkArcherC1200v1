--[[
Copyright(c) 2013 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  dhcp_server.lua
Details :  controller for dhcpServer.html webpage
Author  :  Guo Dongxian <guodongxian@tp-link.com.cn>
Author  :  Ye Qianchuan <yeqianchuan@tp-link.net>
Author  :  Wen Kun <wenkun@tp-link.net>
Version :  1.0.0
Date    :  21Feb, 2014
]]--

local uci   = require "luci.model.uci"
local sys   = require "luci.sys"
local form  = require "luci.tools.form"
local dtype = require "luci.tools.datatypes"
local nw    = require "luci.model.nwcache"
local ipm   = require "luci.ip"

local dbg   = require "luci.tools.debug"

module("luci.controller.admin.dhcps", package.seeall)

local DHCPS_MAX_RSVD_HOST = 32

local DHCPS_SHELL = "/etc/init.d/dnsmasq"
local RELOAD = "reload"

local uci_r = uci.cursor()
form = form.Form(uci_r)

local function recalc_pool()
    local lan_ip  = uci_r:get("network", "lan", "ipaddr")
    local netmask = uci_r:get("network", "lan", "netmask")

    local cidr_ip = ipm.IPv4(lan_ip, netmask):network()
    local cidr_broad = ipm.IPv4(lan_ip, netmask):broadcast()

    if not cidr_ip or not cidr_broad then
        return false
    else
        if tonumber((cidr_broad - cidr_ip)[2][2]) ~= 255 then
            local new_limit = cidr_broad - cidr_ip
            new_limit = tonumber(new_limit[2][2]) - 2
            if new_limit < 0 then
                new_limit = 0
            end
            uci_r:set("dhcp", "lan", "start", 2)
            uci_r:set("dhcp", "lan", "limit", new_limit)
            return true
        else
            -- Use the default ip pool range
            uci_r:set("dhcp", "lan", "start", 100)
            uci_r:set("dhcp", "lan", "limit", 100)
            return true
        end
    end 
    return false
end

local function calc_ipaddr_pool()
    local nw  = nw.init()
    local net = nw:get_network("lan")
    local lan_ip = net:ipaddr()
    local netmask = net:netmask()

    local start = uci_r:get("dhcp", "lan", "start")
    local limit = uci_r:get("dhcp", "lan", "limit")

    local cidr_ip = ipm.IPv4(lan_ip, netmask):network()

    if not cidr_ip then
        return false
    else
        local start_ip = cidr_ip + tonumber(start)
        local end_ip

        if tonumber(limit) > 0 
        then 
            end_ip = start_ip + (tonumber(limit) - 1)
        else 
            end_ip = start_ip + tonumber(limit)
        end

        local ippool = {
            startip = start_ip:string(),
            endip   = end_ip:string()
        }

        return ippool
    end
end

local function calc_pool_region(startip, endip)
    local lanip  = uci_r:get("network", "lan", "ipaddr")
    local netmask = uci_r:get("network", "lan", "netmask")

    local network = ipm.IPv4(lanip, netmask):network() -- get network address
    local broadcast = ipm.IPv4(lanip, netmask):broadcast() -- get broadcast address

    local cidr_startip = ipm.IPv4(startip, netmask)
    local cidr_endip   = ipm.IPv4(endip, netmask)
    
    if cidr_startip:network():string() ~= network:string() or
           cidr_endip:network():string() ~= network:string() 
    then 
        return false -- Not the same subnet
    end

    local a = string.match(endip, ".(%d+)$")
    local b = string.match(broadcast:string(), ".(%d+)$")
    if tonumber(a) >= tonumber(b) then
        return false -- Invalid ip pool range
    end

    local start = cidr_startip - network     -- start = startip - network
    local limit = cidr_endip - startip       -- limit = endip - startip
    if not start or not limit then
        return false
    end
    
    -- plus one here for address pool
    local new_limit = limit[2][2] + 1
    return start[2][2], new_limit
end

local function get_dhcp_options()
    local options = uci_r:get("dhcp", "lan", "dhcp_option")
    local data = {}

    if not options then
        return false
    end

    for _, op in ipairs(options) do
        local op_code = op:match("(%d+),")
        if op_code == "3" then
            data.gateway = op:match(",(.+)") or ""
        elseif op_code == "6" then
            local op1, op2 = op:match(",(.+),%s*(.+)")
            data.dns = {op1, op2} 
        elseif op_code == "15" then
            data.domain = op:match(",(.+)") or ""
        end
    end

    return data
end

-- Update the options of dhcp server
function dhcp_opt_update(ipaddr)
    if not dtype.ipaddr(ipaddr) then
        return false
    end

    local chg = recalc_pool() -- Reset start ipaddr and range limit

    local options = get_dhcp_options()
    if options then
        -- Only lan ip address different form current dhcps gateway, will update
        if ipaddr ~= options.gateway then
            uci_r:delete("dhcp", "lan", "dhcp_option")  
            uci_r:set_list("dhcp", "lan", "dhcp_option", "3," .. ipaddr)
            if options.dns then
                local pri_dns, snd_dns
                pri_dns = options.dns[1] and options.dns[1] or "0.0.0.0"
                snd_dns = options.dns[2] and options.dns[2] or "0.0.0.0"
                if pri_dns ~= "0.0.0.0" or snd_dns ~= "0.0.0.0" then
                    uci_r:set_list("dhcp", "lan", "dhcp_option", "6," .. pri_dns .. "," .. snd_dns)
                end
            end
            if options.domain then
                uci_r:set_list("dhcp", "lan", "dhcp_option", "15," .. options.domain) 
            end
            chg = true      
        end
    end

    if chg then
        uci_r:commit("dhcp")
    end
end

--- Get static dhcp leases from configuration
function load_static_leases()
    local static_leases = {}

    local leases = {}
    -- get static routes info from network configuration
    uci_r:foreach("dhcp", "host",
        function(section)
            leases[#leases + 1] = uci_r:get_all("dhcp", section[".name"])
        end
    )

    for k, rt in ipairs(leases) do
        local data = {}
        local mac = (rt.mac):gsub(":", "-")
        data.ip   = rt.ip
        data.mac  = mac:upper()
        data.comment = rt.comment
        data.enable = (rt.enable == "on") and "on" or "off"
        static_leases[k] = data
    end

    return static_leases
end

--- Get dhcp active leases or client list
function load_active_leases()
    local client_list = {}

    local s = require "luci.tools.status"
    local leases = s.dhcp_leases()
    for k, rt in ipairs(leases) do
        local data = {}
        local macaddr  = (rt.macaddr):gsub(":", "-")
        data.macaddr   = macaddr:upper()
        data.name      = rt.hostname or "--" 
        data.ipaddr    = rt.ipaddr
        
        if rt.expires < 0 then
            data.leasetime = "Permanent"
        else
            local hour = math.floor(rt.expires / 3600)
            local min = math.floor(rt.expires / 60) - hour * 60
            local sec = rt.expires - hour * 3600 - min * 60
            data.leasetime = hour .. ":" .. min .. ":" .. sec
        end
        
        client_list[k] = data
    end

    return client_list
end

function get_dhcp_settings()
    local ippool = calc_ipaddr_pool()
    local options = get_dhcp_options()
    local settings = {}

    local leasetime = uci_r:get("dhcp", "lan", "leasetime")
    local num, unit = leasetime:match("(%d+)(%l)$")

    if unit == "h" then
        num = tonumber(num) * 60
    end

    settings.leasetime = tostring(num)
    settings.ipaddr_start = ippool.startip
    settings.ipaddr_end   = ippool.endip
    if options then
        settings.gateway = options.gateway and options.gateway or ""
        settings.domain  = options.domain and options.domain or ""
        if options.dns then
            settings.pri_dns = options.dns[1] and options.dns[1] or ""
            settings.pri_dns = settings.pri_dns == "0.0.0.0" and "" or settings.pri_dns
            settings.snd_dns = options.dns[2] and options.dns[2] or ""
            settings.snd_dns = settings.snd_dns == "0.0.0.0" and "" or settings.snd_dns
        else
            settings.pri_dns = ""
            settings.snd_dns = ""
        end
    else
        settings.gateway = ""
        settings.domain  = ""
        settings.pri_dns = ""
        settings.snd_dns = ""
    end

    settings.enable  = ("1" == uci_r:get("dhcp", "lan", "ignore")) and "off" or "on"

    return settings
end

function set_dhcp_settings(http_form)
    -- get settings from webpage
    local leasetime = http_form["leasetime"] or ""
    local start_ip  = http_form["ipaddr_start"]
    local end_ip    = http_form["ipaddr_end"] or start_ip
    local status    = http_form["enable"] or ""
    local gateway   = http_form["gateway"]
    local domain    = http_form["domain"]
    local pri_dns   = http_form["pri_dns"] or "0.0.0.0"
    local snd_dns   = http_form["snd_dns"] or "0.0.0.0"
    
    if not start_ip then
        return false, "Start ip can not blank"
    end

    local start, limit = calc_pool_region(start_ip, end_ip)
    if not start then
        return false, "Ip address pool is invalid"
    end

    -- set settings to dhcp config file
    local log = require("luci.model.log").Log(212)
    if status == "off" then
        log(505)
        uci_r:set("dhcp", "lan", "ignore", "1")
    else
        log(504)
        uci_r:set("dhcp", "lan", "ignore", "0")
    end
    leasetime = (tonumber(leasetime) > 2880) and "2880" or leasetime

    uci_r:set("dhcp", "lan", "leasetime", leasetime .. "m") -- leasetime unit is minutes
    uci_r:set("dhcp", "lan", "start", start)
    uci_r:set("dhcp", "lan", "limit", limit)
    uci_r:set("dhcp", "lan", "dynamicdhcp", "1") -- open dynamicdhcp by default
    uci_r:delete("dhcp", "lan", "dhcp_option")
   
    if dtype.ipaddr(gateway) then 
        uci_r:set_list("dhcp", "lan", "dhcp_option", "3," .. gateway) -- set dhcp option gateway
    end
    pri_dns = (pri_dns == "") and "0.0.0.0" or pri_dns
    snd_dns = (snd_dns == "") and "0.0.0.0" or snd_dns
    if dtype.ipaddr(pri_dns) and dtype.ipaddr(snd_dns) then 
        uci_r:set_list("dhcp", "lan", "dhcp_option", "6," .. pri_dns .. "," .. snd_dns) -- dhcp option dns server address
    end
    if dtype.host(domain) then
        uci_r:set_list("dhcp", "lan", "dhcp_option", "15," .. domain) -- set dhcp option default domain
    end
    uci_r:commit("dhcp")

    return get_dhcp_settings()

end

--- Update a dhcp static lease selected by UI
function update_static_lease(http_form)
    local old = http_form["old"]
    local new = http_form["new"]

    local old = luci.json.decode(old)
    local new = luci.json.decode(new)

    if not new and type(new) ~= "table" or
        not old and type(old) ~= "table" then
        return {errorcode="invalid new params"}
    end

    old.mac = string.gsub(old.mac:upper(), "-", ":")
    new.mac = string.gsub(new.mac:upper(), "-", ":")
    if not dtype.macaddr(new.mac) or not dtype.ipaddr(new.ip) then
        return {errorcode="mac or ip address is invalid"}
    end

    local ret = form:update("dhcp", "host", old, new, {"ip", "mac"})
    if ret then ret.mac = string.gsub(ret.mac, ":", "-") end
    if ret then
        uci_r:commit("dhcp")
        return ret
    else
        return false, "modify item failed"
    end
end

function get_max_static_lease()
    return { ["max_rules"] = uci_r:get_profile("dhcps", "max_rsvd_host") or DHCPS_MAX_RSVD_HOST }
end

function static_lease_max_check()
    local max = get_max_static_lease()
    local cur = form:count("dhcp", "host")
    
    if cur >= max.max_rules then
        return false
    end
    
    return true
end

--- Insert a new dhcp static lease
function insert_static_lease(http_form)
    local new_lease = http_form["new"]
    local new = luci.json.decode(new_lease)

    if not new and type(new) ~= "table" then
        return {errorcode="invalid new params"}
    end

    if not static_lease_max_check() then
        return {errorcode="reach max items"}
    end

    new.mac = string.gsub(new.mac:upper(), "-", ":")
    if not dtype.macaddr(new.mac) or not dtype.ipaddr(new.ip) then
        return {errorcode="mac or ip address is invalid"}
    end

    local ret = form:insert("dhcp", "host", new, {"ip", "mac"})
    if ret then ret.mac = string.gsub(ret.mac, ":", "-") end
    if ret then
        uci_r:commit("dhcp")
        return ret
    else
        return false, "insert new items failed"
    end
end

--- Remove a dhcp static lease selected by UI
function remove_static_lease(http_form)
    local key = http_form["key"]
    local index = http_form["index"]

    local ret = form:delete("dhcp", "host", key, index)

    if ret then
        uci_r:commit("dhcp")
        return ret
    else
        return false, "remove static lease failed"
    end

end

--- Remove all dhcp static lease host
function remove_all_static_lease()
    local secs = {}

    uci_r:foreach("dhcp", "host",
        function(section)
            secs[#secs + 1] = section[".name"]
            --uci_r:delete("dhcp", section[".name"])
        end
    )   

    for _, s in ipairs(secs) do
        uci_r:delete("dhcp", s)
    end

    uci_r:commit("dhcp")

    return true
end

-- Disable all invalid host static lease host
-- Details:
-- We need to go through all the static lease host entries to check whether the 
-- entry is still valid when our network ipaddr/netmask changes. If any entry
-- isn't in the range of new masked network, we disable it instead of removing 
-- it.
-- If any parameter(ipaddr/maskaddr) is not provided, we will get it from uci.
function disable_all_invalid_static_lease(ipaddr, maskaddr)
    ipaddr   = ipaddr   or uci_r:get("network", "lan", "ipaddr")
    maskaddr = maskaddr or uci_r:get("network", "lan", "netmask")
    --[[
    dbg.printf("Para:Ipaddr  :" .. ipaddr)
    dbg.printf("Para:maskaddr:" .. maskaddr)
    --]]--
    uci_r:foreach("dhcp", "host",
        function(section)
            local cfg_ip = section["ip"]
            local cfg_network = ipm.IPv4(cfg_ip, maskaddr):network()
            local new_network = ipm.IPv4(ipaddr, maskaddr):network()
            --[[
            dbg.printf("CFG:IP:" .. cfg_ip)
            dbg.printf("CFG:NETWORK:" .. cfg_network:string())
            dbg.printf("NEW:NETWORK:" .. new_network:string())
            --]]--
            if new_network:string() ~= cfg_network:string() then
                uci_r:set("dhcp", section[".name"], "enable", "off")
            --[[
                dbg.printf("Turn Off this entry.")
            else
                dbg.printf("Keep Current config.")
            --]]--
            end
        end
    )   

    uci_r:commit("dhcp")
end

-- General controller routines

local dhcps_forms = {
    setting = {
        ["read"] = {cb = get_dhcp_settings},
        ["write"] = {cb = set_dhcp_settings, cmd = RELOAD}
    },
    client = {
        ["load"] = {cb = load_active_leases}
    },
    reservation = {
        ["load"] = {cb = load_static_leases, others = get_max_static_lease},
        ["insert"] = {cb = insert_static_lease, others = get_max_static_lease, cmd = RELOAD},
        ["update"] = {cb = update_static_lease, others = get_max_static_lease, cmd = RELOAD},
        ["remove"] = {cb = remove_static_lease, others = get_max_static_lease, cmd = RELOAD},
        ["clear"]  = {cb = remove_all_static_lease}--cmd=RELOAD
    }
}

function index()
    entry({"admin", "dhcps"}, call("dhcp_index")).leaf = true
end

local ctl = require "luci.model.controller"

function dhcp_index()
    ctl._index(dhcp_dispatch)
end

function dhcp_dispatch(http_form)
    local function hook(success, action)
        if success and action.cmd and action.cmd ~= "" then
            sys.fork_exec("%s %s" % {DHCPS_SHELL, action.cmd})
        end
        return true
    end
    return ctl.dispatch(dhcps_forms, http_form, {post_hook = hook})
end

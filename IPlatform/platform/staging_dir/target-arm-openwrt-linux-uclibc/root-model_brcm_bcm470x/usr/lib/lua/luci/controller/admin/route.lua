--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  network.lua
Details :  Controller for advanced routing
Author  :  Zhu Xianfeng<yeqianchuan@tp-link.net>
Version :  1.0.0
Date    :  29 May, 2014
]]--

module("luci.controller.admin.route", package.seeall)

local ip     = require "luci.ip"
local sys    = require "luci.sys"
local uci    = require "luci.model.uci"
local dbg    = require "luci.tools.debug"
local ctypes = require "luci.model.checktypes"
local nw     = require "luci.model.nwcache"
local ctl    = require "luci.model.controller"

local uci_r  = uci.cursor()
local form   = require("luci.tools.form").Form(uci_r)

--- Get network name by interface
local function iface_get_network(iface)
    local state = require("luci.model.uci").cursor_state()
    state:load("network")
    local net
    state:foreach("network", "interface",
        function (section)
            local ifname = state:get("network", section['.name'], "ifname")
            if iface == ifname then
                net = section[".name"]
            end
        end
        )
    return net
end

local function iface_all_ipaddrs()
    local nw      = nw.init()
    local ifaces  = {"lan", "wan", "internet"}
    local data    = {}
    local net     = nil
    local ipaddr  = nil
    local netmask = nil

    for _, iface in ipairs(ifaces) do
        net = nw:get_network(iface)
        if net then
            ipaddr  = net:ipaddr()
            netmask = net:netmask()
            if ipaddr and netmask then
                table.insert(data, {ipaddr = ipaddr, netmask = netmask})
            end
        end
    end

    return data
end

--- Check whether static route rule is valid
local function check_rule_extra(rule)
    if not ctypes.check_network(rule.target, rule.netmask) then
        return false
    end

    local ipaddrs = iface_all_ipaddrs()
    local dstaddr = ip.IPv4(rule.target, rule.netmask)

    for _, v in ipairs(ipaddrs) do
        local ifaddr = ip.IPv4(v.ipaddr, v.netmask)

        if dstaddr and ifaddr then
            if dstaddr:contains(ifaddr) or ifaddr:contains(dstaddr) then
                dbg("Target network is conflicting with Wan/Lan IP " .. ifaddr:string())
                return false
            end
        end
    end

    return true
end

local function route_equal(a, b)
    if a.target  == b.target and 
       a.netmask == b.netmask and 
       a.gateway == b.gateway
    then
        -- same equal
        return true
    end

    return false
end

local function route_newkey()
    local now = os.time()

    math.randomseed(now)
    return tostring(now) .. "-" .. tostring(math.random(1, 1024))
end

local function check_duplicated(new, old)
    local matched = false

    if type(new) ~= "table" then
        return matched
    end

    uci_r:foreach("network", "route",
        function(section)
            local ignore = false

            if matched then
                return
            end

            if old then
                if route_equal(old, section) then
                    -- ignore old route entry
                    ignore = true
                end
            end

            if not ignore and route_equal(new, section) then
                matched = true
            end
        end
    )

    return matched
end

local CHECK_RULE_TBL = {
    {
        field = "target", canbe_absent = false,
        check = {
        }
    },
    {
        field = "netmask", canbe_absent = false,
        check = {
            ctypes.check_netmask_loose
        }
    },
    {
        field = "gateway", canbe_absent = false,
        check = {
            ctypes.check_ipv4
        }
    },
    {
        field = "name", canbe_empty = true,
        check = {
            ctypes.check_visible,
            {ctypes.check_rangelen, 0, 32 * 4},
        }
    },
    {
        field = "interface",
        check = {
            {ctypes.check_in, {"LAN", "WAN"}}
        }
    },
    {
        field = "enable",
        check = {
            ctypes.check_onoff
        }
    },
    --[[
    {
        check = {
            check_rule_extra
        }
    }
    ]]--
}

function route_cfg_max()
    local max_routes = uci_r:get_profile("route", "max_static_routes")

    return {max_rules = max_routes}
end

--- Get static routes table from configuration
function route_cfg_list()
    local static_routes = {}
    local routes = {}

    -- get static routes info from network configuration
    uci_r:foreach("network", "route",
        function(section)
            routes[#routes + 1] = uci_r:get_all("network", section[".name"])
        end
    )

    for k, rt in ipairs(routes) do
        local data = {}
        data.key     = rt.key
        data.target  = rt.target
        data.netmask = rt.netmask
        data.gateway = rt.gateway
        data.name    = rt.name     -- TBD name ? interface
        data.enable  = (rt.enable == "1") and "on" or "off"
        data.interface = rt.interface:upper()
        static_routes[k] = data
    end

    return static_routes
end

--- Get system routes table from /proc/net/route
function route_sys_list()
    local sys_routes = {}

    local routes = sys.net.routes()

    for k, rt in ipairs(routes) do
        local data = {}
        local dest = rt.dest
        data.dest      = (dest:string()):gsub("/.+","")
        data.mask      = (dest:mask()):string()
        data.gateway   = rt.gateway:string()
        data.interface = iface_get_network(rt.device) or rt.device
        sys_routes[k] = data
    end

    return sys_routes
end

function route_insert(http_form)
    local new = http_form and http_form.new or false
    local ret

    if new then
        new = luci.json.decode(new)
    else
        return false, "invalid args"
    end

    if not ctl.check(new, CHECK_RULE_TBL) then
        return false, "invalid args"
    end

    local max_routes = uci_r:get_profile("route", "max_static_routes")
    local count = 0

    uci_r:foreach("network", "route",
        function(section)
            count = count + 1
        end
    )

    max_routes = tonumber(max_routes)
    if count >= max_routes then
        return false, "max route entries"
    end

    new.key = route_newkey()
    new.enable = (new.enable == "on") and "1" or "0"
    new.interface = new.interface:lower()

    if check_duplicated(new) then
        return false, "duplicated route entry"
    end

    ret = form:insert("network", "route", new, nil)
    if ret then
        uci_r:commit("network")
        ret.interface = ret.interface:upper()
        ret.enable = (ret.enable == "1") and "on" or "off"
        return ret
    else
        return false, "insert static routes failed."
    end
end

function route_delete(http_form)
    local key = http_form.key
    local index = http_form.index

    local ret = form:delete("network", "route", key, index)
    uci_r:commit("network")

    if ret then
        return ret
    else
        return false, "delete static routes failed."
    end
end

function route_update(http_form)
    local old = http_form and http_form.old or false
    local new = http_form and http_form.new or false
    local ret

    if old and new then
        old = luci.json.decode(old)
        new = luci.json.decode(new)
    else
        return false, "invalid args"
    end

    if not ctl.check(new, CHECK_RULE_TBL) then
        return false, "invalid args"
    end

    old.enable = (old.enable == "on") and "1" or "0"
    old.interface = string.lower(old.interface)

    new.enable = (new.enable == "on") and "1" or "0"
    new.interface = string.lower(new.interface)

    if check_duplicated(new, old) then
        return false, "duplicated route entry"
    end

    ret = form:update("network", "route", old, new, {"target", "netmask", "gateway"})
    if ret then
        uci_r:commit("network")
        ret.interface = string.upper(ret.interface)
        ret.enable = (ret.enable == "1") and "on" or "off"
        return ret
    else
        return false, "update static routes failed."
    end
end

function index()
end

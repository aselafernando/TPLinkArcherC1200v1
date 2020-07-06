--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  checktypes.lua
Details :  Subroutines for validating data types
Author  :  Ye Qianchuan <yeqianchuan@tp-link.net>
Version :  1.0.0
Date    :  13 Mar, 2014
]]--

module("luci.model.checktypes", package.seeall)

require "luci.ip"
local dtypes = require "luci.tools.datatypes"
local util   = require "luci.util"
local bit    = require "bit"
local dbg    = require "luci.tools.debug"

local function check_multi(cb, ...)
    for i = 1, select('#', ...) do
        local v = select(i, ...)
        if not cb(v) then
            return false
        end
    end
    return true
end

function check_nonempty(v)
    if v == nil then
        return false
    elseif type(v) == "string" then
        return #v > 0
    else
        return true
    end
end

function check_nonemptys(...)
    return check_multi(check_nonempty, ...)
end

function check_ipv4(ip)
    return dtypes.ip4addr(ip)
end
function check_ipv4s(...)
    return check_multi(check_ipv4, ...)
end

function check_ipv6(ip)
    return dtypes.ip6addr(ip)
end
function check_ipv6s(...)
    return check_multi(check_ipv6, ...)
end

function check_ip(ip)
    return dtypes.ipaddr(ip)
end
function check_ips(...)
    return check_multi(check_ip, ...)
end

local function mac_helper(mac)
    mac = mac:gsub("-", ":")
    if not mac or not mac:match(
        "^[a-fA-F0-9]+:[a-fA-F0-9]+:[a-fA-F0-9]+:" ..
            "[a-fA-F0-9]+:[a-fA-F0-9]+:[a-fA-F0-9]+$")
    then
        return false
    end

    local parts = util.split(mac, ":")
    for i = 1,6 do
        parts[i] = tonumber(parts[i], 16)
        if parts[i] < 0 or parts[i] > 255 then
            return false
        end
    end

    return parts
end

function check_mac(mac)
    return not not mac_helper(mac)
end
function check_macs(...)
    return check_multi(check_mac, ...)
end

function check_unicast_mac(mac)
    local parts = mac_helper(mac)
    if not parts then
        return false
    end

    -- Filter all 0 and all 1.
    local all0 = true
    local all1 = true
    for i = 1,6 do
        if parts[i] ~= 0 then
            all0 = false
        end
        if parts[i] ~= 255 then
            all1 = false
        end
    end

    if all0 or all1 then
        return false
    end

    -- Filter multicast address
    if bit.band(parts[1], 1) == 1 then
        return false
    end

    return true
end

function check_in(v, t)
    return util.contains(t, v)
end

function check_onoff(v)
    return check_in(v, {"on", "off"})
end

function check_onoffs(...)
    return check_multi(check_onoff, ...)
end

function check_3p(v)
    return check_in(v, {"pppoe", "l2tp", "pptp"})
end

function check_range(v, min, max)
    return dtypes.range(v, min, max)
end

function check_day_time(v)
    local hour, min = v:match("^(%d+):(%d+)$")
    local rc = true
    if hour then
        rc = rc and check_range(hour, 0, 23)
    end

    if min then
        rc = rc and check_range(min, 0, 59)
    end

    return rc
end

function check_day_time_lower(v1, v2)
    local h1, m1 = v1:match("^(%d+):(%d+)$")
    local h2, m2 = v2:match("^(%d+):(%d+)$")
    h1 = tonumber(h1)
    h2 = tonumber(h2)
    m1 = tonumber(m1)
    m2 = tonumber(m2)

    if h1 < h2 then
        return true
    elseif h1 > h2 then
        return false
    else
        if m1 < m2 then
            return true
        else
            return false
        end
    end
end

function check_hostname(v)
    return dtypes.hostname(v)
end

function check_host(v)
    return check_hostname(v) or check_unicast_ipv4(v)
end

function check_domain(v)
    if v and (#v < 254) and v:match("^[a-zA-Z0-9_%-%.]+$") then
        return true
    end

    return false
end

function check_visible(v)
    -- TODO: allow all visible characters.
    -- return dtypes.hostname(v)
    return true
end

function check_email(v)
    local user, host = v:match("^(.*)@(.*)$")
    return check_visible(user) and check_host(host)
end

function check_emails(...)
    return check_multi(check_email, ...)
end

function check_rangelen(v, min, max)
    return dtypes.rangelength(v, min, max)
end

function check_maxlen(v, max)
    return dtypes.maxlength(v, max)
end

function check_minlen(v, min)
    return dtypes.minlength(v, min)
end

function ip_to_number(ip)
    local b1, b2, b3, b4 = ip:match("^(%d+)%.(%d+)%.(%d+)%.(%d+)$")
    b1 = tonumber(b1)
    b2 = tonumber(b2)
    b3 = tonumber(b3)
    b4 = tonumber(b4)
    if b1 and b1 <= 255
        and b2 and b2 <= 255
        and b3 and b3 <= 255
        and b4 and b4 <= 255
    then
        return bit.lshift(b4, 0)
            + bit.lshift(b3, 8)
            + bit.lshift(b2, 16)
            + bit.lshift(b1, 24)
    else
        return false
    end

end

function number_to_ip(n)
    local b4 = bit.band(bit.rshift(n, 0), 0xFF)
    local b3 = bit.band(bit.rshift(n, 8), 0xFF)
    local b2 = bit.band(bit.rshift(n, 16), 0xFF)
    local b1 = bit.band(bit.rshift(n, 24), 0xFF)

    return "%d.%d.%d.%d" % {b1, b2, b3, b4}
end

function check_same_network(ip1, ip2, mask)
    ip1 = luci.ip.IPv4(ip1, mask)
    ip2 = luci.ip.IPv4(ip2, mask)
    return ip1 and ip2 and ip1:network() == ip2:network()
end

function check_network(nw, mask)
    nw = luci.ip.IPv4(nw, mask)
    return nw and nw:network() == nw
end

function check_network_no_collision(ip1, mask1, ip2, mask2)
    local mask = luci.ip.IPv4(mask1):lower(luci.ip.IPv4(mask2)) and mask1 or mask2
    return not check_same_network(ip1, ip2, mask)
end

function check_ipv4_equal(ip1, ip2)
    return luci.ip.IPv4(ip1) == luci.ip.IPv4(ip2)
end

function check_unicast_ipv4(ip)
    if not check_ipv4(ip) then
        return false
    end

    local fb = tonumber(ip:match("^(%d+)"))
    if fb >= 224 or fb == 127 or fb == 0
    then
        return false
    end

    return true
end

function check_unicast_ipv4_extra(ip, mask)
    ip = luci.ip.IPv4(ip, mask)
    local nw = ip:network()
    return nw ~= ip:mask()
        and nw ~= luci.ip.IPv4("0.0.0.0")
        and nw ~= ip
        and ip ~= ip:broadcast()
end

function check_unicast_ipv4_full(ip, mask)
    return check_unicast_ipv4(ip)
        and check_unicast_ipv4_extra(ip, mask)
end

function check_netmask(mask, loose)
    local uint32_max = 4294967295
    mask = ip_to_number(mask)
    if mask then
        if mask == 0 then
            return false
        end

        if not loose and mask == uint32_max then
            return false
        end

        mask = bit.band(bit.bnot(mask), uint32_max)
        return bit.band(mask + 1, mask) == 0
    else
        return false
    end
end

function check_netmask_loose(mask)
    return check_netmask(mask, true)
end

function check_unicast_ipv6(ip)
    return check_ipv6(ip)
        and ip:match("^[23]")
        and not ip:match("::$")
        and not ip:match("/%d*$")
end

-- Lan IPv6 prefix's length is 64
local function check_valid_ipv6_prefix(str)
    local reg1 = "^[23]%x%x%x:"
    local reg2 = "^%x%x?%x?%x?:"
    local reg3 = "^:$"

    local i = nil
    local j = nil
    local k = nil
    local count = 0

    i, j = string.find(str, reg1)
    if i == nil then
        return false
    end

    while true do
        k = j
        i, j = string.find(str, reg2, j + 1)
        if i then
            count = count + 1
        else
            i, j = string.find(str, reg3, k + 1)
            if i then
                return true
            else
                return false
            end
        end

        if count > 3 then
            break
        end
    end

    return false
end

function check_unicast_ipv6_prefix(ip, len)
    local valid = check_ipv6(ip)

    if not valid then
        return valid
    end

    return check_valid_ipv6_prefix(ip)
end

function check_ip_in_lan(ip)
    local uci_r = require("luci.model.uci").cursor()
    local vpn = require "luci.model.vpn"
    local lan_ipaddr = uci_r:get("network", "lan", "ipaddr")
    local lan_netmask = uci_r:get("network", "lan", "netmask")
    if vpn == nil then
    return check_same_network(ip, lan_ipaddr, lan_netmask)
    else
        return check_same_network(ip, lan_ipaddr, lan_netmask) and not vpn.check_ip_is_vpn_client(ip)
    end
end

function check_safe_request(request)
    local http = require "luci.http"
    local forms = http.formvalue("form") or ""
    forms = type(forms) == "table" and forms or {forms}
    local operation = http.formvalue("operation") or""
    local req = table.concat(request, ".")

    local block_tbl = {
        "admin.administration.account.write",
        "admin.firmware.config.factory",
        "admin.firmware.config.restore",
        "admin.firmware.config.backup",
        "admin.firmware.config.firmware",
        "admin.firmware.upgrade.firmware",
        "admin.firmware.upgrade.check",
        "admin.firmware.config.check",
		"admin.firmware.upgrade.fwup_check",
		"admin.firmware.config.fwup_check"
    }

    local stat = true
    for _, form in ipairs(forms) do
        stat = stat and not util.contains(block_tbl,
                                          req .. "." .. form
                                              .. "." .. operation)
        if req == "admin.network"
            and (form == "mac_clone_advanced" or form:match("^wan_ipv."))
            and operation ~= "read"
        then
            stat = false
        end
    end

    return stat
end

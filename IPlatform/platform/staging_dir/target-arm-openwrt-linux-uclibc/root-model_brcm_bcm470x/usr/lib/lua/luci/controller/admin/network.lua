--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  network.lua
Details :  Controller for internet webpage
Author  :  Zhu xianfeng <zhuxianfeng@tp-link.net>
Author  :  Ye Qianchuan <yeqianchuan@tp-link.net>
Author  :  Wen Kun <wenkun@tp-link.net>
Version :  1.0.0
Date    :  15 Jan, 2014
]]--

module("luci.controller.admin.network", package.seeall)

local nixio  = require "nixio"
local ip     = require "luci.ip"
local uci    = require "luci.model.uci"
local util   = require "luci.util"
local sys    = require "luci.sys"
local dbg    = require "luci.tools.debug"
local ctypes = require "luci.model.checktypes"
local nw     = require "luci.model.nwcache"
local route  = require "luci.controller.admin.route"
local ctl    = require "luci.model.controller"
local inetm  = require "luci.model.internet"
local logm   = require "luci.model.log"
local nlog   = require "luci.model.network_log"
local protom = require "luci.model.protodata"

local uci_r    = uci.cursor()
local internet = inetm.Internet()
local log      = logm.Log(nlog.ID)

local PROTO_TBL     = protom.PROTO_TBL
local PROTO_CFG_TBL = protom.PROTO_CFG_TBL

local PREFIX_PATH    = "/tmp/dhcp6c/prefix.info"
--local NETWORK_RELOAD = "/etc/init.d/network reload"
local NETWORK_RELOAD = "/etc/init.d/network restart"

--- Load protocol configuration by proto name
-- @pram proto_name  name of protocol
-- @return table of protocol
local function load_proto(proto_name)
    return uci_r:get_all("protocol", proto_name)
end

local function uci_commit()
    local stat = uci_r:commit("protocol", "network")
    if not stat then
        log(nlog.CONFIG_SAVE_FAIL)
    end
    return stat
end

local function invalid_args(errorcode)
    errorcode = errorcode or "invalid args"
    log(nlog.INVALID_ARGS)
    return false, errorcode
end

local PROTOS = {
    -- IPv4 Protos
    {ipv4 = true, key = "static",      value = "Static IP"},
    {ipv4 = true, key = "dhcp",        value = "Dynamic IP"},
    {ipv4 = true, key = "pppoe",       value = "PPPoE"},
    {ipv4 = true, key = "bigpond",     value = "BigPond Cable"},
    {ipv4 = true, key = "l2tp",        value = "L2TP"},
    {ipv4 = true, key = "pptp",        value = "PPTP"},
    -- IPv6 Protos
    {ipv6 = true, key = "staticv6",    value = "STATIC_IP"},
    {ipv6 = true, key = "dhcp6c",      value = "DYNAMIC_IP_v6"},
    {ipv6 = true, key = "pppoev6",     value = "PPPOE"},
    {ipv6 = true, key = "6to4",        value = "TUNNEL_6TO4"},
--    {ipv6 = true, key = "6rd",         value = "RD6"},
--    {ipv6 = true, key = "dslite",      value = "DSLITE"},
    {ipv6 = true, key = "passthrough", value = "PASS_THROUGH"},
}

local function get_valid_proto(key)
    local ipv4_protos = uci_r:get("protocol", "ipv4", "protos")
    local ipv6_protos = uci_r:get("protocol", "ipv6", "protos")
    local protos = {}
    local valid = nil

    key = key == "dhcpv6" and "dhcp6c" or key
    
    for _, proto in ipairs(PROTOS) do
        if proto.key == key then
            valid = proto
            break
        end
    end

    if valid then
        util.append(protos, unpack(ipv4_protos))
        util.append(protos, unpack(ipv6_protos))

        for _, v in ipairs(protos) do
            if v == valid.key then
                return valid
            end
        end
    end

    return nil
end

local function get_ipvx_protos(form, v6)
    local data = {}
    local protos = uci_r:get("protocol", v6 and "ipv6" or "ipv4", "protos")

    for _, v in ipairs(protos) do
        if v ~= "bigpond" or form.advanced ~= "false" then
            for _, proto in ipairs(PROTOS) do
                if proto.key == v then
                    table.insert(data, {name = proto.value, value = proto.key})
                    break
                end
            end
        end
    end

    return data
end

local function set_proto_cfg_lanv6(proto)
    local lanv6_proto = uci_r:get("protocol", "lanv6", "proto") or "slaac"
    local ip_mode = proto.ip_mode
    local ip_config = proto.ip_config
    local data = uci_r:get_all("protocol", lanv6_proto)
    if ip_config ~= "specified" and ip_mode == "prefix" then
        data.prefix = nil
    end
    uci_r:delete("network", "lanv6")
    uci_r:section("network", "interface", "lanv6", data)
end

local function set_proto_cfg(proto_name, operation)
    local cb = PROTO_CFG_TBL[proto_name]
    if not cb then
        return
    end

    local proto = load_proto(proto_name)
    local vldproto = get_valid_proto(proto_name)

    if operation == "disconnect" or operation == "release" then
        proto["connect"] = "0";
    else
        proto["connect"] = "1";
    end

    -- Delete old configs before updating.
    if vldproto.ipv6 then
        -- Save wanv6 connection type to protocol config.
        uci_r:set("protocol", "wanv6", "proto_name", proto_name)
        uci_r:set("protocol", "wanv6", "proto", PROTO_TBL[proto_name].wan_type)

        set_proto_cfg_lanv6(proto)

        if proto_name == "dhcpv6" then
            proto["connect"] = operation == "release" and "0" or "1"
	elseif proto_name == "passthrough"  then
           uci_r:delete("network", "lanv6")
        end
        uci_r:delete("network", "wanv6")
        uci_r:delete("network", "internetv6")
    else
        proto["macaddr"] = uci_r:get("network", "wan", "macaddr") or ""
        uci_r:delete("network", "wan")
        uci_r:delete("network", "internet")
    end

    -- Update new configs by proto_name.
    local data = cb(proto, proto_name)
    for k, v in pairs(data) do
        uci_r:section("network", "interface", k, v)
    end
end

--- Get wan ipv6 dynamic information from runing state
local function get_wanv6_dyninfo(cmd)
    local nw = nw.init()
    local net = cmd and nw:get_network("wanv6") or nil
    local dnsaddrs = net and net:dns6addrs() or {}
    local ip6gw = net and net:gw6addr() or "::"
    local ifname = uci_r:get("network", "wanv6", "ifname")

    if ip6gw == "::" and ifname then
        local ret = sys.exec("ip -6 route show default")
        ip6gw = ret:match("default via%s+([%x:]+)%s+dev " .. ifname) or "::"
    end

    local dyn6info = {
        ip6addr = net and net:ip6addr() or "::",
        ip6gw   = ip6gw,
        pri_dns = dnsaddrs[1] or "::",
        snd_dns = dnsaddrs[2] or "::",
    }

    --handle 6to4 DNS 
    local proto = uci_r:get("network", "wanv6", "proto") 
    if proto == "6to4" then 
        if dyn6info.pri_dns == "::" and dyn6info.snd_dns == "::" then 
            dyn6info.pri_dns = "2001:4860:4860::8888" 
            dyn6info.snd_dns = "2001:4860:4860::8844" 
        end 
    end 

    return dyn6info
end

--- Get wan dynamic information from running state
--@param interface 'wan' or 'internet'
--@param cmd  false or true
--@return dyninfo
local function get_wan_dyninfo(interface, cmd)
    local nw = nw.init()
    local net = cmd and nw:get_network(interface) or nil
    local dnsaddrs = net and net:dnsaddrs() or {}
    local dyninfo = {
        ipaddr  = net and net:ipaddr() or "0.0.0.0",
        netmask = net and net:netmask() or "0.0.0.0",
        gateway = net and net:gwaddr() or "0.0.0.0",
        pri_dns = dnsaddrs[1] or "0.0.0.0",
        snd_dns = dnsaddrs[2] or "0.0.0.0"
    }

    return dyninfo
end

local function get_wan6_dhcp6c_prefix()
    local prefix
    if not nixio.fs.access(PREFIX_PATH) then
        return ""
    end
    for line in io.lines(PREFIX_PATH) do
        prefix = string.match(line, "([%x:]+)")
        -- dbg.printf("get_wan6_dhcp6c_prefix(): " .. prefix)
    end
    return prefix or ""
end

--- Get ipv4 or ipv6 wan protocol configuration settings
local function get_inet_proto(form, proto_name)
    local dyninfo, inet_dyninfo, dyn6info, dyn6to4info
    local vldproto = get_valid_proto(proto_name)
    local v6 = vldproto.ipv6

    -- Get protocol static information from config file.
    local proto = proto_name and load_proto(proto_name)
    if not proto then
        return false, "invalid proto_name"
    end

    -- Get current protocol.
    local protov4 = uci_r:get("network", "wan", "wan_type")

    -- Get protocol dynamic information.
    if v6 then
        local protov6 = uci_r:get("protocol", "wanv6", "proto_name")
        local cmd = protov6 == proto_name
        dyn6info = get_wanv6_dyninfo(cmd)

        if proto_name == "6to4" then
            if ctypes.check_3p(protov4) then
                dyn6to4info = get_wan_dyninfo("internet", cmd)
            else
                dyn6to4info = get_wan_dyninfo("wan", cmd)
            end
        end
    else
        local cmd = protov4 == proto_name
        if ctypes.check_3p(proto_name) then
            inet_dyninfo = get_wan_dyninfo("internet", cmd)
            if proto.snd_conn ~= "dynamic" then
                -- cmd here should not affect @inet_dyninfo
                cmd = false
            end
        end
        if proto_name ~= "static" then
			if cmd == true then 
				local if_proto
				if ctypes.check_3p(proto_name) then
					if_proto = internet:proto("internet")
				else
					if internet:proto("internet") == nil then
						if_proto = internet:proto("wan")
					else
						if_proto = nil
					end
				end
				cmd = proto_name == if_proto
			end
            dyninfo = get_wan_dyninfo("wan", cmd)
        end
    end

    if proto_name == "dhcp" then
        util.update(proto, dyninfo)
        proto.dns_mode = proto.dns_mode or "dynamic"
        proto.dyn_pridns = proto.dns_mode == "dynamic" and dyninfo.pri_dns or "0.0.0.0"
        proto.dyn_snddns = proto.dns_mode == "dynamic" and dyninfo.snd_dns or "0.0.0.0"
        proto.mac_clone_type = get_mac(form).mac_clone_type
    elseif proto_name == "l2tp" or proto_name == "pptp" then
        proto.inet_ip     = inet_dyninfo.ipaddr
        proto.inet_pridns = inet_dyninfo.pri_dns
        proto.inet_snddns = inet_dyninfo.snd_dns
        proto.snd_conn    = proto.snd_conn or "dynamic"
        proto.conn_mode   = proto.conn_mode or "auto"
        proto.dyn_ip      = dyninfo.ipaddr
        proto.dyn_gateway = dyninfo.gateway
        proto.dyn_netmask = dyninfo.netmask
        proto.dyn_pridns  = dyninfo.pri_dns
        proto.dyn_snddns  = dyninfo.snd_dns
    elseif proto_name == "pppoe" then
        proto.inet_ip     = inet_dyninfo.ipaddr
        proto.inet_pridns = inet_dyninfo.pri_dns
        proto.inet_snddns = inet_dyninfo.snd_dns
        proto.snd_conn    = proto.snd_conn or "none"
        proto.conn_mode   = proto.conn_mode or "auto"
        proto.dyn_ip      = dyninfo.ipaddr
        proto.dyn_netmask = dyninfo.netmask
        proto.dns_mode    = proto.dns_mode or "dynamic"
        proto.ip_mode     = proto.ip_mode or "dynamic"
        proto.dyn_pridns  = proto.dns_mode == "dynamic" and proto.inet_pridns or "0.0.0.0"
        proto.dyn_snddns  = proto.dns_mode == "dynamic" and proto.inet_snddns or "0.0.0.0"
    elseif proto_name == "dhcpv6" then
        util.update(proto, dyn6info)
        proto.prefix = get_wan6_dhcp6c_prefix()
        proto.dyn_pridns = proto.dns_mode == "dynamic" and proto.pri_dns or "::"
        proto.dyn_snddns = proto.dns_mode == "dynamic" and proto.snd_dns or "::"
    elseif proto_name == "pppoev6" then
        proto.ip6addr = dyn6info.ip6addr
    elseif proto_name == "6to4" then
        proto.ipaddr = dyn6to4info.ipaddr
        proto.netmask = dyn6to4info.netmask
        proto.gateway = dyn6to4info.gateway
        proto.tunnel_addr = dyn6info.ip6addr
    elseif proto_name == "6rd" then
        proto.ip_mode = proto.ip_mode
        if proto.ip_mode == "auto" then
            proto.ipv4_mask_len = 0
            proto["6rd_prefix"] = "2001:55c::"
            proto["6rd_prefix_len"] = 32
            proto.relay_ipv4_addr = "69.252.80.66"
        end
    elseif proto_name == "dslite" then
        util.update(proto, dyn6info)
        if proto.snd_conn == "dynamic" then
            proto.dynamic_ip6addr = dyn6info.ip6addr
            proto.dynamic_pri_dns = dyn6info.pri_dns
            proto.dynamic_snd_dns = dyn6info.snd_dns
            proto.dynamic_dyn_pridns = dyn6info.pri_dns
            proto.dynamic_dyn_snddns = dyn6info.snd_dns
            proto.dynamic_prefix = get_wan6_dhcp6c_prefix()
        elseif proto.snd_conn == "static" then
            -- nothing
        else 
            proto.pppoe_ip6addr = dyn6info.ip6addr
            --proto.pppoe_specific_ip = dyn6info.ip6addr 
        end
        proto.pppoe_ip6addr = dyn6info.ip6addr
        --proto.pppoe_specific_ip = dyn6info.ip6addr 
        --proto.dynamic_pri_dns = proto.dynamic_pri_dns == "dynamic" and proto.dynamic_pri_dns or "::"
        --proto.dynamic_snd_dns = proto.dynamic_snd_dns == "dynamic" and proto.dynamic_snd_dns or "::"
    end

    -- Set protocol settings to json data for webpage
    local data = {}
    local proto_tbl = PROTO_TBL[proto_name]
    for _, v in ipairs(proto_tbl) do
        data[v.key] = proto[v.key] or ""
    end

    if v6 then
        local st = get_ipv6_status()
        if st.enable == "on" then
            data.conntype = st.conntype
        else
            data.conntype = "none"
        end
    else
        data.conntype = get_ipv4_conntype()
    end

    if data.conntype == proto_tbl.wan_type then
        data.conn_status = internet:status(v6)
    else
        data.conn_status = "disconnected"
    end
    data.link_status = internet:link_state("wan")
    return data
end

local function field_canbe_empty(proto, field, badvalue)
    return proto[field] ~= badvalue
end

local function check_network_extra(proto, ip_field, mask_field, lan)
    local ipaddr = type(ip_field) == "function" and ip_field(proto)
        or proto[ip_field]
    local netmask = type(mask_field) == "function" and mask_field(proto)
        or proto[mask_field]

    if not ipaddr and not netmask then
        return true
    elseif not (ipaddr and netmask) then
        return false
    end

    if not ctypes.check_unicast_ipv4_extra(ipaddr, netmask) then
        return false
    end

    local ano
    local info
    if lan then
        info = get_status_wan_ipv4()
        ano = {
            ipaddr = info.wan_ipv4_ipaddr,
            netmask = info.wan_ipv4_netmask,
            snd_ipaddr = info.wan_ipv4_snd_ipaddr,
            snd_netmask = info.wan_ipv4_snd_netmask,
        }
    else
        info = get_status_lan_ipv4()
        ano = {
            ipaddr = info.lan_ipv4_ipaddr,
            netmask = info.lan_ipv4_netmask
        }
    end

    if ano.netmask and ctypes.check_ipv4_equal(ano.netmask, "0.0.0.0")
    then
        ano.ipaddr = nil
        ano.netmask = nil
    end

    if ano.snd_netmask and ctypes.check_ipv4_equal(ano.snd_netmask, "0.0.0.0")
    then
        ano.snd_ipaddr = nil
        ano.snd_netmask = nil
    end

    if ipaddr and netmask
        and not ctypes.check_network_no_collision(ipaddr, netmask,
                                                  ano.ipaddr, ano.netmask)
    then
        return false
    end

    if ano.snd_ipaddr and ano.snd_netmask
        and not ctypes.check_network_no_collision(ipaddr, netmask,
                                                  ano.snd_ipaddr, ano.snd_netmask)
    then
        return false
    end

    return true
end

local CHECK_DHCP_TBL = {
    {
        field = "dns_mode",
        check = {
            {ctypes.check_in, {"dynamic", "static"}}
        }
    },
    {
        field = "unicast",
        check = {
            ctypes.check_onoff
        }
    },
    {
        field = "manual_pridns",
        canbe_absent = {field_canbe_empty, "dns_mode", "static"},
        check = {
            ctypes.check_unicast_ipv4
        }
    },
    {
        field = "manual_snddns", canbe_empty = true,
        check = {
            ctypes.check_unicast_ipv4
        }
    },
    {
        field = "mtu",
        check = {
            {ctypes.check_range, 576, 1500}
        }
    },
    {
        field = "hostname", canbe_empty = true,
        check = {
            ctypes.check_visible,
            {ctypes.check_rangelen, 0, 63 * 3}
        }
    },
    {
        field = "mac_clone_type",
        check = {
            -- We don't have "custom" here.
            {ctypes.check_in, {"default", "computer", "custom"}}
        }
    }
}

local CHECK_STATIC_TBL = {
    {
        field = {"ipaddr", "gateway", "pri_dns"}, canbe_absent = false,
        check = {
            ctypes.check_unicast_ipv4
        }
    },
    {
        field = "snd_dns", canbe_empty = true,
        check = {
            ctypes.check_unicast_ipv4
        }
    },
    {
        field = "netmask", canbe_absent = false,
        check = {
            ctypes.check_netmask
        }
    },
    {
        field = "mtu",
        check = {
            {ctypes.check_range, 576, 1500}
        }
    },
    {
        check = {
            {check_network_extra, "ipaddr", "netmask"},
        }
    }
}

local CHECK_PPPOE_TBL = {
    {
        field = {"username", "password"}, canbe_empty = true,
        check = {
            ctypes.check_visible,
            {ctypes.check_rangelen, 0, 118}
        }
    },
    {
        field = "snd_conn",
        check = {
            {ctypes.check_in, {"none", "dynamic", "static"}}
        }
    },
    {
        field = "static_ip",
        canbe_absent = {field_canbe_empty, "snd_conn", "static"},
        check = {
            ctypes.check_unicast_ipv4
        }
    },
    {
        field = "static_netmask",
        canbe_absent = {field_canbe_empty, "snd_conn", "static"},
        check = {
            ctypes.check_netmask
        }
    },
    {
        field = "mtu",
        check = {
            {ctypes.check_range, 576, 1492}
        }
    },
    {
        field = {"server", "access"}, canbe_empty = true,
        check = {
            ctypes.check_visible,
            {ctypes.check_rangelen, 0, 31}
        }
    },
    {
        field = "interval",
        check = {
            {ctypes.check_range, 0, 120}
        }
    },
    {
        field = "ip_mode",
        check = {
            {ctypes.check_in, {"dynamic", "static"}}
        }
    },
    {
        field = "specific_ip",
        canbe_absent = {field_canbe_empty, "ip_mode", "static"},
        check = {
            ctypes.check_unicast_ipv4
        }
    },
    {
        field = "dns_mode",
        check = {
            {ctypes.check_in, {"dynamic", "static"}}
        }
    },
    {
        field = "static_pridns",
        canbe_absent = {field_canbe_empty, "dns_mode", "static"},
        check = {
            ctypes.check_unicast_ipv4
        }
    },
    {
        field = "static_snddns", canbe_empty = true,
        check = {
            ctypes.check_unicast_ipv4
        }
    },
    {
        field = "conn_mode",
        check = {
            {ctypes.check_in, {"auto", "demand", "time_based", "manually"}}
        }
    },
    {
        field = {"manual_idle", "demand_idle"},
        check = {
            {ctypes.check_range, 0, 99}
        }
    },
    {
        field = {"time_start", "time_end"},
        canbe_absent = {field_canbe_empty, "conn_mode", "time_based"},
        check = {
            ctypes.check_day_time
        }
    },
    {
        check = {
            {check_network_extra, "static_ip", "static_netmask"},
            function(proto)
                if proto.time_start and proto.time_end then
                    if ctypes.check_day_time_lower(proto.time_start,
                                                   proto.time_end)
                    then
                        return true
                    else
                        return false
                    end
                elseif not proto.time_start and not proto.time_end then
                    -- It's fine.
                    return true
                else
                    return false
                end
            end
        }
    }
}

local CHECK_BIGPOND_TBL = {
    {
        field = {"username", "password"}, canbe_empty = true,
        check = {
            ctypes.check_visible,
            {ctypes.check_rangelen, 0, 118}
        }
    },
    {
        field = {"server", "domain"}, canbe_empty = true,
        check = {
            ctypes.check_visible,
            {ctypes.check_rangelen, 0, 31}
        }
    },
    {
        field = "mtu",
        check = {
            {ctypes.check_range, 576, 1500}
        }
    },
    {
        field = "conn_mode",
        check = {
            {ctypes.check_in, {"auto", "demand", "manually"}}
        }
    },
    {
        field = {"demand_idle", "manual_idle"},
        check = {
            {ctypes.check_range, 0, 99}
        }
    }
}

local function xxtp_gateway_canbe_empty(proto)
    if not proto.static_server then
        return true
    end

    if not proto.static_ip or not proto.static_netmask then
        return false
    end

    if ctypes.check_ipv4(proto.static_server) then
        if not ctypes.check_same_network(proto.static_server,
                                         proto.static_ip,
                                         proto.static_netmask)
        then
            return false
        end
    else
        if not proto.static_pridns
            or not ctypes.check_same_network(proto.static_pridns,
                                             proto.static_ip,
                                             proto.static_netmask)
        then
            return false
        end
    end

    return true
end

local function xxtp_pridns_canbe_empty(proto)
    return not proto.static_server
        or ctypes.check_ipv4(proto.static_server)
end

local CHECK_L2TP_TBL = {
    {
        field = {"username", "password"}, canbe_empty = true,
        check = {
            ctypes.check_visible,
            {ctypes.check_rangelen, 0, 118}
        }
    },
    {
        field = "snd_conn",
        check = {
            {ctypes.check_in, {"dynamic", "static"}}
        }
    },
    {
        field = "dyn_server",
        canbe_absent = {field_canbe_empty, "snd_conn", "dynamic"},
        check = {
            {ctypes.check_rangelen, 1, 63},
            ctypes.check_domain,
        }
    },
    {
        field = "static_server",
        canbe_absent = {field_canbe_empty, "snd_conn", "static"},
        check = {
            {ctypes.check_rangelen, 1, 63},
            ctypes.check_domain,
        }
    },
    {
        field = "static_ip",
        canbe_absent = {field_canbe_empty, "snd_conn", "static"},
        check = {
            ctypes.check_unicast_ipv4
        }
    },
    {
        field = "static_netmask",
        canbe_absent = {field_canbe_empty, "snd_conn", "static"},
        check = {
            ctypes.check_netmask
        }
    },
    {
        field = "static_gateway",
        canbe_absent = xxtp_gateway_canbe_empty,
        canbe_empty = xxtp_gateway_canbe_empty,
        check = {
            ctypes.check_unicast_ipv4
        }
    },
    {
        field = "static_pridns",
        canbe_absent = xxtp_pridns_canbe_empty,
        canbe_empty = xxtp_pridns_canbe_empty,
        check = {
            ctypes.check_unicast_ipv4
        }
    },
    {
        field = "static_snddns", canbe_empty = true,
        check = {
            ctypes.check_unicast_ipv4
        }
    },
    {
        field = "mtu",
        check = {
            {ctypes.check_range, 576, 1460}
        }
    },
    {
        field = "conn_mode",
        check = {
            {ctypes.check_in, {"auto", "demand", "manually"}}
        }
    },
    {
        field = {"demand_idle", "manual_idle"},
        check = {
            {ctypes.check_range, 0, 99}
        }
    }
}

local CHECK_PPTP_TBL = {
    {
        field = {"username", "password"}, canbe_empty = true,
        check = {
            ctypes.check_visible,
            {ctypes.check_rangelen, 0, 118}
        }
    },
    {
        field = "snd_conn",
        check = {
            {ctypes.check_in, {"dynamic", "static"}}
        }
    },
    {
        field = "dyn_server",
        canbe_absent = {field_canbe_empty, "snd_conn", "dynamic"},
        check = {
            {ctypes.check_rangelen, 1, 63},
            ctypes.check_domain,
        }
    },
    {
        field = "static_server",
        canbe_absent = {field_canbe_empty, "snd_conn", "static"},
        check = {
            {ctypes.check_rangelen, 1, 63},
            ctypes.check_domain,
        }
    },
    {
        field = "static_ip",
        canbe_absent = {field_canbe_empty, "snd_conn", "static"},
        check = {
            ctypes.check_unicast_ipv4
        }
    },
    {
        field = "static_netmask",
        canbe_absent = {field_canbe_empty, "snd_conn", "static"},
        check = {
            ctypes.check_netmask
        }
    },
    {
        field = "static_gateway",
        canbe_absent = xxtp_gateway_canbe_empty,
        canbe_empty = xxtp_gateway_canbe_empty,
        check = {
            ctypes.check_unicast_ipv4
        }
    },
    {
        field = "static_pridns",
        canbe_absent = xxtp_pridns_canbe_empty,
        canbe_empty = xxtp_pridns_canbe_empty,
        check = {
            ctypes.check_unicast_ipv4
        }
    },
    {
        field = "static_snddns", canbe_empty = true,
        check = {
            ctypes.check_unicast_ipv4
        }
    },
    {
        field = "mtu",
        check = {
            {ctypes.check_range, 576, 1420}
        }
    },
    {
        field = "conn_mode",
        check = {
            {ctypes.check_in, {"auto", "demand", "manually"}}
        }
    },
    {
        field = {"demand_idle", "manual_idle"},
        check = {
            {ctypes.check_range, 0, 99}
        }
    }
}

local CHECK_DHCPV6_TBL = {
    {
        field = "ip_mode",
        check = {
            {ctypes.check_in, {"non_temp", "prefix"}}
        }
    },
    {
        field = "dns_mode",
        check = {
            {ctypes.check_in, {"dynamic", "static"}}
        }
    },
	{
		field = "ip_config",
        check = {
            {ctypes.check_in, {"dhcpv6", "slaac"}}
        }
    },
    {
        field = "static_pridns",
        canbe_absent = {field_canbe_empty, "dns_mode", "static"},
        check = {
            ctypes.check_unicast_ipv6
        }
    },
    {
        field = "static_snddns", canbe_empty = true,
        check = {
            ctypes.check_unicast_ipv6
        }
    }
}

local CHECK_STATICV6_TBL = {
    {
        field = {"ip6addr", "ip6gw", "pri_dns"}, canbe_absent = false,
        check = {
            ctypes.check_unicast_ipv6
        }
    },
    {
        field = "snd_dns", canbe_empty = true,
        check = {
            ctypes.check_unicast_ipv6
        }
    },
    {
        field = "mtu",
        check = {
            {ctypes.check_range, 1280, 1500},
        }
    },
}

local CHECK_PPPOEV6_TBL = {
    {
        field = {"username", "password"}, canbe_empty = true,
        check = {
            ctypes.check_visible,
            {ctypes.check_rangelen, 0, 118}
        }
    },
    {
        field = "ip_mode",
        check = {
            {ctypes.check_in, {"non_temp", "prefix"}}
        }
    },
    {
        field = "specific_ip",
	canbe_absent = {field_canbe_empty, "ip_config", "specified"},
        check = {
            ctypes.check_unicast_ipv6
        }
    },
	{
         field = "ip_config",
         check = {
            {ctypes.check_in, {"dhcpv6", "slaac", "specified"}}
         }
     },
    {
        field = "dns_mode",
        check = {
            {ctypes.check_in, {"dynamic", "static"}}
        }
    },
    {
        field = "static_pridns",
        canbe_absent = {field_canbe_empty, "dns_mode", "static"},
        check = {
            ctypes.check_unicast_ipv6
        }
    },
    {
        field = "static_snddns", canbe_empty = true,
        check = {
            ctypes.check_unicast_ipv6
        }
    }
}

local CHECK_6TO4_TBL = {
    {
        field = "dns_mode",
        check = {
            {ctypes.check_in, {"dynamic", "static"}}
        }
    },
    {
        field = "pri_dns",
        canbe_absent = {field_canbe_empty, "dns_mode", "static"},
        check = {
            ctypes.check_unicast_ipv6
        }
    },
    {
        field = "snd_dns", canbe_empty = true,
        check = {
            ctypes.check_unicast_ipv6
        }
    }
}

local CHECK_6RD_TBL = {
    {
        field = "ipv4_mask_len",
        check = {
            {ctypes.check_range, 0, 32}
        }
    },
    {
        field = "ip_mode",
        check = {
            {ctypes.check_in, {"auto", "manual"}}
        }
    },
    {
        field = "6rd_prefix", canbe_empty = false,
        check = {
            ctypes.check_unicast_ipv6_prefix
        }
    },
    {
        field = "relay_ipv4_addr", canbe_empty = false,
        check = {
            ctypes.check_unicast_ipv4
        }
    }
}

local CHECK_DSLITE_TBL = {
    {
        field = "ip_mode",
        check = {
            {ctypes.check_in, {"auto", "manually"}}
        }
    },
    {
        field = "AFTR_name", canbe_empty = true,
        check = {
            ctypes.check_visible,
            {ctypes.check_rangelen, 0, 118}
        }
    }
}

local CHECK_PASSTHROUGH_TBL = {
    
}

local CHECK_PROTO_TBL = {
    ["pppoe"]       = CHECK_PPPOE_TBL,
    ["pptp"]        = CHECK_PPTP_TBL,
    ["l2tp"]        = CHECK_L2TP_TBL,
    ["dhcp"]        = CHECK_DHCP_TBL,
    ["static"]      = CHECK_STATIC_TBL,
    ["bigpond"]     = CHECK_BIGPOND_TBL,
    ["staticv6"]    = CHECK_STATICV6_TBL,
    ["dhcpv6"]      = CHECK_DHCPV6_TBL,
    ["pppoev6"]     = CHECK_PPPOEV6_TBL,
    ["6to4"]        = CHECK_6TO4_TBL,
    ["6rd"]         = CHECK_6RD_TBL,
    ["dslite"]      = CHECK_DSLITE_TBL,
    ["passthrough"] = CHECK_PASSTHROUGH_TBL,
}

--- Check if wan protocol config changed
local function check_proto_changes(proto_name, proto)
    local change = {
        proto  = false,
        config = false,
    }
    local proto_old = load_proto(proto_name)
    local vldproto = get_valid_proto(proto_name)
    local old_wan_type

    if vldproto.ipv6 then
        old_wan_type = uci_r:get("protocol", "wanv6", "proto_name") or ""
    else
        old_wan_type = uci_r:get("network", "wan", "wan_type") or ""
    end

    if old_wan_type ~= proto_name then
        change.proto = true
    else
        for k, v in pairs(proto) do
            local oldv = proto_old[k] == nil and "" or proto_old[k]
            if oldv ~= v then
                change.config = true
                break
            end
        end
    end
    return change
end

local function get_cfg_devsec(ifname)
    local retsec = nil

    uci_r:foreach("network", "device", 
    function(section)
        local name    = section["name"]
        local secname = section[".name"]

        if not retsec and name == ifname then
            retsec = secname
        end
    end)

    return retsec
end

local function re_enable_ipv6()
    local ifname = uci_r:get("network", "wanv6", "ifname")
    local ipv6_enable_cfg = "/proc/sys/net/ipv6/conf/" .. ifname .. "/disable_ipv6"
    sys.fork_call("echo '1' > " .. ipv6_enable_cfg)
    sys.fork_call("sleep 1")
    sys.fork_call("echo '0' > " .. ipv6_enable_cfg)
end

local function set_dhcp_connect(form, proto_name)
    local proto = {}
    local vldproto = get_valid_proto(proto_name)
    local iface = vldproto.ipv6 and "wanv6" or "wan"
    local proto = uci_r:get("network", "wanv6", "ifname")
    local ipv6_enable_cfg =  proto and "/proc/sys/net/ipv6/conf/" .. proto .. "/disable_ipv6"

    if form.operation == "renew" then
        if ipv6_enable_cfg and (iface == "wanv6" or  iface == "internetv6") then
            re_enable_ipv6()
        end
        internet:connect(iface)
    elseif form.operation == "release" then
        internet:disconnect(iface)
    else
        -- Invalid operations
        dbg("Invalid operation " .. tostring(form.operation))
        return invalid_args()
    end

    -- Check data validation
    if not ctl.check(form, CHECK_PROTO_TBL[proto_name]) then
        return invalid_args()
    end

    return get_inet_proto({}, proto_name)
end

local function set_inet_proto(form, proto_name)
    local proto = {connectable = "1"}
    local mac_clone_flag = false
    local vldproto = get_valid_proto(proto_name)

    log(nlog.SET_WAN_PROTO, proto_name, form.operation)

    if not vldproto then
        dbg("invalid proto " .. proto_name)
        return invalid_args()
    end

    -- Check data validation
    if not ctl.check(form, CHECK_PROTO_TBL[proto_name]) then
        return invalid_args()
    end

    local v6 = vldproto.ipv6

    -- Get protocol config
    local proto_tbl = PROTO_TBL[proto_name]
    for _, v in ipairs(proto_tbl) do
        if not v.readonly then
            proto[v.key] = form[v.key]
        end
    end

    if form.operation == "disconnect" or form.operation == "release" then
        proto.connectable = "0"
    end

    -- Check protocol config to see if there are changes or not
    local changes = check_proto_changes(proto_name, proto)
    if form.operation == "write" 
        and not changes.proto and not changes.config
    then
        -- nothing changed
        log(nlog.NO_CHANGED)
        return get_inet_proto({}, proto_name)
    end

    -- Save protocol config
    uci_r:section("protocol", "proto", proto_name, proto)

    -- Update protocol and network config
    set_proto_cfg(proto_name, form.operation)

    -- Mac clone config from basic page
    mac_clone_flag = set_mac_flag(form, proto_name)

    -- MTU handler
    local ifname = uci_r:get("network", "wan", "ifname")
    local mtu    = uci_r:get("network", "wan", "mtu")

    if mtu then
        local secname = get_cfg_devsec(ifname)
        if secname then
            uci_r:set("network", secname, "mtu", mtu)
        end
    end

    -- make sure wan's ifname keepup always
    uci_r:set("network", "wan", "keepup", "1")

    -- if proto_name:find("passthrough") then
        -- uci_r:delete("network", "lanv6")
    -- end

    if not uci_commit() then
        return false, "set_inet commit config failed"
    end

    -- When wan_type changed, call ddns_iface_change() before network reload
    if changes.proto then
        log(nlog.DDNS_IFACE_CHANGE)
        local ddns = require "luci.model.ddns"
        ddns.change_iface()
    end

    -- Check IPTV, in order to set some config to network, Add Jason Guo
    local iptv = require "luci.controller.admin.iptv"
    iptv.set_network_attr()

    -- operation(write, connect, disconnect) handler for:
    -- ipv4: pppoe, pptp, l2tp
    -- ipv6: pppoe, 6to4

    -- 1. disconnect
    if mac_clone_flag == true then
        --[[
        internet:disconnect("wanv6")
        internet:disconnect("internet")
        internet:disconnect("wan")
        nixio.nanosleep(1, 0)
        ]]--
        internet:restart()
        return get_inet_proto({}, proto_name)

    elseif v6 then
        internet:disconnect("wanv6")

    else
        internet:disconnect("internet")
        nixio.nanosleep(1, 0)
        internet:disconnect("wan")
    end

    -- 2. reload: ubus call network reload
    if changes.proto or changes.config then
        dbg("set_inet_proto reload")
        internet:reload()
    end
    
    if proto_name:find("passthrough") then
        internet:down("lanv6")
    end
    
    -- 3. connect; or confirm disconnect
    if form.operation == "write" then
        if v6 then
            local wanv6_type = uci_r:get("network", "wanv6", "wan_type")
            if wanv6_type == "dhcp6c" then
                re_enable_ipv6()
            end
        end
    elseif form.operation == "connect" then
	if changes.proto or changes.config then
	    sys.fork_call("sleep 5")
	end
	dbg("set_inet_proto: write or connect operation")
        if v6 then
	    internet:connect("wanv6")
            internet:connect("internetv6")
        else 
            internet:connect("wan")
            internet:connect("internet")
        end

    elseif form.operation == "disconnect" then
        -- ignore
        dbg("set_inet_proto: disconnect operation")
        if v6 then
            internet:disconnect("wanv6")
        else
            internet:disconnect("internet")
            -- ignore disconnect wan because netifd will down it
            -- internet:disconnect("wan")
        end

    else
        return invalid_args()
    end

    dbg("set_inet_proto return")
    return get_inet_proto({}, proto_name)
end

local function get_autodetect_enable()
    local result = "on"

    -- FIXME: dirty.
    local wlan = require "luci.model.wireless"
    local cur_country = wlan.APCFG("region"):read_data().country

    local region = require "luci.controller.admin.region"
    for _, country, _, no_autodetect in region.regions() do
        if cur_country == country then
            if no_autodetect then
                result = "off"
            end
            break
        end
    end

    return result
end

local function get_ipv4_protos(form)
    return get_ipvx_protos(form, false)
end

function get_ipv4_conntype()
    local proto = uci_r:get("network", "wan", "proto")
    local wan_type = uci_r:get("network", "wan", "wan_type")

    if wan_type then
        return wan_type or ""
    else
        return proto or ""
    end
end

--- Operation of internet ipv4 conntype request
local function get_ipv4_status()
    return {
        conntype = get_ipv4_conntype(),
        autodetect = get_autodetect_enable()
    }
end

local function get_ipv6_protos(form)
    return get_ipvx_protos(form, true)
end

--- Operation of internet ipv6 request
function get_ipv6_status()
    local proto = uci_r:get("protocol", "wanv6", "proto") or ""
    local enable = uci_r:get("protocol", "wanv6", "enable") or "off"
    local data  = {}

    data.enable = enable
    data.conntype = proto --== "dhcp6c" and "dhcpv6" or proto

    return data
end

local function set_ipv6_enable(on)
 --   local iface    = uci_r:get("network", "wan", "ifname")
    local ipv6_cfg = "/proc/sys/net/ipv6/conf/all/disable_ipv6"

    if on == "on" then
        sys.fork_call("echo '0' > " .. ipv6_cfg)
    elseif on == "off" then
        sys.fork_call("echo '1' > " .. ipv6_cfg)
    end
end

local function set_ipv6_status(form)
    local enabled = form.enable
    local cur_enable = uci_r:get("protocol", "wanv6", "enable")
    local proto_name = uci_r:get("protocol", "wanv6", "proto_name")

    log(nlog.SET_IPV6, enabled, proto_name)

    if not ctypes.check_onoff(enabled) then
        return invalid_args("set ipv6 status arguments invalid")
    end

    if cur_enable ~= enabled then
        uci_r:set("protocol", "wanv6", "enable", enabled)
        if enabled == "off" then
            --disconnect wanv6 to let dhcp send release,or it will occupy an address or pd;
            sys.fork_call("ubus call network.interface.wanv6 disconnect")
            uci_r:delete("network", "wanv6")
            --uci_r:delete("network", "lanv6")
        else
            set_proto_cfg(proto_name, nil)
        end
        uci_commit()
        set_ipv6_enable(enabled)
        internet:restart()
    else
        log(nlog.NO_CHANGED)
    end

    return get_ipv6_status()
end

--[[
    LAN ipv4 and ipv6 block
]]--

local function get_wan_ipaddrs()
    local nw      = nw.init()
    local ifaces  = {"wan", "internet"}
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

--- Get and send lan ipv4 info to ui
local function get_lan_ipv4()
    local nw = nw.init()
    local net = nw:get_network("lan")
    local ifc = net and net:get_interface()

    local ipaddr = net and net:ipaddr()
    local macaddr = ifc:mac()
    local maskaddr = net:netmask() or uci_r:get("network", "lan", "netmask")
    --local igmp = require("luci.model.igmp_proxy").get_igmp_proxy() or "off"
    local mask_type = uci_r:get("protocol", "lan", "mask_type") or maskaddr

    --load ipv4 setting webpage
    local data = {
        macaddr      = macaddr:gsub(":", "-"):upper(),
        ipaddr       = ipaddr,
        mask_type    = mask_type,
        custom_value = uci_r:get("protocol", "lan", "custom_value") or "",
        --igmp         = igmp, 
    }

    return data
end

local CHECK_LAN_IPV4_TBL = {
    {
        field = "ipaddr",
        check = {
            ctypes.check_unicast_ipv4
        }
    },
    {
        field = "mask_type",
        check = {
            {ctypes.check_in, {"255.255.255.0", "255.255.0.0", "255.0.0.0", "custom"}}
        }
    },
    {
        field = "custom_value",
        canbe_absent = {field_canbe_empty, "mask_type", "custom"},
        check = {
            ctypes.check_netmask
        }
    },
    {
        field = "igmp",
        check = {
            ctypes.checkonoff
        }
    },
    {
        check = {
            {check_network_extra, "ipaddr", function(form)
                 return form.mask_type == "custom" and form.custom_value
                     or form.mask_type
            end, true}
        }
    }
}

--- Set ipv4 config when ui operation is write
local function set_lan_ipv4(form)
    local ipaddr    = form.ipaddr
    local mask_type = form.mask_type
    local igmp      = form.igmp
    local custom_value = form.custom_value
    local maskaddr  = (mask_type == "custom") and custom_value or mask_type

    log(nlog.SET_LAN, ipaddr, maskaddr)
    if not ctl.check(form, CHECK_LAN_IPV4_TBL) then
        return invalid_args()
    end

    local igmp_m = require "luci.model.igmp_proxy"

    local nw = nw.init()
    local net = nw:get_network("lan")

    local cur_ipaddr       = net and net:ipaddr()
    local cur_maskaddr     = net and net:netmask()
    local cur_custom_value = uci_r:get("protocol", "lan", "custom_value")
    local cur_igmp         = igmp_m.get_igmp_proxy()
    igmp = igmp or cur_igmp

    if cur_ipaddr == ipaddr
        and cur_maskaddr == maskaddr
        and mask_type == uci_r:get("protocol", "lan", "mask_type")
        and cur_igmp == igmp
    then
        log(nlog.NO_CHANGED)
        return get_lan_ipv4()
    end

    local ipaddrs = get_wan_ipaddrs()
    local lanip   = ip.IPv4(ipaddr, maskaddr)

    for _, v in ipairs(ipaddrs) do
        local wanip = ip.IPv4(v.ipaddr, v.netmask)
        if lanip and wanip then
            if wanip:contains(lanip) or lanip:contains(wanip) then
                return false, "Lan IP address is conflicting with Wan IP address."
            end
        end
    end

    uci_r:section("protocol", "interface", "lan", {
                      ['mask_type'] = mask_type,
                      ['custom_value'] = custom_value or cur_custom_value
    })
    uci_r:set("network", "lan", "ipaddr", ipaddr)
    uci_r:set("network", "lan", "netmask", maskaddr)

    if cur_ipaddr ~= ipaddr or cur_maskaddr ~= maskaddr then
        -- Remove session
        log(nlog.RM_SESSION)
        local sess = require "luci.dispatcher".context.authsession
        if sess then
            local sauth = require "luci.sauth"
            sauth.kill(sess)
        else
            -- kill all sessions.
            local nixio     = require "nixio"
            local lconfig   = require "luci.config"
            local sess_auth = require "luci.sauth"
            local sess_path = lconfig.sauth.sessionpath
            local session_dir = nixio.fs.dir(sess_path)
            local session

            if session_dir ~= nil then
                for session in nixio.fs.dir(sess_path) do
                    sess_auth.kill(session)
                end
            end
        end
    end

    if cur_igmp ~= igmp then
        log(nlog.SET_IGMP, igmp)
        if igmp == "on" then
            igmp_m.enable_igmp_proxy(igmp, ipaddr, maskaddr)
        else
            igmp_m.disable_igmp_proxy()
        end
    end

    nixio.nanosleep(3, 0)

    if not uci_commit() then
        return false, "set_lan_ipv4 commit config failed"
    end

    local dhcps = require "luci.controller.admin.dhcps"
    dhcps.dhcp_opt_update(ipaddr)
    -- ip or mask changes, we will reverify all the static lease entries. We will
    -- disable all the invalid entry instead of deleting all of them.
    if cur_ipaddr ~= ipaddr or cur_maskaddr ~= maskaddr then
        dbg.printf("Lan ip/mask changes, check all static lease entries "
            .. "with new ip/mask(" .. ipaddr .. "/" .. maskaddr .. ")")
        dhcps.disable_all_invalid_static_lease(ipaddr, maskaddr)
        sys.fork_exec("/etc/init.d/dnsmasq stop")
    end
    
    -- ip changes, call tfstats to change ip and netmask
    local cur_network = luci.ip.IPv4(cur_ipaddr, cur_maskaddr):network() -- get network address
    local new_network = luci.ip.IPv4(ipaddr, maskaddr):network()
    if cur_network:string() ~= new_network:string() then
        log(nlog.UPDATE_TFSTATS)
        local tfs = require "luci.model.tfstats"
        local tfs_t = tfs.TFS_INST()
        local ret = tfs_t:set_net(ipaddr, maskaddr)
        dbg.printf("Lan ip changes, ubus set tfstats net:" .. tostring(ret))
        --[[
        -- IP changed, we need to clear dhcp server reserved ip address host
        -- Add by guodongxian, June 2014
        dhcps.remove_all_static_lease()
        ]]--
    end


    local data = get_lan_ipv4()
    data.new_ipaddr = ipaddr

    -- 1. restart network in daemon
    internet:restart()

    -- 2. restart switch in daemon ugly
    sys.fork_exec("wifi reload")
    sys.fork_exec("sleep 1; /sbin/switch_link_down; sleep 5; /sbin/switch_link_up");

    -- 3. return the last config of lan immediately
    return data
end

--- Get ipv6 address infomation from system
local function get_ipv6addr()
    local nw = nw.init()
    local net = nw:get_network("lan")
    local ifc = net and net:get_interface()
    local ipv6_addr = {}

    for _, a in ipairs(ifc:ip6addrs()) do
        ipv6_addr[#ipv6_addr+1] = {
            addr    = a:host():zcstring(),
            netmask = a:mask():zcstring(),
            prefix  = a:prefix()
        }
    end

    return ipv6_addr
end

local function _get_prefix(path)
   --local f = io.open("/tmp/dhcp6c/prefix.info", 'r')
   local f = io.open(path, 'r')
   if f == nil then
       return ""
   end

   local prefix = f:read("*all")
   f:close()
   --dbg.printf(prefix)
   if prefix == "::\n" then
       return ""
   end

   return prefix
end


--- Get and send lan ipv6 info to ui
local function get_lan_ipv6()
    local ipv6_info = get_ipv6addr()

    if not ipv6_info or #ipv6_info == 0 then
        --return false, "get lan ipv6 info failed!"
        ipv6_info = ipv6_info or {}
        ipv6_info[#ipv6_info + 1] = {
            addr = "::",
            prefix = 0
        }
    end

    local lanv6_proto = uci_r.get("network", "lanv6", "proto")
	local wanv6_proto = uci_r.get("network", "wanv6", "proto")
    local ipv6_type   = uci_r.get("network", "lanv6", "proto") or "slaac"
    local addr_prefix = uci_r:get("protocol", ipv6_type, "prefix") or ""
    local ipv6_addr   = ipv6_info[1].addr .. "/" .. ipv6_info[1].prefix

    --load ipv6 setting webpage
    local data = {
        assign_type  = (ipv6_type == "dhcp6s") and "dhcpv6" 
                            or (ipv6_type == "slaac" and "slaac" or "rdnss"),
        dhcp_prefix  = uci_r:get("protocol", "dhcp6s", "prefix") or "",
        slaac_prefix = uci_r:get("protocol", "slaac", "prefix") or "",
        rdnss_prefix = uci_r:get("protocol", "rdnss", "prefix") or "",
        -- rdnss_prefix_type = uci_r:get("protocol", "rdnss", "rdnss_prefix_type") or "",
        release_time = uci_r:get("network", "lanv6", "leasetime") or "86400",
        address      = ipv6_addr
    }
    if lanv6_proto == nil then
		 local pd = _get_prefix("/tmp/dhcp6c/prefix.info")
		 data.dhcp_prefix = pd
		 data.slaac_prefix = pd
		 data.rdnss_prefix = pd
		 end

		 if wanv6_proto == "6to4" then
		local pd = _get_prefix("/tmp/6to4_prefix.info")
        data.dhcp_prefix = pd
        data.slaac_prefix = pd
        data.rdnss_prefix = pd
    end
    return data
end

local CHECK_LAN_IPV6_TBL = {
    --[[{
        field = "assign_type",
        check = {
            {ctypes.check_in, {"dhcpv6", "slaac", "rdnss"}}
        }
    },
    {
        field = "dhcp_prefix",
        canbe_absent = {field_canbe_empty, "assign_type", "dhcpv6"},
        check = {
            ctypes.check_unicast_ipv6_prefix
        }
    },
    {
        field = "slaac_prefix",
        canbe_absent = {field_canbe_empty, "assign_type", "slaac"},
        check = {
            ctypes.check_unicast_ipv6_prefix
        }
    },
    {
        field = "rdnss_prefix",
        canbe_absent = {field_canbe_empty, "assign_type", "rdnss"},
        check = {
            ctypes.check_unicast_ipv6_prefix
        }
    },--]]
    {
        field = "release_time",
        check = {
            {ctypes.check_range, 1, 691200}
        }
    }
}

--- Set lan ipv6 settings to configuration file
local function set_lan_ipv6(form)
    local ip_mode = uci_r:get("network", "wanv6", "ip_mode")
    local ip_config = uci_r:get("network", "wanv6", "ip_config")
    local assign_type = form.assign_type
    local leasetime = form.release_time
    local prefix = ""
    local wanproto = uci_r:get("network", "wanv6", "proto")

    if ip_config ~= "specified" and ip_mode == "prefix" or wanproto == "6to4" or wanproto == "6rd" then
        local tem_prefix = get_wan6_dhcp6c_prefix()

        if wanproto == "6to4" then
            tem_prefix = "2002::"
        elseif wanproto == "6rd" then
            tem_prefix = uci_r:get("protocol", "6rd", "6rd_prefix") or "2001:55c::"
        end

        if assign_type == "dhcpv6" then
            form.dhcp_prefix = tem_prefix
        elseif assign_type == "slaac" then
            form.slaac_prefix = tem_prefix
        elseif assign_type == "rdnss" then
            form.rdnss_prefix = tem_prefix
        end

        if assign_type == "dhcpv6" then
            if ctypes.check_range(leasetime, 1, 691200) == false then
                return invalid_args()
            end
        end

    elseif not ctl.check(form, CHECK_LAN_IPV6_TBL) then
        return invalid_args()
    end

    if assign_type == "dhcpv6" then
        prefix = form.dhcp_prefix
        assign_type = "dhcp6s"
    elseif assign_type == "slaac" then
        prefix = form.slaac_prefix
    elseif assign_type == "rdnss" then
        prefix = form.rdnss_prefix
    end

    local changed  = false
    local cur_type = uci_r:get("network", "lanv6", "proto")
    local lanv6_proto = uci_r:get_all("protocol", assign_type)

    -- Check whether lanv6 config changed.
    if cur_type == assign_type and lanv6_proto.leasetime == leasetime then
        if lanv6_proto.prefix == prefix then
            return form
        elseif ip.IPv6(lanv6_proto.prefix):equal(ip.IPv6(prefix)) then
            -- lanv6 config didn't changed actually.
            changed = false
        else 
            changed = true
        end
    else
        changed = true
    end

    uci_r:delete("network", "lanv6")
    lanv6_proto.prefix = prefix or ""
    lanv6_proto.leasetime = form.release_time or ""
    uci_r:section("protocol", "proto", assign_type, lanv6_proto)

    lanv6_proto.proto = assign_type
    uci_r:section("network", "interface", "lanv6", lanv6_proto)

    uci_r:set("protocol", "lanv6", "interface")
    uci_r:set("protocol", "lanv6", "proto", assign_type)

    if not uci_commit() then
        return false, "save lan ipv6 commit config failed!"
    end

    if not changed then
        return form
    end

    internet:reload()
    return get_lan_ipv6()
end

--[[
    MAC clone block
]]--

local function get_default_mac()
    -- FIXME: use a better way.
    local mac = sys.exec("network_get_firm wan")

    mac = util.trim(mac)
    -- mac = mac:upper()  
    return mac:gsub("-", ":")
end

local function get_default_lan_mac()
    local mac = sys.exec("network_get_firm lan")
    mac = util.trim(mac)
    mac = mac:upper()
    return mac:gsub(":", "-")
end

local function get_current_mac(interface)
    local nw = nw.init()
    local net = nw:get_network(interface)
    local ifc = net and net:get_interface()
    local cur_mac = ifc:mac()
    local dft_mac = "00:00:00:00:00:00"

    if cur_mac == dft_mac then
        -- wan was down
        cur_mac = uci_r:get("network", interface, "macaddr")
    end

    if not cur_mac then
        cur_mac = get_default_mac()
    end

    return cur_mac or dft_mac
end

local function get_user_mac()
    -- Get user ipaddr from uhttpd
    local user_ip = sys.getenv("REMOTE_ADDR")
    local cm = require "luci.model.client_mgmt"
    local user_mac = cm.get_mac_by_ip(user_ip) or ""

    return user_mac:gsub("-", ":")
end

--- Get mac informations and send to webpages
function get_mac(form)
    local custom_mac = uci_r:get("protocol", "wan", "custom_mac") or ""
    local clone_type = uci_r:get("protocol", "wan", "mactype") or "default"
    local data = {
        mac_clone_type = clone_type,
        mac_custom     = custom_mac:gsub(":", "-"),
        mac_computer   = get_user_mac():gsub(":", "-"),
        mac_default    = get_default_mac():gsub(":", "-")
    }

    return data
end

--- Set mac configurations from webpage to network config file
--- Call uci_commit after set_mac recommended
-- @return true if macaddr changed, else false
local function set_mac(form)
    local clone_type = form.mac_clone_type
    local custom_mac = form.mac_custom or ""
    local old_mactype = uci_r:get("protocol", "wan", "mactype")
    local old_macaddr = uci_r:get("network", "wan", "macaddr")
    local wan_ifname  = uci_r:get("network", "wan", "ifname")
    local new_macaddr = ""
    local mac_changed = false

    if clone_type == 'default' then
        new_macaddr = get_default_mac()
    elseif clone_type == 'computer' then
        new_macaddr = get_user_mac()
    else
        custom_mac = custom_mac:gsub("-", ":")
        new_macaddr = custom_mac
    end

    if #new_macaddr == 0 then
        -- get mac failed
        mac_changed = false
    elseif new_macaddr == old_macaddr then
        -- macaddr didn't changed
        mac_changed = false
    else 
        mac_changed = true
    end

    if old_mactype ~= clone_type then
        uci_r:set("protocol", "wan", "mactype", clone_type)
    end
    if #custom_mac > 0 then
        uci_r:set("protocol", "wan", "custom_mac", custom_mac)
    end

    if mac_changed == true then
        log(nlog.SET_MAC, new_macaddr)
        uci_r:set("network", "wan", "macaddr", new_macaddr)

        -- update device's macaddr in case wan(IPv4/IPv6) didn't set
        local secname = get_cfg_devsec(wan_ifname)
        if secname then
            uci_r:set("network", secname, "macaddr", new_macaddr)
        end
    end

    return mac_changed
end

--- Get mac configurations from IPv4 DHCP 
-- @return true if macaddr changed, else false
function set_mac_flag(form, proto_name)
    local flag = false

    if proto_name == "dhcp" and form["mac_clone_type"] then
        flag = set_mac(form)
    end

    return flag
end

local CHECK_MAC_CLONE_TBL = {
    {
        field = "mac_clone_type", canbe_absent = false,
        check = {
            {ctypes.check_in, {"default", "computer", "custom"}}
        }
    },
    {
        field = "mac_custom",
        canbe_absent = {field_canbe_empty, "mac_clone_type", "custom"},
        check = {
            ctypes.check_unicast_mac
        }
    }
}

local function set_mac_advanced(form)
    log(nlog.SET_MAC_CLONE)
    -- Check data validation
    if not ctl.check(form, CHECK_MAC_CLONE_TBL) then
        return invalid_args()
    end

    local ret = set_mac(form)

    uci_commit()
    if ret == true then
        --[[
        internet:disconnect("wanv6")
        internet:disconnect("internet")
        internet:disconnect("wan")
        nixio.nanosleep(1, 0)
        internet:reload()
        ]]--
        internet:restart()
    else
        log(nlog.NO_CHANGED)
    end
    
    return get_mac()
end

local autodetect_result_file = "/tmp/autodetect"

local function autodetect()
    local rc = luci.sys.fork_call("autodetect")
    local err = "autodetect failed"
    if rc ~= 0 then
        return false, err
    end

    local file = io.open(autodetect_result_file, "r")
    if file then
        local time = file:read()
        file:close()
        time = time and time:match("time (%d+)")
        time = time or 10000
        return {time = time}
    else
        return false, err
    end
end

local function autodetect_status()
    local conntypes = {
        "dhcp", "static", "pppoe", "bigpond", "l2tp", "pptp", "unplugged"
    }
    local file = io.open(autodetect_result_file, "r")
    if file then
        local conntype = file:read()
        file:close()
        for _, value in ipairs(conntypes) do
            if conntype == value then
                return {conntype = conntype}
            end
        end
        if conntype == "none" then
            return false, "autodetect failed"
        else
            -- Still detecting.
            return {conntype = "none"}
        end
    else
        return false, "autodetect failed"
    end
end

function get_status_wan_ipv4()
    local ret

    -- Wan mac
    local wan_macaddr = get_current_mac("wan"):gsub(":", "-")
    -- local wan_macaddr = get_default_mac()
    local lan_dft_mac = get_default_lan_mac()

    if wan_macaddr == lan_dft_mac then
        wan_macaddr = get_default_mac()
        wan_macaddr = wan_macaddr:gsub(":", "-")
    end

    wan_macaddr = wan_macaddr:upper()    

    -- Wan ipv4
    local conntypev4
    local dyninfo = {}
    local sndconninfo = {}
    conntypev4 = get_ipv4_conntype()
    if ctypes.check_3p(conntypev4) then
        dyninfo = get_wan_dyninfo("internet", true)

        local sndconn = uci_r:get("protocol", conntypev4, "snd_conn")
        if sndconn and sndconn ~= "none" then
            sndconninfo = get_wan_dyninfo("wan", true)
            sndconninfo.conn = sndconn
        end
    else
        dyninfo = get_wan_dyninfo("wan", true)
    end

    local data = {
        -- Wan mac
        wan_macaddr = wan_macaddr,
        -- Wan ipv4
        wan_ipv4_ipaddr = dyninfo.ipaddr,
        wan_ipv4_netmask = dyninfo.netmask,
        wan_ipv4_gateway = dyninfo.gateway,
        wan_ipv4_pridns = dyninfo.pri_dns,
        wan_ipv4_snddns = dyninfo.snd_dns,
        wan_ipv4_conntype = conntypev4,
        wan_ipv4_snd_conn = sndconninfo.conn,
        wan_ipv4_snd_ipaddr = sndconninfo.ipaddr,
        wan_ipv4_snd_netmask = sndconninfo.netmask,
    }

    return data
end

function get_status_lan_ipv4()
    -- Lan ipv4
    local lan4info = get_lan_ipv4() or {}

    -- dhcp
    local dhcp_enable = uci_r:get("dhcp", "lan", "ignore")
    dhcp_enable = dhcp_enable == "1" and "Off" or "On"

    -- Set data
    local data = {
        -- Lan mac
        lan_macaddr = lan4info.macaddr,
        -- Lan ipv4
        lan_ipv4_ipaddr = lan4info.ipaddr,
        lan_ipv4_netmask = lan4info.mask_type == "custom" and lan4info.custom_value or lan4info.mask_type,
        lan_ipv4_dhcp_enable = dhcp_enable,
    }

    return data
end

function get_status_ipv4()
    local data = get_status_lan_ipv4()
    util.update(data, get_status_wan_ipv4())

    return data
end

function get_status_wan_ipv6()
    -- Wan ipv6
    local conntypev6
    local dyn6info = {}
    local ret = get_ipv6_status()

    if ret then
        conntypev6 = ret.conntype
        dyn6info = get_wanv6_dyninfo(ret.enable)
    end

    local data = {
        -- Wan ipv6
        wan_ipv6_enable   = ret.enable,
        wan_ipv6_ip6addr  = dyn6info.ip6addr,
        wan_ipv6_gateway  = dyn6info.ip6gw,
        wan_ipv6_pridns   = dyn6info.pri_dns,
        wan_ipv6_snddns   = dyn6info.snd_dns,
        wan_ipv6_conntype = conntypev6,
    }

    return data
end

function get_status_lan_ipv6()
    local ret

    -- Lan ipv6
    local lan6info = get_lan_ipv6() or {}

    -- ipv6 link local addr
    local state = require("luci.model.uci").cursor_state()
    local ifname = state:get("network", "lan", "ifname")
    -- FIXME: looks unreliable
    ret = sys.exec("ifconfig %s" % ifname)
    local lladdrv6 = ret:match("inet6 addr:%s*(fe80[%x:]*/%d+)") or ""
    lladdrv6 = lladdrv6:upper()

    -- Set data
    local data = {
        -- Lan ipv6
        lan_ipv6_ipaddr = lan6info.address,
        lan_ipv6_link_local_addr = lladdrv6,
        lan_ipv6_assign_type = lan6info.assign_type
    }

    return data
end

function get_status_ipv6()
    local data = get_status_lan_ipv6()
    util.update(data, get_status_wan_ipv6())

    return data
end

function get_status_all()
    local data = get_status_ipv4()
    util.update(data, get_status_ipv6())

    return data
end

local function port_speed(form, args)
    if args == "supported" then
        return {supported = inetm.get_supported_speed()}
    elseif args ~= "current" then
        return invalid_args()
    end

    if form.operation == "read" then
        return {speed = inetm.get_port_speed()}
    elseif form.operation == "write" then
        local result = inetm.set_port_speed(form.speed)
        if result == nil then
            return invalid_args()
        end
        return {speed = result}
    else
        return invalid_args()
    end
end

-- General controller routines

local dispatch_tbl = {
    wan_autodetect = {
        ["detect"] = {cb = autodetect},
        ["read"] = {cb = autodetect_status}
    },
    wan_ipv4_protos = {
        ["read"] = {cb = get_ipv4_protos}
    },
    wan_ipv4_status = {
        ["read"] = {cb = get_ipv4_status}
    },
    wan_ipv4_dynamic = {
        [".args"] = "dhcp",
        ["read"] = {cb = get_inet_proto},
        ["renew"] = {cb = set_dhcp_connect},
        ["release"] = {cb = set_dhcp_connect},
        ["write"] = {cb = set_inet_proto}
    },
    wan_ipv4_staticip = {
        [".args"] = "static",
        ["read"] = {cb = get_inet_proto},
        ["write"] = {cb = set_inet_proto}
    },
    wan_ipv4_bigpond = {
        [".args"] = "bigpond",
        ["connect"] = {cb = set_inet_proto},
        ["disconnect"] = {cb = set_inet_proto},
        ["read"] = {cb = get_inet_proto},
        ["write"] = {cb = set_inet_proto}
    },
    wan_ipv4_pppoe = {
        [".args"] = "pppoe",
        ["connect"] = {cb = set_inet_proto},
        ["disconnect"] = {cb = set_inet_proto},
        ["renew"] = {cb = set_dhcp_connect},
        ["release"] = {cb = set_dhcp_connect},
        ["read"] = {cb = get_inet_proto},
        ["write"] = {cb = set_inet_proto}
    },
    wan_ipv4_l2tp = {
        [".args"] = "l2tp",
        ["connect"] = {cb = set_inet_proto},
        ["disconnect"] = {cb = set_inet_proto},
        ["read"] = {cb = get_inet_proto},
        ["write"] = {cb = set_inet_proto}
    },
    wan_ipv4_pptp = {
        [".args"] = "pptp",
        ["connect"] = {cb = set_inet_proto},
        ["disconnect"] = {cb = set_inet_proto},
        ["read"] = {cb = get_inet_proto},
        ["write"] = {cb = set_inet_proto}
    },
    wan_ipv6_protos = {
        ["read"] = {cb = get_ipv6_protos}
    },
    wan_ipv6_status = {
        ["read"] = {cb = get_ipv6_status},
        ["write"] = {cb = set_ipv6_status}
    },
    wan_ipv6_static = {
        [".args"] = "staticv6",
        ["read"] = {cb = get_inet_proto},
        ["write"] = {cb = set_inet_proto}
    },
    wan_ipv6_dynamic = {
        [".args"] = "dhcpv6",
        ["read"] = {cb = get_inet_proto},
        ["renew"] = {cb = set_dhcp_connect},
        ["release"] = {cb = set_dhcp_connect},
        ["write"] = {cb = set_inet_proto}
    },
    wan_ipv6_pppoe = {
        [".args"] = "pppoev6",
        ["connect"] = {cb = set_inet_proto},
        ["disconnect"] = {cb = set_inet_proto},
        ["read"] = {cb = get_inet_proto},
        ["write"] = {cb = set_inet_proto}
    },
    wan_ipv6_tunnel = {
        [".args"] = "6to4",
        ["connect"] = {cb = set_inet_proto},
        ["disconnect"] = {cb = set_inet_proto},
        ["read"] = {cb = get_inet_proto},
        ["write"] = {cb = set_inet_proto}
    },
    wan_ipv6_6rd = {
        [".args"] = "6rd",
        ["connect"] = {cb = set_inet_proto},
        ["disconnect"] = {cb = set_inet_proto},
        ["read"] = {cb = get_inet_proto},
        ["write"] = {cb = set_inet_proto}
    },
    wan_ipv6_dslite = {
        [".args"] = "dslite",
        ["connect"] = {cb = set_inet_proto},
        ["disconnect"] = {cb = set_inet_proto},
        ["renew"] = {cb = set_dhcp_connect},
        ["release"] = {cb = set_dhcp_connect},
        ["read"] = {cb = get_inet_proto},
        ["write"] = {cb = set_inet_proto}
    },
    wan_ipv6_pass = {
        [".args"] = "passthrough",
        ["connect"] = {cb = set_inet_proto},
        ["disconnect"] = {cb = set_inet_proto},
        ["read"] = {cb = get_inet_proto},
        ["write"] = {cb = set_inet_proto}
    },
    mac_clone_advanced = {
        ["read"] = {cb = get_mac},
        ["write"] = {cb = set_mac_advanced},
    },
    port_speed_supported = {
        [".args"] = "supported",
        [".super"] = {cb = port_speed},
    },
    port_speed_current = {
        [".args"] = "current",
        ["read"] = {cb = port_speed},
        ["write"] = {cb = port_speed},
    },
    lan_ipv4 = {
        ["read"] = {cb = get_lan_ipv4},
        ["write"] = {cb = set_lan_ipv4},
    },
    lan_ipv6 = {
        ["read"] = {cb = get_lan_ipv6},
        ["write"] = {cb = set_lan_ipv6},
    },
    routes_static = {
        ["load"]   = {cb = route.route_cfg_list, others = route.route_cfg_max},
        ["insert"] = {cb = route.route_insert, others = route.route_cfg_max, cmd = NETWORK_RELOAD},
        ["update"] = {cb = route.route_update, others = route.route_cfg_max, cmd = NETWORK_RELOAD},
        ["remove"] = {cb = route.route_delete, others = route.route_cfg_max, cmd = NETWORK_RELOAD},
    },
    routes_system = {
        ["load"] = {cb = route.route_sys_list}
    },
    status_ipv4 = {
        [".super"] = {cb = get_status_ipv4}
    },
    status_ipv4_lan = {
        [".super"] = {cb = get_status_lan_ipv4}
    },
    status_ipv4_wan = {
        [".super"] = {cb = get_status_wan_ipv4}
    },
    status_ipv6 = {
        [".super"] = {cb = get_status_ipv6}
    },
    status_ipv6_lan = {
        [".super"] = {cb = get_status_lan_ipv6}
    },
    status_ipv6_wan = {
        [".super"] = {cb = get_status_wan_ipv6}
    },
    status_all = {
        [".super"] = {cb = get_status_all}
    }
}

function dispatch(http_form)
    local function hook(success, action)
        if success and action.cmd then
            if type(action.cmd) == "table" then
                for i = 1, #action.cmd - 1 do
                    sys.fork_call(action.cmd[i])
                end
                sys.fork_exec(action.cmd[#action.cmd])
            else
                sys.fork_exec(action.cmd)
            end
        end
        return true
    end

    return ctl.dispatch(dispatch_tbl, http_form, {post_hook = hook})
end

function _index()
    return ctl._index(dispatch)
end

function index()
    entry({"admin", "network"}, call("_index")).leaf = true
end

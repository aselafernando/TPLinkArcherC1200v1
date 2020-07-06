--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  protodata.lua
Details :  Protocol data description
Author  :  Zhu Xianfeng <zhuxianfeng@tp-link.net>
Author  :  Ye Qianchuan <yeqianchuan@tp-link.net>
Author  :  Wen Kun <wenkun@tp-link.net>
Version :  1.0.0
Date    :  15 Dec, 2013
]]--

module("luci.model.protodata", package.seeall)

local logm   = require "luci.model.log"
local nlog   = require "luci.model.network_log"
local chk    = require "luci.model.checktypes"
local log    = logm.Log(nlog.ID)

PROTO_TBL = {
    dhcp = {
        proto = "dhcp", wan_type = "dhcp",
        {key = "ipaddr", readonly = true},
        {key = "netmask", readonly = true},
        {key = "gateway", readonly = true},
        {key = "pri_dns", readonly = true},
        {key = "snd_dns", readonly = true},
        {key = "dns_mode"},
        {key = "dyn_pridns", readonly = true},
        {key = "dyn_snddns", readonly = true},
        {key = "manual_pridns"},
        {key = "manual_snddns"},
        {key = "mtu"},
        {key = "hostname"},
        {key = "mac_clone_type"},
        {key = "unicast"}
    },
    bigpond = {
        proto = "bigpond", wan_type = "bigpond",
        {key = "username"},
        {key = "password"},
        {key = "server"},
        {key = "domain"},
        {key = "mtu"},
        {key = "conn_mode"},
        {key = "demand_idle"},
        {key = "manual_idle"}
    },
    static = {
        proto = "static", wan_type = "static",
        {key = "ipaddr"},
        {key = "netmask"},
        {key = "gateway"},
        {key = "pri_dns"},
        {key = "snd_dns"},
        {key = "mtu"}
    },
    pppoe = {
        proto = "pppoe", wan_type = "pppoe",
        {key = "username"},
        {key = "password"},
        {key = "inet_ip", readonly = true},
        {key = "inet_pridns", readonly = true},
        {key = "inet_snddns", readonly = true},
        {key = "snd_conn"},
        {key = "dyn_ip", readonly = true},
        {key = "dyn_netmask", readonly = true},
        {key = "static_ip"},
        {key = "static_netmask"},
        {key = "mtu"},
        {key = "server"},
        {key = "access"},
        {key = "interval"},
        {key = "ip_mode"},
        {key = "specific_ip"},
        {key = "dns_mode"},
        {key = "dyn_pridns", readonly = true},
        {key = "dyn_snddns", readonly = true},
        {key = "static_pridns"},
        {key = "static_snddns"},
        {key = "conn_mode"},
        {key = "demand_idle"},
        {key = "time_start"},
        {key = "time_end"},
        {key = "manual_idle"}
    },
    l2tp = {
        proto = "l2tp", wan_type = "l2tp",
        {key = "username"},
        {key = "password"},
        {key = "inet_ip", readonly = true},
        {key = "inet_pridns", readonly = true},
        {key = "inet_snddns", readonly = true},
        {key = "snd_conn"},
        {key = "dyn_server"},
        {key = "dyn_ip", readonly = true},
        {key = "dyn_netmask", readonly = true},
        {key = "dyn_gateway", readonly = true},
        {key = "dyn_pridns", readonly = true},
        {key = "dyn_snddns", readonly = true},
        {key = "static_server"},
        {key = "static_ip"},
        {key = "static_netmask"},
        {key = "static_gateway"},
        {key = "static_pridns"},
        {key = "static_snddns"},
        {key = "mtu"},
        {key = "conn_mode"},
        {key = "demand_idle"},
        {key = "manual_idle"}
    },
    pptp = {
        proto = "pptp", wan_type = "pptp",
        {key = "username"},
        {key = "password"},
        {key = "inet_ip", readonly = true},
        {key = "inet_pridns", readonly = true},
        {key = "inet_snddns", readonly = true},
        {key = "snd_conn"},
        {key = "dyn_server"},
        {key = "dyn_ip", readonly = true},
        {key = "dyn_netmask", readonly = true},
        {key = "dyn_gateway", readonly = true},
        {key = "dyn_pridns", readonly = true},
        {key = "dyn_snddns", readonly = true},
        {key = "static_server"},
        {key = "static_ip"},
        {key = "static_netmask"},
        {key = "static_gateway"},
        {key = "static_pridns"},
        {key = "static_snddns"},
        {key = "mtu"},
        {key = "conn_mode"},
        {key = "demand_idle"},
        {key = "manual_idle"}
    },
    staticv6 = {
        proto = "static", wan_type = "staticv6",
        {key = "ip6addr"},
        {key = "ip6gw"},
        {key = "pri_dns"},
        {key = "snd_dns"},
        {key = "mtu"}
    },
    pppoev6 = {
        proto = "pppoev6", wan_type = "pppoev6",
        ipv6 = true, value = "PPPOE",
        {key = "username"},
        {key = "password"},
        {key = "ip6addr", readonly = true},
        {key = "ip_mode"},
        {key = "dns_mode"},
	{key = "ip_config"},
        {key = "dyn_pridns", readonly = true},
        {key = "dyn_snddns", readonly = true},
        {key = "static_pridns"},
        {key = "static_snddns"},
        {key = "specific_ip"}
    },
    dhcpv6= {
        proto = "dhcp6c", wan_type = "dhcp6c",
        {key = "ip6addr", readonly = true},
        {key = "prefix", readonly = true},
        {key = "pri_dns", readonly = true},
        {key = "snd_dns", readonly = true},
		{key = "ip_config"},
        {key = "ip_mode"},
        {key = "dns_mode"},
        {key = "dyn_pridns", readonly = true},
        {key = "dyn_snddns", readonly = true},
        {key = "static_pridns"},
        {key = "static_snddns"}
    },
    ["6to4"] = {
        proto = "6to4", wan_type = "6to4",
        {key = "ipaddr", readonly = true},
        {key = "netmask", readonly = true},
        {key = "gateway", readonly = true},
        {key = "tunnel_addr", readonly = true},
        {key = "dns_mode"},
        {key = "pri_dns"},
        {key = "snd_dns"}
    },
    ["6rd"] = {
        proto = "6rd", wan_type = "6rd",
        {key = "ip_mode",},
        {key = "ipv4_mask_len",},
        {key = "6rd_prefix",},
        {key = "6rd_prefix_len",},
        {key = "relay_ipv4_addr",},
    },
    dslite = {
        proto = "dslite", wan_type = "dslite",
        {key = "AFTR_name",},
        {key = "snd_conn",},
        --staticv6
        {key = "static_ip6addr"},
        {key = "static_ip6gw"},
        {key = "static_pri_dns"},
        {key = "static_snd_dns"},
        {key = "static_mtu"},
        --dhcpv6
        {key = "dynamic_ip6addr", readonly = true},
        {key = "dynamic_prefix", readonly = true},
        {key = "dynamic_pri_dns", readonly = true},
        {key = "dynamic_snd_dns", readonly = true},
		{key = "dynamic_ip_config"},
        {key = "dynamic_ip_mode"},
        {key = "dynamic_dns_mode"},
        {key = "dynamic_dyn_pridns", readonly = true},
        {key = "dynamic_dyn_snddns", readonly = true},
        {key = "dynamic_static_pridns"},
        {key = "dynamic_static_snddns"},
        --pppoev6
        {key = "pppoe_username"},
        {key = "pppoe_password"},
        {key = "pppoe_dns_mode"},
        {key = "pppoe_static_pridns"},
        {key = "pppoe_static_snddns"},
        {key = "pppoe_ip6addr"},
        {key = "pppoe_ip_mode"},
		{key = "pppoe_ip_config"},
        {key = "pppoe_specific_ip"},
        
    },
    passthrough = {
        proto = "passthrough", wan_type = "passthrough",
        {key = "type",}
    }
}

local function dns_concat(...)
    local t = {}
    for _, v in ipairs{...} do
        if #v > 0 then
            t[#t + 1] = v
        end
    end
    return table.concat(t, " ")
end

local function proto_cfg_time(proto, inet)
    if proto.conn_mode == "demand" then
        inet.idle_time = tonumber(proto.demand_idle) * 60
    elseif proto.conn_mode == "manually" then
        inet.idle_time = tonumber(proto.manual_idle) * 60
    elseif proto.conn_mode == "time_based" then
        local _, _, hour, minute = string.find(proto.time_start, "(%d+):(%d+)")

        if hour ~= nil and minute ~= nil then
            inet.start_time = (tonumber(hour) * 60 + tonumber(minute)) * 60
        end

        _, _, hour, minute = string.find(proto.time_end, "(%d+):(%d+)")
        if hour ~= nil and minute ~= nil then
            inet.end_time = (tonumber(hour) * 60 + tonumber(minute)) * 60
        end
    elseif proto.conn_mode == "auto" then
        -- TODO
    end
end

local function proto_cfg_xxtp(proto, proto_name)
    local xxtp_wan = {
        wan_type = PROTO_TBL[proto_name].wan_type,
        ifname   = proto.ifname,
        macaddr  = proto.macaddr,
        mtu      = "1500",
        auto     = "0"
    }

    if proto.snd_conn == "dynamic" then
        xxtp_wan.proto = "dhcp"
    else
        xxtp_wan.proto = "static"
        xxtp_wan.ipaddr = proto.static_ip
        xxtp_wan.netmask = proto.static_netmask
        xxtp_wan.gateway = proto.static_gateway
        xxtp_wan.dns = dns_concat(proto.static_pridns, proto.static_snddns)
    end
    xxtp_wan.connectable = proto.connect

    local xxtp_inet = {
        proto       = PROTO_TBL[proto_name].proto,
        parent      = "wan",
        wan_type    = proto_name,
        username    = proto.username,
        password    = proto.password,
        mru         = proto.mtu,
        auto        = "0",
        conn_mode   = proto.conn_mode,
        connectable = proto.connect,
    }

    if proto.snd_conn == "dynamic" then
        xxtp_inet.server = proto.dyn_server or ""
    else
        xxtp_inet.server = proto.static_server or ""
    end

    proto_cfg_time(proto, xxtp_inet)

    log(nlog.SET_XXTP_BPA, proto_name, xxtp_inet.username, xxtp_inet.server)
    return { wan = xxtp_wan, internet = xxtp_inet}
end

local function proto_cfg_pppoe(proto, proto_name)
    local pppoe_wan = {
        wan_type = PROTO_TBL[proto_name].wan_type,
        ifname   = proto.ifname,
        macaddr  = proto.macaddr,
        mtu      = "1500",
        auto     = "0"
    }

    if proto.snd_conn == "none" then
        pppoe_wan.proto = "none"
    elseif proto.snd_conn == "dynamic" then
        pppoe_wan.proto = "dhcp"
    else
        pppoe_wan.proto = "static"
        pppoe_wan.ipaddr = proto.static_ip
        pppoe_wan.netmask = proto.static_netmask
    end
    pppoe_wan.connectable = proto.connect

    local pppoe_inet = {
        proto       = PROTO_TBL[proto_name].proto,
        parent      = "wan",
        ifname      = proto.ifname,
        username    = proto.username,
        password    = proto.password,
        mru         = proto.mtu,
        service     = proto.server,
        ac          = proto.access,
        ip_mode     = proto.ip_mode,
        dns_mode    = proto.dns_mode,
        ipaddr      = proto.ip_mode == "static" and proto.specific_ip or "",
        dns         = proto.dns_mode == "static" and dns_concat(proto.static_pridns, proto.static_snddns) or "",
        keepalive   = (proto.interval == "0" or proto.interval == nil) and "" or (5 .. " " .. proto.interval),
        auto        = "0",
        conn_mode   = proto.conn_mode,
        connectable = proto.connect,
    }

    proto_cfg_time(proto, pppoe_inet)
    log(nlog.SET_PPPOE, pppoe_inet.username)
    return {wan = pppoe_wan, internet = pppoe_inet}
end

local function proto_cfg_bigpond(proto, proto_name)
    local bigpond_wan = {
        proto       = PROTO_TBL[proto_name].proto,
        wan_type    = PROTO_TBL[proto_name].wan_type,
        ifname      = proto.ifname,
        macaddr     = proto.macaddr,
        mtu         = proto.mtu,
        conn_mode   = proto.conn_mode,
        auto        = proto.auto,
        username    = proto.username,
        password    = proto.password,
        server      = proto.server,
        domain      = proto.domain,
        connectable = proto.connect,
    }

    proto_cfg_time(proto, bigpond_wan)
    log(nlog.SET_XXTP_BPA, proto_name, bigpond_wan.username, bigpond_wan.server)
    return {wan = bigpond_wan}
end

local function proto_cfg_dhcp(proto, proto_name)
    local dhcp_wan = {
        proto       = PROTO_TBL[proto_name].proto,
        wan_type    = PROTO_TBL[proto_name].wan_type,
        ifname      = proto.ifname,
        macaddr     = proto.macaddr,
        mtu         = proto.mtu,
        auto        = "0",
        hostname    = proto.hostname,
        broadcast   = proto.unicast == "off" and "1" or "0",
        peerdns     = proto.dns_mode == "static" and "0" or "1",
        connectable = proto.connect,
        conn_mode   = "auto",
    }

    if proto.dns_mode == "static" then
        dhcp_wan.dns = dns_concat(proto.manual_pridns, proto.manual_snddns)
    end

    return {wan = dhcp_wan}
end

local function proto_cfg_static(proto, proto_name)
    local static_wan = {
        proto       = PROTO_TBL[proto_name].proto,
        wan_type    = PROTO_TBL[proto_name].wan_type,
        ifname      = proto.ifname,
        macaddr     = proto.macaddr,
        mtu         = proto.mtu,
        auto        = proto.auto,
        ipaddr      = proto.ipaddr,
        netmask     = proto.netmask,
        gateway     = proto.gateway,
        dns         = dns_concat(proto.pri_dns, proto.snd_dns),
        conn_mode   = "auto",
        connectable = "1",
    }

    log(nlog.SET_STATIC, static_wan.ipaddr, static_wan.netmask)
    return {wan = static_wan}
end

local function proto_cfg_staticv6(proto, proto_name)
    local static_wanv6 = {
        proto       = PROTO_TBL[proto_name].proto,
        wan_type    = PROTO_TBL[proto_name].wan_type,
        ifname      = proto.ifname,
        mtu         = proto.mtu,
        auto        = proto.auto,
        ip6addr     = proto.ip6addr,
        ip6gw       = proto.ip6gw,
        dns         = dns_concat(proto.pri_dns, proto.snd_dns),
        conn_mode   = "auto",
        connectable = "1",
    }

    log(nlog.SET_STATICV6, static_wanv6.ip6addr)
    return {wanv6 = static_wanv6}
end

local function proto_cfg_dhcpv6(proto, proto_name)
    local dhcpv6_wanv6 = {
        proto       = PROTO_TBL[proto_name].proto,
        wan_type    = PROTO_TBL[proto_name].wan_type,
        ifname      = proto.ifname,
        lanif       = proto.lanif,
		ip_config   = proto.ip_config,
        ip_mode     = proto.ip_mode,
        auto        = proto.auto,
        dns_mode    = proto.dns_mode,
        peerdns     = proto.dns_mode == "static" and "0" or "1",
        connectable = proto.connect,
        conn_mode   = "auto",
    }

    if proto.dns_mode == "static" then
        dhcpv6_wanv6.dns = dns_concat(proto.static_pridns, proto.static_snddns)
    end

    return {wanv6 = dhcpv6_wanv6}
end

local function proto_cfg_pppoev6(proto, proto_name)
    local pppoe_wanv6 = {
        proto       = PROTO_TBL[proto_name].proto,
        wan_type    = PROTO_TBL[proto_name].wan_type,
        ifname      = proto.ifname,
        lanif       = proto.lanif,
        auto        = proto.auto,
        mru         = proto.mru,
        keeplive    = proto.keeplive,
        dns_mode    = proto.dns_mode,
        peerdns     = proto.dns_mode == "static" and "0" or "1",
        ip_mode     = proto.ip_mode,
		ip_config   = proto.ip_config,
        username    = proto.username,
        password    = proto.password,
        connectable = proto.connect,
        conn_mode   = "auto",
    }

    if proto.ip_config == "specified" then
        pppoe_wanv6.ip6addr = proto.specific_ip
    end

    if proto.dns_mode == "static" then
        pppoe_wanv6.dns = dns_concat(proto.static_pridns, proto.static_snddns)
    end

    log(nlog.SET_PPPOEV6, pppoe_wanv6.username)
    return {wanv6 = pppoe_wanv6}
end

local function proto_cfg_6to4(proto, proto_name)
    local tunnel_wanv6 = {
        proto       = PROTO_TBL[proto_name].proto,
        wan_type    = PROTO_TBL[proto_name].wan_type,
        ifname      = proto.ifname,
        ttl         = proto.ttl,
        auto        = proto.auto,
        connectable = proto.connect,
        conn_mode   = "auto",
    }

    if proto.dns_mode == "static" then
        tunnel_wanv6.dns = dns_concat(proto.pri_dns, proto.snd_dns)
    end

    return {wanv6 = tunnel_wanv6}
end

local function proto_cfg_6rd(proto, proto_name)
    local tunnel_6rd_wanv6 = {
        proto          = PROTO_TBL[proto_name].proto,
        wan_type       = PROTO_TBL[proto_name].wan_type,
        ip_mode        = proto.ip_mode,
        ip4prefixlen   = proto.ipv4_mask_len,
        ip6prefix      = proto["6rd_prefix"],
        ip6prefixlen   = proto["6rd_prefix_len"],
        peeraddr       = proto.relay_ipv4_addr,
        connectable    = proto.connect,
        conn_mode      = "auto",
    }
    if proto.ip_mode == "auto" then
        tunnel_6rd_wanv6.ip4prefixlen = 0
        tunnel_6rd_wanv6.ip6prefix    = "2001:db8::"
        tunnel_6rd_wanv6.ip6prefixlen = 32
        tunnel_6rd_wanv6.peeraddr     = "69.252.80.66"
    end
    
    return {wanv6 = tunnel_6rd_wanv6}
end

local function proto_cfg_dslite(proto, proto_name)
    local dslite_inte6 = {
        proto           = PROTO_TBL[proto_name].proto,
        wan_type        = PROTO_TBL[proto_name].wan_type,
        snd_conn        = proto.snd_conn,
        conn_status     = proto.conn_status,
        connectable     = proto.connect,
        conn_mode       = "auto",
        parent          = "wanv6"
    }
    if chk.check_ipv6(proto.AFTR_name) then
        dslite_inte6.peeraddr = proto.AFTR_name
    else
        dslite_inte6.AFTR_name = proto.AFTR_name
    end
    local dslite_wanv6 = {
        connectable = proto.connect,
        conn_mode   = "auto",
        ifname      = proto.ifname,
        lanif       = proto.lanif,
        auto        = proto.auto,
    }
    if proto.snd_conn == "dynamic" then
        dslite_wanv6.proto       = "dhcp6c"
        dslite_wanv6.wan_type    = "dhcp6c"
		dslite_wanv6.ip_config   = proto.dynamic_ip_config
        dslite_wanv6.ip_mode     = proto.dynamic_ip_mode
        dslite_wanv6.dns_mode    = proto.dynamic_dns_mode
        dslite_wanv6.peerdns     = proto.dynamic_dns_mode == "static" and "0" or "1"
        dslite_wanv6.connectable = proto.connect
        if proto.dynamic_dns_mode == "static" then
            dslite_wanv6.dns = dns_concat(proto.dynamic_static_pridns, proto.dynamic_static_snddns)
        end
    elseif proto.snd_conn == "static" then
            dslite_wanv6.proto       = 'static'
            dslite_wanv6.wan_type    = 'static'
            dslite_wanv6.mtu         = proto.static_mtu
            dslite_wanv6.ip6addr     = proto.static_ip6addr
            dslite_wanv6.ip6gw       = proto.static_ip6gw
            dslite_wanv6.dns         = dns_concat(proto.static_pri_dns, proto.static_snd_dns)
    else 
            dslite_wanv6.proto       = 'pppoev6'
            dslite_wanv6.wan_type    = 'pppoev6'
            dslite_wanv6.mru         = 1492
            dslite_wanv6.keeplive    = proto.keeplive
            dslite_wanv6.dns_mode    = proto.pppoe_dns_mode
            dslite_wanv6.ip_mode     = proto.pppoe_ip_mode
			dslite_wanv6.ip_config     = proto.pppoe_ip_config
            dslite_wanv6.username    = proto.pppoe_username
            dslite_wanv6.password    = proto.pppoe_password
        if proto.pppoe_ip_mode == "specified" then
            dslite_wanv6.ip6addr = proto.pppoe_specific_ip
        end
    end
    return {internetv6 = dslite_inte6 ,  wanv6= dslite_wanv6}
end

local function proto_cfg_passthr(proto, proto_name)
    local passthr_wanv6 = {
        proto          = PROTO_TBL[proto_name].proto,
        wan_type       = PROTO_TBL[proto_name].wan_type,
        passtype       = proto.type,
        connectable    = proto.connect,
        conn_mode      = "auto",
    } 

    return {wanv6 = passthr_wanv6}
end

PROTO_CFG_TBL = {
    ["static"]      = proto_cfg_static,
    ["dhcp"]        = proto_cfg_dhcp,
    ["pppoe"]       = proto_cfg_pppoe,
    ["pptp"]        = proto_cfg_xxtp,
    ["l2tp"]        = proto_cfg_xxtp,
    ["bigpond"]     = proto_cfg_bigpond,
    ["staticv6"]    = proto_cfg_staticv6,
    ["dhcpv6"]      = proto_cfg_dhcpv6,
    ["pppoev6"]     = proto_cfg_pppoev6,
    ["6to4"]        = proto_cfg_6to4,
    ["6rd"]         = proto_cfg_6rd,
    ["dslite"]      = proto_cfg_dslite,
    ["passthrough"] = proto_cfg_passthr,
}

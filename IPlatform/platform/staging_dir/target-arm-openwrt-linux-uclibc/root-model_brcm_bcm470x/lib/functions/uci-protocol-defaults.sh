#!/bin/sh /etc/rc.common
# Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.
#
# Details : The default config script for protocol.
# Author  : Wen Kun <wenkun@tp-link.net>
# Version : 1.0
# Date    : 27 Mar, 2014

ucidef_set_proto_dhcp(){
    local ifname=$1

    uci batch <<EOF
set protocol.dhcp='proto'
set protocol.dhcp.ifname='$ifname'
set protocol.dhcp.unicast='off'
set protocol.dhcp.dns_mode='dynamic'
set protocol.dhcp.mtu='1500'
set protocol.dhcp.pri_dns='0.0.0.0'
set protocol.dhcp.snd_dns='0.0.0.0'
set protocol.dhcp.hostname='Akronite'
EOF
}

ucidef_set_protocol_static(){
    local ifname=$1

    uci batch <<EOF
set protocol.static='proto'
set protocol.static.ifname='$ifname'
set protocol.static.mtu='1500'
set protocol.static.auto='0'
EOF
}

ucidef_set_protocol_pppoe(){
    local ifname=$1

    uci batch <<EOF
set protocol.pppoe='proto'
set protocol.pppoe.ifname='$ifname'
set protocol.pppoe.snd_conn='none'
set protocol.pppoe.ip_mode='dynamic'
set protocol.pppoe.conn_mode='auto'
set protocol.pppoe.dns_mode='dynamic'
set protocol.pppoe.mtu='1480'
set protocol.pppoe.interval='0'
set protocol.pppoe.manual_idle='15'
set protocol.pppoe.demand_idle='15'
set protocol.pppoe.time_start='00:00'
set protocol.pppoe.time_end='23:59'
set protocol.pppoe.specific_ip='0.0.0.0'
EOF
}

ucidef_set_protocol_bigpond(){
    local ifname=$1

    uci batch <<EOF
set protocol.bigpond='proto'
set protocol.bigpond.ifname='$ifname'
set protocol.bigpond.mtu='1500'
set protocol.bigpond.conn_mode='auto'
set protocol.bigpond.manual_idle='15'
set protocol.bigpond.demand_idle='15'
set protocol.bigpond.auto='0'
EOF
}

ucidef_set_protocol_l2tp(){
    local ifname=$1

    uci batch <<EOF
set protocol.l2tp='proto'
set protocol.l2tp.ifname='$ifname'
set protocol.l2tp.snd_conn='dynamic'
set protocol.l2tp.mtu='1460'
set protocol.l2tp.conn_mode='auto'
set protocol.l2tp.manual_idle='15'
set protocol.l2tp.demand_idle='15'
EOF
}

ucidef_set_protocol_pptp(){
    local ifname=$1

    uci batch <<EOF
set protocol.pptp='proto'
set protocol.pptp.ifname='$ifname'
set protocol.pptp.snd_conn='dynamic'
set protocol.pptp.mtu='1420'
set protocol.pptp.conn_mode='auto'
set protocol.pptp.manual_idle='15'
set protocol.pptp.demand_idle='15'
EOF
}

ucidef_set_protocol_wan(){
    uci batch <<EOF
set protocol.wan='interface'
set protocol.wan.mactype='default'
set protocol.wan.custom_mac=''
EOF
}

ucidef_set_protocol_wan6(){
    uci batch <<EOF
set protocol.wanv6='interface'
set protocol.wanv6.proto='none'
set protocol.wanv6.enable='off'
EOF
}


ucidef_set_protocol_ip6static(){
    local ifname=$1

    uci batch <<EOF
set protocol.ipv6_static='proto'
set protocol.ipv6_static.ifname='$ifname'
set protocol.ipv6_static.mtu='1500'
set protocol.ipv6_static.auto='0'
EOF
}

ucidef_set_protocol_ip6dhcp(){
    local ifname=$1

    uci batch <<EOF
set protocol.ipv6_dynamic='proto'
set protocol.ipv6_dynamic.ifname='$ifname'
set protocol.ipv6_dynamic.lanif='br-lan'
set protocol.ipv6_dynamic.ip_mode='non_temp'
set protocol.ipv6_dynamic.dns_mode='dynamic'
set protocol.ipv6_dynamic.peerdns='1'
set protocol.ipv6_dynamic.auto='0'
EOF
}

ucidef_set_protocol_ip6pppoe(){
    local ifname=$1

    uci batch <<EOF
set protocol.ipv6_pppoe='proto'
set protocol.ipv6_pppoe.ifname='$ifname'
set protocol.ipv6_pppoe.lanif='br-lan'
set protocol.ipv6_pppoe.auto='0'
set protocol.ipv6_pppoe.mru='1492'
set protocol.ipv6_pppoe.keeplive='5 20'
set protocol.ipv6_pppoe.dns_mode='dynamic'
set protocol.ipv6_pppoe.ip_mode='non_temp'
EOF
}

ucidef_set_protocol_ip6tunnel(){
    local ifname=$1

    uci batch <<EOF
set protocol.ipv6_6to4='proto'
set protocol.ipv6_6to4.ifname='$ifname'
set protocol.ipv6_6to4.ttl='255'
set protocol.ipv6_6to4.dns_mode='dynamic'
set protocol.ipv6_6to4.auto='0'
EOF
}

#lanv6 dhcp6s
ucidef_set_protocol_dhcp6s(){

    uci batch <<EOF
set protocol.dhcp6s='proto'
set protocol.dhcp6s.ifname='br-lan'
set protocol.dhcp6s.prefixlen='64'
set protocol.dhcp6s.auto='1'
set protocol.dhcp6s.startip='1000'
set protocol.dhcp6s.endip='2000'
set protocol.dhcp6s.leasetime='86400'
EOF
}

#lanv6 slaac
ucidef_set_protocol_slaac(){

    uci batch <<EOF
set protocol.slaac='proto'
set protocol.slaac.ifname='br-lan'
set protocol.slaac.prefixlen='64'
set protocol.slaac.auto='1'
EOF
}

ucidef_set_protocol(){
    local ifname=$1

    ucidef_set_protocol_wan 
    ucidef_set_proto_dhcp "$ifname"
    ucidef_set_protocol_static "$ifname"
    ucidef_set_protocol_bigpond "$ifname"
    ucidef_set_protocol_pppoe "$ifname"
    ucidef_set_protocol_l2tp "$ifname"
    ucidef_set_protocol_pptp "$ifname"
    
    ucidef_set_protocol_wan6    
    ucidef_set_protocol_ip6static "$ifname"
    ucidef_set_protocol_ip6dhcp "$ifname"
    ucidef_set_protocol_ip6pppoe "$ifname"
    ucidef_set_protocol_ip6tunnel "$ifname"

    #lanv6
    ucidef_set_protocol_dhcp6s
    ucidef_set_protocol_slaac    
}


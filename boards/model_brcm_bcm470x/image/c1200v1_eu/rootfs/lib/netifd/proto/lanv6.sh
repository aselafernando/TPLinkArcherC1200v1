#!/bin/sh
# dhcp6s.sh - IPv6-in-IPv4 tunnel backend
# Copyright (c) 2010-2012 OpenWrt.org

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. /lib/functions/network.sh
	. /usr/share/libubox/jshn.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

get_ifname_macaddr() {
	local ifname="$1"
	local __var="$2"
	
	local __tmp=`ubus call network.device status "{\"name\":\"$ifname\"}" 2>/dev/null`	
	json_load "${__tmp:-{}}"
	json_get_var __tmp "macaddr"
	
	eval "export -- \"$__var=$__tmp\""
}

get_eui_64_id() {
	local macaddr="$1"
	local __var="$2"
	
	local oIFS="$IFS"; IFS=":"; set -- $macaddr; IFS="$oIFS"
	local tmp="0x$1"
	tmp=$(($tmp))		
	tmp=$(($tmp^2))
	tmp=`printf %02x $tmp`
	
	local id="$tmp$2:$3ff:fe$4:$5$6"
	
	eval "export -- \"$__var=$id\""
}

get_tunnel_prefix() {
	local ip4="$1"
	local oIFS="$IFS"; IFS="."; set -- $ip4; IFS="$oIFS"
	printf "2002:%02x%02x:%02x%02x:1::\n" $1 $2 $3 $4
}

proto_lan_setup() {
	local iface="$1"
	local ifname="$2"
	
	local ip6addr macaddr ifaceid
	local wan_type 
	local lanif_name
	
	local prefix prefixlen proto
	json_get_vars prefix prefixlen proto
	lanif_name=$(uci get network.lanv6.ifname)
	ifconfig $lanif_name down
	ifconfig $lanif_name up

	# make sure to get the latest prefix
	prefix=$(uci get network.lanv6.prefix)
	
	get_ifname_macaddr $ifname macaddr
	
	[ -z "$macaddr" ] && {
		proto_notify_error "$iface" "GET_MACADDR_ERROR"
		return
	}
	
	get_eui_64_id "$macaddr" ifaceid
	
	[ -z "$ifaceid" ] && {
		proto_notify_error "$iface" "GET_EUI-ID_ERROR"
		return	
	}
	
	config_load /etc/config/network
	config_get wan_type wanv6 wan_type
	
	[ "$wan_type" == "6to4" ] && {
		local wanif ipaddr
		if ! network_find_wan wanif; then
			proto_notify_error "$iface" "NO_WAN_LINK"
			return
		fi
			
		if ! network_get_ipaddr ipaddr "$wanif"; then
			proto_notify_error "$iface" "NO_WAN_ADDRESS"	
			return
		fi
		prefix=$(get_tunnel_prefix "$ipaddr")
		prefixlen="64"
	}
	
	[ "$wan_type" == "6rd" ] && {
		local wanif ipaddr
		if ! network_find_wan wanif; then
			proto_notify_error "$iface" "NO_WAN_LINK"
			return
		fi
			
		if ! network_get_ipaddr ipaddr "$wanif"; then
			proto_notify_error "$iface" "NO_WAN_ADDRESS"	
			return
		fi

		local ip6prefix=$(uci get network.wanv6.ip6prefix)
		local ip6prefixlen=$(uci get network.wanv6.ip6prefixlen)
		local ip4prefixlen=$(uci get network.wanv6.ip4prefixlen)
		local ip6subnet=$(6rdcalc "$ip6prefix/$ip6prefixlen" "$ipaddr/$ip4prefixlen")

		#prefix="${ip6subnet%%::*}"
		prefix="${ip6subnet%%::*}:1::"
		prefixlen="64"
	}

	[ -z "$prefix" ] && {
		proto_notify_error "$iface" "INVALID_PREFIX"
		return
	}
	
	ip6addr="${prefix}${ifaceid}"
	
	local oIFS="$IFS"; IFS=":"; set -- $prefix; IFS="$oIFS"
	[ "$#" == "5" ] && {
		ip6addr="${prefix%:}${ifaceid}"
	}
	[ "$#" == "6" ] && {
		ip6addr="${prefix%%1::}${ifaceid}"
	}
	
	# [ "$wan_type" == "6rd" ] && ip6addr="${prefix}:1::1"

	prefixlen="${prefixlen:-64}"
	
	proto_init_update "$ifname" 1
	proto_add_ipv6_address "$ip6addr" "$prefixlen"
	proto_send_update "$iface"
}

proto_dhcp6s_init_config() {
	proto_config_add_string "startip"
	proto_config_add_string "endip"
	proto_config_add_int	"leasetime"
	proto_config_add_string "prefix"
	proto_config_add_int "prefixlen"
}

proto_dhcp6s_setup() {
	proto_lan_setup "$1" "$2"
}

proto_lan_teardown() {

	local ifname="$2"
	local dhcp6sdir="/tmp/dhcp6s-$ifname"
	local radvddir="/tmp/radvd-$ifname"
	local dhcppidfile="$dhcp6sdir/dhcp6s.pid"
	local radvdpidfile="$radvddir/radvd.pid"
	
	[ -f "$dhcppidfile" ] && kill -15 `cat $dhcppidfile`
	[ -f "$radvdpidfile" ] &&  kill -15 `cat $radvdpidfile`
	
	[ -d "$dhcp6sdir" ] && rm -rf "$dhcp6sdir"
	[ -d "$radvddir" ] && rm -rf "$radvddir"
}

proto_dhcp6s_teardown() {
	proto_lan_teardown $@
}

proto_slaac_init_config() {
	proto_config_add_string "prefix"
	proto_config_add_int "prefixlen"
}

proto_slaac_setup() {
	proto_lan_setup "$1" "$2"
}

proto_slaac_teardown() {
	proto_lan_teardown $@
}

proto_rdnss_init_config() {
	proto_config_add_string "prefix"
	proto_config_add_int "prefixlen"
}

proto_rdnss_setup() {
	proto_lan_setup "$1" "$2"
}

proto_rdnss_teardown() {
	proto_lan_teardown $@
}


[ -n "$INCLUDE_ONLY" ] || {
	add_protocol dhcp6s
	add_protocol slaac 
	add_protocol rdnss
}

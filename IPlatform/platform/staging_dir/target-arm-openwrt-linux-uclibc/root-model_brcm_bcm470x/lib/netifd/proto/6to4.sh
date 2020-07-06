#!/bin/sh
# 6to4.sh - IPv6-in-IPv4 tunnel backend
# Copyright (c) 2010-2012 OpenWrt.org

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. /lib/functions/network.sh
	. /usr/share/libubox/jshn.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

find_6to4_addr() {
	local ip4="$1"
	local oIFS="$IFS"; IFS="."; set -- $ip4; IFS="$oIFS"

	printf "2002:%02x%02x:%02x%02x::%02x%02x:%02x%02x\n" $1 $2 $3 $4 $1 $2 $3 $4
}

find_6to4_prefix() {
	local ip4="$1"
	local oIFS="$IFS"; IFS="."; set -- $ip4; IFS="$oIFS"

	printf "2002:%02x%02x:%02x%02x\n" $1 $2 $3 $4
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

test_6to4_rfc1918()
{
	local oIFS="$IFS"; IFS="."; set -- $1; IFS="$oIFS"
	[ $1 -eq  10 ] && return 0
	[ $1 -eq 192 ] && [ $2 -eq 168 ] && return 0
	[ $1 -eq 172 ] && [ $2 -ge  16 ] && [ $2 -le  31 ] && return 0

	# RFC 6598
	[ $1 -eq 100 ] && [ $2 -ge  64 ] && [ $2 -le 127 ] && return 0

	return 1
}
lanv6_setup(){
	 local prefix="$1"
	 local ip6addr macaddr ifaceid
	 config_load protocol
	 config_get brifname dhcpv6 lanif

	 get_ifname_macaddr $brifname macaddr

	 [ -z "$macaddr" ] && {
	 proto_notify_error "$iface" "GET_MACADDR_ERROR"
	 return
	 }

	 get_eui_64_id "$macaddr" ifaceid

	 [ -z "$ifaceid" ] && {
	 proto_notify_error "$iface" "GET_EUI-ID_ERROR"
	 return
	 }
	 local prefix6="${prefix}:1:"
	 ip6addr="${prefix6}${ifaceid}"

	 #proto_init_update "$brifname" 1
	 #proto_add_ipv6_address "$ip6addr" 64
	 ip -6 addr add "${ip6addr}/64" dev $brifname
}
proto_6to4_setup() {
	local cfg="$1"
	local iface="$2"
	local link="6to4-$cfg"

	local mtu ttl ipaddr 
	json_get_vars mtu ttl ipaddr

	( proto_add_host_dependency "$cfg" 0.0.0.0 )

	local wanif
	if ! network_find_wan wanif; then
		proto_notify_error "$cfg" "NO_WAN_LINK"
		proto_setup_failed "$cfg"
		return
	fi

	[ -z "$ipaddr" ] && {
		if ! network_get_ipaddr ipaddr "$wanif"; then
			proto_notify_error "$cfg" "NO_WAN_ADDRESS"
			return
		fi
	}

#	test_6to4_rfc1918 "$ipaddr" && {
#		proto_notify_error "$cfg" "INVALID_LOCAL_ADDRESS"
#		return
#	}

	# find our local prefix
	local prefix6=$(find_6to4_prefix "$ipaddr")
	local local6=$(find_6to4_addr "$ipaddr")

	proto_init_update "$link" 1
	proto_add_ipv6_address "$local6" 48
	proto_add_ipv6_route "::" 96 "::" 256
	proto_add_ipv6_route "::" 0 "::192.88.99.1"

	proto_add_tunnel
	json_add_string mode sit
	json_add_int mtu "${mtu:-1480}"
	json_add_int ttl "${ttl:-255}"
	json_add_string local "$ipaddr"
	proto_close_tunnel

	proto_send_update "$cfg"
	
	
	 config_load /etc/config/network
	 config_get lanif lanv6 ifname

	 [ -z "$lanif" ] && {
	 lanv6_setup "$prefix6"
	 }
	 echo "${prefix6}::" > "/tmp/6to4_prefix.info"

}

proto_6to4_teardown() {
	local cfg="$1"
	local link="6to4-$cfg"
	 rm /tmp/6to4_prefix.info
}

proto_6to4_init_config() {
	no_device=1
	available=1

	proto_config_add_string "ipaddr"
	proto_config_add_int "mtu"
	proto_config_add_int "ttl"
}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol 6to4
}

#!/bin/sh

. /lib/functions.sh
. ../netifd-proto.sh
init_proto "$@"

proto_dhcp_init_config() {
	proto_config_add_string "ipaddr"
	proto_config_add_string "netmask"
	proto_config_add_string "hostname"
	proto_config_add_string "clientid"
	proto_config_add_string "vendorid"
	proto_config_add_boolean "broadcast"
	proto_config_add_string "reqopts"
}

proto_dhcp_setup() {
	local config="$1"
	local iface="$2"
	local lastip=/var/run/udhcpc-$iface.ip

	local ipaddr hostname clientid vendorid broadcast reqopts
	json_get_vars ipaddr hostname clientid vendorid broadcast reqopts

	local opt dhcpopts
	for opt in $reqopts; do
		append dhcpopts "-O $opt"
	done

	[ "$broadcast" = 1 ] && broadcast="-B" || broadcast=
	[ -n "$clientid" ] && clientid="-x 0x3d:${clientid//:/}" || clientid="-C"

	# get the last ipaddr
	[ -f $lastip ] && ipaddr=$(cat $lastip)

	# disguise as Windows DHCP Client
	vendorid="MSFT 5.0"

	proto_export "INTERFACE=$config"
	proto_export "IFNAME=$iface"
	proto_run_command "$config" udhcpc \
		-p /var/run/udhcpc-$iface.pid \
		-s /lib/netifd/dhcp.script \
		-O 33 -O 121 -O 249 \
		-f -R -a -t 0 -i "$iface" \
		${ipaddr:+-r $ipaddr} \
		${hostname:+-H $hostname} \
		${vendorid:+-V "$vendorid"} \
		$broadcast $dhcpopts
}

proto_dhcp_teardown() {
	local interface="$1"
	local ifname=""
	
	config_load /etc/config/network
	config_get ifname wan ifname
	
#	[ -n "$ifname" ] && /sbin/dhcpc.sh release "$ifname"
	
	proto_kill_command "$interface" 15
}

add_protocol dhcp


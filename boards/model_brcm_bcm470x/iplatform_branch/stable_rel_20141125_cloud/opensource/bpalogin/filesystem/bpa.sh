#!/bin/sh

[ -x /usr/sbin/bpalogin ] || exit 0


[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

proto_bigpond_init_config() {
	proto_config_add_string "username"
	proto_config_add_string "password"
	proto_config_add_string "server"
	proto_config_add_string "domain"
	proto_config_add_string "hostname"
	proto_config_add_boolean "broadcast"
}

proto_bigpond_setup() {
	local config="$1"
	local iface="$2"

	local hostname clientid broadcast
	json_get_vars hostname clientid broadcast

	[ "$broadcast" = 0 ] && broadcast= || broadcast="-B"
	[ -n "$clientid" ] && clientid="-x 0x3d:${clientid//:/}" || clientid="-C"

	proto_export "INTERFACE=$config"
	proto_export "IFNAME=$iface"
	proto_run_command "$config" udhcpc \
		-p /var/run/udhcpc-$iface.pid \
		-s /lib/netifd/bpa.script \
		-f -R -t 0 -i "$iface" \
		${hostname:+-H $hostname} \
		$clientid $broadcast
}

proto_bigpond_teardown() {
	local interface="$1"
	local ifname="$2"

	case "$ERROR" in
		11|19)
			proto_notify_error "$interface" AUTH_FAILED
			proto_block_restart "$interface"
		;;
		2)
			proto_notify_error "$interface" INVALID_OPTIONS
			proto_block_restart "$interface"
		;;
	esac
	
	killall -q -15 bpalogin
	sleep 1
	fw s_del 4 f zone_wan ACCEPT 1 { "-p udp  -m udp --dport 5050" }
#	[ -n "$ifname" ] && /sbin/dhcpc.sh release "$ifname"	
	proto_kill_command  "$interface" 15
}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol bigpond
}

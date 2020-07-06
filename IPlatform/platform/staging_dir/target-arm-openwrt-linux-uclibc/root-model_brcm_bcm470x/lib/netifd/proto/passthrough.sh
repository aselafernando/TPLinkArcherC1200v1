#!/bin/sh
. /usr/share/libubox/jshn.sh
[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

proto_passthrough_init_config() {
	no_device=1
	available=1
}

proto_passthrough_setup() {
        echo "##################################################" > /dev/console
	json_init
	json_load "`ubus call network.interface.wan status`"
	json_get_var device device
	insmod ipv6-pass-through wan_eth_name=$device lan_br_name=br-lan lan_eth_name=eth0.1
	/etc/init.d/dhcp6s stop
#	killall dhcp6c
	/etc/init.d/radvd stop
}

proto_passthrough_teardown() {
	local interface="$1"
	local ifname=""
	
	rmmod  ipv6-pass-through
}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol passthrough
}

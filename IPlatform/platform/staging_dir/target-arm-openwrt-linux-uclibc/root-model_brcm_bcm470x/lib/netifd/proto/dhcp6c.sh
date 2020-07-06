#!/bin/sh

[ -x /usr/sbin/dhcp6c ] || exit 0

dhcp6cdir="/tmp/dhcp6c"
conffile="$dhcp6cdir/dhcp6c.conf"
dhcp6cpid="$dhcp6cdir/dhcp6c.pid"

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

proto_dhcp6c_init_config() {
	proto_config_add_string "dns"
	proto_config_add_string "ip_mode"
	proto_config_add_string "ip_config"
	proto_config_add_string "ifname"
	proto_config_add_string "dns_mode"
	proto_config_add_string "lanif"
}

proto_dhcp6c_create_conffile(){
	local ip_mode="$1"
	local dns_mode="$2"
	local ifname="$3"
	local lanif="$4"
	local ip_config="$5"
	#Use the fixed iaid for ComCast IPv6
	#local id=`date +%s`
	#id=`expr $id % 1000000`
	#local id2=`expr $id + 1`
	local id=1
	local id2=$id

	rm -rf ${dhcp6cdir}
	mkdir -p ${dhcp6cdir}
	

	[ "$ip_mode" == "prefix" ] && {
		echo "interface $ifname {" > "${conffile}" 
		[ "$ip_config" == "dhcpv6" ] && echo -e "\tsend ia-na $id;" >> "${conffile}"
		echo -e "\tsend ia-pd $id2;" >> "${conffile}"

		[ "$dns_mode" == "dynamic" ] && echo -e "\trequest domain-name-servers;" >> "${conffile}"
		echo -e "\tscript \"/lib/netifd/dhcp6c.script\";" >> "${conffile}"

		echo -e "};\n" >>  "${conffile}"

		echo -e "id-assoc na $id {};\n" >> "${conffile}"

		echo "id-assoc pd $id2 {" >> "${conffile}"
		echo -e "\tprefix-interface $lanif {" >> "${conffile}"
		echo -e "\t\tsla-id 1;" >> "${conffile}"
		echo -e "\t};" >>  "${conffile}"
		echo "};" >>  "${conffile}"
	}

	[ "$ip_mode" == "non_temp" ] && {
		echo "interface $ifname {" > "${conffile}"
		[ "$ip_config" == "dhcpv6" ] && echo -e "\tsend ia-na $id;" >> "${conffile}"

		[ "$dns_mode" == "dynamic" ] && echo -e "\trequest domain-name-servers;" >> "${conffile}"
		echo -e "\tscript \"/lib/netifd/dhcp6c.script\";" >> "${conffile}"

		echo "};" >>  "${conffile}"

		echo "id-assoc na $id {};" >> "${conffile}"
	}
}

proto_dhcp6c_setup() {
	local config="$1"
	local iface="$2"
	local count=10
	local mbit=1

	json_get_vars dns_mode ip_mode ifname lanif ip_config

	echo "2"  > /proc/sys/net/ipv6/conf/$ifname/accept_ra
	[ "$ip_config" == "slaac" ] && {
 
		#echo '-1' > /proc/ndisc_mbit   # reset the mbit value to -1

		# Wait for RA
		while [ $count -ne 0 -a $mbit -eq -1 ]; do
			sleep 1
			mbit=`cat /proc/ndisc_mbit`
			let "count=count - 1"
		done

		[ $mbit -nq 0 ] && {
			sleep 3
			proto_setup_failed "$config"
			return
		}
	}
	
	proto_dhcp6c_create_conffile "$ip_mode" \
		"$dns_mode" "$ifname" "$lanif" "$ip_config"

	proto_run_command "$config" /usr/sbin/dhcp6c -f \
		-p "$dhcp6cpid" \
		-c "$conffile" \
		-I "$ifname" \
		-t "$config" \
		"$ifname"
}

proto_dhcp6c_teardown() {
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

	[ -n "$ifname" ] && {
		echo "1" > /proc/sys/net/ipv6/conf/$ifname/accept_ra
		ip -6 route del ::/0 dev "$ifname"
	}
	
	proto_kill_command  "$interface" 15
	sleep 1
	rm -rf ${dhcp6cdir}

}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol dhcp6c
}

#!/bin/sh

[ -x /usr/sbin/xl2tpd ] || exit 0

conffile="/tmp/l2tp/l2tpd.conf"
controlfile="/tmp/l2tp/l2tp-control"
l2tpdir="/tmp/l2tp"

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

escaped_str()
{
	word=$1
	word=${word//\\/\\\\}
	echo ${word//\"/\\\"}
}

proto_l2tp_init_config() {
	proto_config_add_string "username"
	proto_config_add_string "password"
	proto_config_add_string "keepalive"
	proto_config_add_string "pppd_options"
	proto_config_add_boolean "ipv6"
	proto_config_add_int "mru"
	proto_config_add_int "demand"
	proto_config_add_string "server"
	available=1
	no_device=1
}

proto_l2tp_setup() {
	local config="$1"
	local iface="$2"
	local parent=""
	local optfile="/tmp/l2tp/options.${config}"

	local server
	json_get_var server server

	json_get_var server server && {
		for ip in $(resolveip -4 -t 5 "$server"); do
			( proto_add_host_dependency "$config" "$ip" )
			echo "$ip" >> /tmp/server.l2tp-${config}
			serv_addr=1
		done
	}

	[ -n "$serv_addr" ] || {
		echo "Could not resolve server address"
		sleep 5
		proto_setup_failed "$config"
		exit 1
	}

	config_load network
	config_get parent $config parent
	config_get iface $parent ifname

#	if [ ! -p /var/run/xl2tpd/l2tp-control ]; then
#		/etc/init.d/xl2tpd start
#	fi

	json_get_vars ipv6 demand keepalive username password pppd_options
	[ "$ipv6" = 1 ] || ipv6=""
#	if [ "${demand:-0}" -gt 0 ]; then
#		demand="precompiled-active-filter /etc/ppp/filter demand idle $demand"
#	else
		demand="persist"
#	fi

	[ -n "$mru" ] || json_get_var mru mru
	
#	local interval="${keepalive##*[, ]}"
#	[ "$interval" != "$keepalive" ] || interval=5

	rm -rf ${l2tpdir} 
	mkdir -p ${l2tpdir}

	: > "${optfile}"
#	echo "${keepalive:+lcp-echo-interval $interval lcp-echo-failure ${keepalive%%[, ]*}}" > "${optfile}"
#	echo "ipcp-accept-local" >>"${optfile}"
#	echo "ipcp-accept-remote" >>"${optfile}"
#	echo "refuse-pap" >>"${optfile}"
#	echo "refuse-chap" >>"${optfile}"
	echo "refuse-eap" >>"${optfile}"
#	echo "require-mschap-v2" >>"${optfile}"
#	echo "noccp" >> "${optfile}"
#	echo "noauth" >> "${optfile}"
#	echo "crtscts" >> "${optfile}"
	echo "nopcomp" >> "${optfile}"
	echo "noaccomp" >> "${optfile}"
	echo "nobsdcomp" >> "${optfile}"
	echo "nodeflate" >> "${optfile}"
	echo "usepeerdns" >> "${optfile}"
	echo "defaultroute" >> "${optfile}"
	echo "default-asyncmap" >> "${optfile}"
	echo "${username:+user \"$(escaped_str "$username")\" password \"$(escaped_str "$password")\"}" >> "${optfile}"
	echo "ipparam \"$config\"" >> "${optfile}"
	echo "ifname \"l2tp-$config\"" >> "${optfile}"
	echo "ip-up-script /lib/netifd/ppp-up" >> "${optfile}"
	echo "ipv6-up-script /lib/netifd/ppp-up" >> "${optfile}"
	echo "ip-down-script /lib/netifd/ppp-down" >> "${optfile}"
	echo "ipv6-down-script /lib/netifd/ppp-down" >> "${optfile}"
	# Don't wait for LCP term responses; exit immediately when killed.
#	echo "lcp-max-terminate 0" >> "${optfile}"
#	echo "${ipv6:++ipv6} ${pppd_options}" >> "${optfile}"
#	echo "${mtu:+mtu $mtu mru $mtu}" >> "${optfile}"
	echo "${mru:+mtu $mru mru $mru}" >> "${optfile}"
	echo "${iface:+nic-ifname $iface}" >> "${optfile}"

#	xl2tpd-control add l2tp-${config} pppoptfile=${optfile} lns=${server} redial=yes redial timeout=20
#	xl2tpd-control connect l2tp-${config}

	echo "[global]" >  "${conffile}"
	echo "port = 1701" >>  "${conffile}"
	echo "[lac TP_L2TP]" >>  "${conffile}"
	echo "lns = $server" >>  "${conffile}"
	echo "pppoptfile = $optfile" >>  "${conffile}"

	proto_run_command "$config" /usr/sbin/xl2tpd -D \
		-C $controlfile \
		-c $conffile

	sleep 2
	echo "c TP_L2TP" > "${controlfile}" 
}

proto_l2tp_teardown() {
	local interface="$1"
	local optfile="/tmp/l2tp/options.${interface}"

	rm -f /tmp/server.l2tp-${interface}
	echo "no_server" > /tmp/connecterror
	case "$ERROR" in
		1)
			echo "auth_failed" > /tmp/connecterror
		;;
		11|19)
			proto_notify_error "$interface" AUTH_FAILED
			proto_block_restart "$interface"
		;;
		2)
			proto_notify_error "$interface" INVALID_OPTIONS
			proto_block_restart "$interface"
		;;
	esac
	
	proto_kill_command "$interface" 15
	sleep 1
	rm -rf ${l2tpdir}

#	xl2tpd-control disconnect l2tp-${interface}
	# Wait for interface to go down
#       while [ -d /sys/class/net/l2tp-${interface} ]; do
#		sleep 1
#	done

#	xl2tpd-control remove l2tp-${interface}
	rm -f ${optfile}
}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol l2tp
}

# Copyright(c) 2011-2014 Shenzhen TP-LINK Technologies Co.Ltd.

. /lib/config/uci.sh

transform_for_space()
{
	echo \"$@\"
}

get_ifname()
{
	local interface="$1"
	
	. /usr/share/libubox/jshn.sh
	json_init
	json_load "`ubus call network.interface.$interface status`"
	json_get_var device device
	
	echo $device
}

start_print_server() {
	config_load usbshare
	config_get printer "global" printer "on"

	local lanif="$(get_ifname lan)"
	local model="$(uci get system.@system[0].hostname)"
	
	if [ "$printer" = "on" ]
	then
		insmod GPL_NetUSB

		model=$(transform_for_space "$model")
		insmod NetUSB bndev="$lanif"
	fi
}

stop_print_server() {
	config_load usbshare
	config_get printer global printer "off"

	if [ "$printer" = "off" ]
	then
		rmmod GPL_NetUSB
		rmmod NetUSB
	fi
}


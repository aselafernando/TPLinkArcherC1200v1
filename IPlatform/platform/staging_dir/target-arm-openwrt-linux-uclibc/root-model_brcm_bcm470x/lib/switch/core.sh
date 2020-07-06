# Copyright (C) 2009-2010 OpenWrt.org

. /lib/switch/config.sh
. /lib/switch/core_phy.sh
. /lib/config/uci.sh

fw_config_get_mac() {
    fw_config_get_section "$1" device { \
        string macaddr "" \
        string name    "" \
    } || return
}

find_mac() {
    fw_config_get_mac $1
    ifname=$(uci get network.lan.ifname)
    if [[ "$device_name" == "$ifname" ]]; then
        ssdk_sh fdb entry add "${device_macaddr//:/-}" 65535 forward forward 6 yes no no no no no no
    fi
}

setup_vlan() {
	config_load switch
	config_foreach setup_one_vlan switch_vlan
	log "SETUP vlan settings success!"
    return
    # todo: rewrite it with register
    #ssdk_sh vlan member add 1 2 untagged
    #ssdk_sh vlan member add 1 3 untagged
    #ssdk_sh vlan member add 1 4 untagged
    #ssdk_sh portVlan member add 1 2
    #ssdk_sh portVlan member add 1 3
    #ssdk_sh portVlan member add 1 4
    #ssdk_sh vlan fid set 1 65535

    #ssdk_sh vlan member add 2 1 untagged
    #ssdk_sh vlan member add 2 3 untagged
    #ssdk_sh vlan member add 2 4 untagged
    #ssdk_sh portVlan member add 2 1
    #ssdk_sh portVlan member add 2 3
    #ssdk_sh portVlan member add 2 4
    #ssdk_sh vlan fid set 2 65535

    #ssdk_sh vlan member add 3 1 untagged
    #ssdk_sh vlan member add 3 2 untagged
    #ssdk_sh vlan member add 3 4 untagged
    #ssdk_sh portVlan member add 3 1
    #ssdk_sh portVlan member add 3 2
    #ssdk_sh portVlan member add 3 4
    #ssdk_sh vlan fid set 3 65535

    #ssdk_sh vlan member add 4 1 untagged
    #ssdk_sh vlan member add 4 2 untagged
    #ssdk_sh vlan member add 4 3 untagged
    #ssdk_sh portVlan member add 4 1
    #ssdk_sh portVlan member add 4 2
    #ssdk_sh portVlan member add 4 3
    #ssdk_sh vlan fid set 4 65535

    #ssdk_sh fdb portLearn set 6 disable
    #ssdk_sh fdb entry flush 1
    #ssdk_sh portVlan ingress set 6 fallback
    #ssdk_sh portVlan defaultCVid set 6 1
    #config_load network

    #config_foreach find_mac device
}

unsetup_vlan() {
	clear_all_vlans
	log "CLEAN vlan settings success!"
	return
}

config_get_switch_vlan () {
	fw_config_get_section "$1" switch_vlan { \
		    string ports		''	\
			string device		''	\
			string vlan			''	\
	} || return
}

setup_one_vlan() {
	config_get_switch_vlan $1

	setup_switch_vlan $switch_vlan_vlan $switch_vlan_ports
	#[ -z "$(echo $switch_vlan_ports | grep 'u' )" ] && vconfig add eth0 $switch_vlan_vlan  
}

setup_ports() {
	link_up_all_ports
}

unsetup_ports() {
	link_down_all_ports
}

setup_duplex() {
	local port=$(uci get portspeed.wan.port)
	local speed=$(uci get portspeed.wan.current)
	local autoneg
	local duplex

	case $speed in
		"auto")
			autoneg="on"
			speed="1000"
			duplex="full"
			;;
		"10H")
			autoneg="off"
			speed="10"
			duplex="half"
			;;
		"10F")
			autoneg="off"
			speed="10"
			duplex="full"
			;;
		"100H")
			autoneg="off"
			speed="100"
			duplex="half"
			;;
		"100F")
			autoneg="off"
			speed="100"
			duplex="full"
			;;
		"1000H")
			autoneg="off"
			speed="1000"
			duplex="half"
			;;
		"1000F")
			autoneg="off"
			speed="1000"
			duplex="full"
			;;
	esac
	
	set_port_duplex $port $speed $duplex $autoneg
	log "SETUP port ($port) duplex: $speed $half!"
}

unsetup_duplex() {
	log "UNSETUP duplex"
}

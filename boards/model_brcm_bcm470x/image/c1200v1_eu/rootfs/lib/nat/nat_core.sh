# Copyight(c) 2011-2013 Shenzhen TP-LINK Technologies Co.Ltd.
# file     nat_core.sh
# bief    
# autho   Guo Dongxian
# vesion  1.0.0
# date     26Feb14
# history   arg 1.0.0, 26Feb14, Guo Dongxian, Ceate the file

NAT_LIBDIR=${NAT_LIBDIR:-/lib/nat}

. /lib/functions.sh
. /lib/functions/network.sh
include /lib/network

nat_main() {
      
    lock /var/run/nat-interface.lock 
    
    local which=$1

    nat_stop
    nat_init

    nat_fw_is_unloaded && {
        echo "Firewall unloaded" >&2
        lock -u /var/run/nat-interface.lock
		exit 1
    }
    
    uci_revert_state nat
    uci_set_state nat core "" nat_state
	uci_set_state nat env "" nat_env_state 
	
    case $which in
        nat)
            shift 
            nat_run "$1"
        ;;
        *)
            nat_run "$@"
        ;;
    esac

	uci_set_state nat core wan_zones "$NAT_WAN_ZONES"
	uci_set_state nat core loaded 1

    lock -u /var/run/nat-interface.lock

    return
}

nat_event() {
    nat_main "$@"
}

nat_start() {
	nat_main nat "$1"
}

nat_stop() {
    nat_init

 	config_clear
    nat_clear
    uci_revert_state nat
    unset NAT_INITIALIZED
}

nat_restart() {
    # Ugly stop operation in start operation, just for doing locked?
	nat_start $1
}

nat_reload() {
	nat_restart $1
}

nat_is_loaded() {
	local bool=$(uci_get_state nat.core.loaded)
	return $((! ${bool:-0}))
}

nat_is_enable() {
    local bool=$(uci_get_state nat.core.enabled)
	return $((! ${bool:-0}))
}

nat_fw_is_unloaded() {
	local bool=$(uci_get_state firewall.core.loaded)
	return $((${bool:-0}))
}

nat_init() {
	[ -z "$NAT_INITIALIZED" ] || return 0

	. $NAT_LIBDIR/config.sh
    . $NAT_LIBDIR/nat_log.sh

	scan_interfaces

	nat_config_append nat
    nat_config_append administration
   
    # optimize startup time
    #nat_config_append network
    nat_config_append usbshare

	local f n

	for f in $NAT_LIBDIR/nat_*.sh; do
		n=$(basename $f .sh)
		if [ "$n" != "nat_core" -a "$n" != "nat_log" ]; then
			. $f
		fi
	done

	NAT_INITIALIZED=1
    
    nat_syslog 501

	return 0
}



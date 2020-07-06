# Copyright(c) 2011-2013 Shenzhen TP-LINK Technologies Co.Ltd.
# file     nat_var.sh
# brief    
# author   Guo Dongxian
# version  1.0.0
# date     26Feb14
# history   arg 1.0.0, 26Feb14, Guo Dongxian, Create the file

NAT_ENV_VAR_APPLIED=
NAT_ENV_IFACES=

nat_net_get_proto() { __network_device "$1" "$2" proto; }

nat_generate_if_env() {
	local iface=$1
	local action=$2
	local dev=$3
	local ifname=$4
	
	[ "$action" == "add" ] && {
		local status=$(uci_get_state network "$iface" up 0)
		[ "$status" == 1 ] || return 0
	}

	[ -n "$ifname" ] || {
		ifname=$(uci_get_state network "$iface" ifname)
		ifname="${ifname%%:*}"
		[ -z "$ifname" ] && {
			[ -z "$dev" ] && {
				network_get_device dev "$iface"
				[ -z "$dev" ] && return 0
				ifname=${dev}
			}
		}
	}

	if [ -z "$dev" ]; then
		network_get_device dev "$iface"
		[ -z $dev ] && return 0
	fi

	[ "$ifname" == "lo" ] && {
		local lo_set=$(uci_get_state nat env lo_if)
		[ -z "$lo_set" ] && {
			uci_set_state nat env lo_if "$ifname"
			uci_set_state nat env lo_ip "127.0.0.1"
			return 0
		}
		return 0
	}

    #local old_ifs=$(uci_get_state nat core ifaces)
	#list_contains old_ifs $iface || {
		#append NAT_ENV_IFACES $iface
        #local old_ifs=$(uci_get_state nat core ifaces)
        
        #if [ -z "$old_ifs" ]; then
            #append old_ifs $iface
            #uci_set_state nat core ifaces "$old_ifs"
        #else
            #append old_ifs $iface
            #uci_toggle_state nat core ifaces "$old_ifs"
        #fi
	#}
	
	nat__gen_if_info() {
		local iface=$1
		local action=$2
		local dev=$3
		local ifname=$4

		case ${action} in
			add)
				local act ip sec_ip mask proto
				local s=$(uci_get_state nat env ${iface}_if)
				if [ -z "$s" ]; then
					act=set
				else
					act=toggle
				fi

				network_get_ipaddr ip "$iface"
				network_get_subnet mask "$iface"
				[ -z "$ip" ] && {
					ip="0.0.0.0"
				}
				mask="${mask#*/}"
				
				nat_net_get_proto proto "$iface"
				case $proto in
					pppoe|pptp|l2tp)
						#network_get_ipaddr sec_ip internet 2>/dev/null
                        sec_ip="-"
						[ -z "$sec_ip" ] && {
							sec_ip="0.0.0.0"
						}
					;;
					*) sec_ip="-" ;;
				esac
						
				uci_${act}_state nat env ${iface}_if "$ifname"
				uci_${act}_state nat env ${iface}_dev "$dev"
				uci_${act}_state nat env ${iface}_ip "$ip"
				uci_${act}_state nat env ${iface}_mask "$mask"
				uci_${act}_state nat env ${iface}_sec_ip "$sec_ip"
			;;
			del)
				uci_revert_state nat env "${iface}_if"
				uci_revert_state nat env "${iface}_dev"
				uci_revert_state nat env "${iface}_ip"
				uci_revert_state nat env "${iface}_mask"
				uci_revert_state nat env "${iface}_sec_ip"
			;;
		esac
	}

	nat__gen_if_info $iface $action $dev $ifname
}

nat_generate_normal_env() {
	#TODO
	echo "Normal env generate"
}

nat_generate_env() {
	local iface=$2
	local action=$3
	local dev=$4
	
	case $action in
		ifup|ifdown)
			echo "Network event, up or down" >&2
            # However, up or down we always add each interface.
            # It maybe be changed in the further            
            config_foreach nat_generate_if_env interface add
		;;
		*) # Config operation
		    nat_generate_normal_env
			config_foreach nat_generate_if_env interface add	
		;;
	esac
}


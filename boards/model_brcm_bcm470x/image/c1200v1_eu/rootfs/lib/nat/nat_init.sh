# Copyright(c) 2011-2013 Shenzhen TP-LINK Technologies Co.Ltd.
# file     nat_core.sh
# brief    
# author   Guo Dongxian
# version  1.0.0
# date     26Feb14
# history   arg 1.0.0, 26Feb14, Guo Dongxian, Create the file
NAT_LIBDIR=${NAT_LIBDIR:-/lib/nat}

NAT_INITIALIZED=
NAT_GLOABL_APPLIED=

NAT_SWITCH_ENABLED=
NAT_SWITCH_HW_ENABLED=
NAT_ZONES=
NAT_WAN_ZONES=

FW_FORWARD_PREFIX=forwarding
FW_PREROUTING_PREFIX=prerouting

NAT_F_DMZ_CHAIN=dmz
NAT_F_VS_CHAIN=vs
NAT_F_PT_CHAIN=pt
NAT_F_VPN_PASS_CHAIN=vpn

NAT_N_FTP_CHAIN=ftp
NAT_N_VS_CHAIN=vs
NAT_N_PT_CHAIN=pt
NAT_N_DMZ_CHAIN=dmz

NAT_ZONE_LAN=lan

nat_chain_flush() {
    local tbl=$1
    local chain=$2
    
    case $tbl in
        nat|n) fw flush 4 n $chain ;;
        *) fw flush 4 f $chain ;;
    esac 
}

nat_chain_clear() {
    local tbl=$1
    local chain=$2
    
    local parent=${chain%_*}
    case $tbl in
        nat|n)
            fw flush 4 n $chain
            fw del 4 n $parent $chain
            fw del 4 n $chain
        ;;
        *)
            fw flush 4 f $chain
            fw del 4 f $parent $chain
            fw del 4 f $chain
        ;;
    esac
}

nat_chain_destroy() {
    [ -n "$nat_filter_chains" ] && {
        for f in $nat_filter_chains; do
            nat_chain_clear filter $f
        done
    }

    [ -n "$nat_norder" -a -n "$nat_rule_chains" -a "$nat_dmz_chains" ] && {
        for n in $nat_norder; do
            nat_chain_clear nat ${nat_rule_chains}_${n}        
        done

        for dmz in $nat_dmz_chains; do
            nat_chain_clear nat $dmz
        done
    }

    nat_chain_flush nat zone_wan_nat
    nat_chain_flush nat zone_lan_nat

    # Destroy some main chains, which create by NAT itself
    fw del 4 f zone_lan_forward zone_lan_natfd
    fw del 4 f zone_lan_natfd

    fw del 4 f zone_wan_forward zone_wan_natfd
    fw del 4 f zone_wan_natfd  

    nat_syslog 52
}

nat_chain_create() {
    # Create some main chains, which firewall not create when init
    fw add 4 f zone_lan_natfd
    fw add 4 f zone_lan_forward zone_lan_natfd ^
    
    fw add 4 f zone_wan_natfd
    fw add 4 f zone_wan_forward zone_wan_natfd ^

    [ -n "$nat_filter_chains" ] && {
        for fc in $nat_filter_chains; do
            local p_fc=${fc%_*}            
            fw add 4 f $fc
            fw add 4 f $p_fc $fc 
        done
    }
    
    [ -n "$nat_norder" -a -n "$nat_rule_chains" -a "$nat_dmz_chains" ] && {
        for nc in $nat_norder; do
            local chain=${nat_rule_chains}_${nc}
            fw add 4 n $chain
            fw add 4 n $nat_rule_chains $chain
        done

        for dmz in $nat_dmz_chains; do
            local p_dmz=${dmz%_*}
            fw add 4 n $dmz
            fw add 4 n $p_dmz $dmz
        done
    }

    nat_syslog 51
}

nat_chain_exist() {
    local f=/var/nat.switch
    
    [ -e $f ] && {
        local en=$(cat $f)
        [ "$en" == "1" ] && return 0
    }
    
    return 1
}

nat_masq() {
    for z in ${NAT_WAN_ZONES:-"wan"}; do
        local mt=$(fw list 4 n|grep -E "zone_${z}_nat \-j MASQUERADE")
        [ -z "$mt" ] && fw add 4 n zone_${z}_nat MASQUERADE
    done
}

nat_config_load_global() {
	nat_config_get_section "$1" global { \
		string enable on \
		string hw_enable off \
	} || return

	NAT_SWITCH_ENABLED=$global_enable
        NAT_SWITCH_HW_ENABLED=$global_hw_enable
    
    ! nat_chain_exist && {
        # Create all nat forward chains in order
        nat_chain_create

        # Do snat flow
        nat_masq

        echo 1 >/var/nat.switch 2>/dev/null
        nat_syslog 502  

        # Hotplug call modules which needs nat
        env -i NAT_ACT=on /sbin/hotplug-call nat
    }	

	[ "$NAT_SWITCH_ENABLED" == "off" ] && nat_chain_exist && {
        # Clear all nat forward chains
		nat_chain_destroy

        local en=$(uci_get_state nat core enabled)
        if [ -z "$en" ]; then
            uci_set_state nat core enabled 0
        else
            uci_toggle_state nat core enabled 0
        fi

        echo 0 >/var/nat.switch 2>/dev/null
        nat_syslog 503
	}
}

nat_env_parse_done() {
	local ifaces=$(uci_get_state nat core ifaces)

	[ -z "$ifaces" ] && return 1
	
	for i in $ifaces; do
        local ip=
		[ "$i" == "$NAT_ZONE_LAN" ] && continue
	    network_get_ipaddr ip "$i"
        [ -n "$ip" -a "$ip" != "0.0.0.0" ] && return 0	
	done
    
    return 1
}

nat_clear() {
    #lock -u /var/run/nat-interface.lock
    return
}

nat_loopback() {
    local ifaces=$(uci_get_state nat core ifaces)

    for i in ${ifaces:-"lan"}; do
        [ "$i" = "$NAT_ZONE_LAN" ] && continue
        nat_config_nw_exist $z $i && {
	        local lan_ip=$(uci_get_state nat env ${NAT_ZONE_LAN}_ip)
	        local lan_mask=$(uci_get_state nat env ${NAT_ZONE_LAN}_mask)

	        local wan_ip=$(uci_get_state nat env ${i}_ip)
            
            # NAT Loopback function
            [ -n "$lan_ip" -a -n "$lan_mask" -a -n "$wan_ip" ] && {
                fw flush 4 n zone_${NAT_ZONE_LAN}_nat
                fw add 4 n zone_${NAT_ZONE_LAN}_nat SNAT { -s ${lan_ip}/${lan_mask} \
                    -d ${lan_ip}/${lan_mask} --to-source ${wan_ip} }
            }
        }
    done
}

nat_config_rule_operation() {
	
	nat_generate_env "$@"

    [ "$1" == "network" ] && {
        sleep 1
    }

	if nat_env_parse_done
    then
        # POSTROUTING
		nat_loopback
		case $2 in
			vs)
				echo "Loading virsual server"
				nat_rule_vs_operation
			;;
			pt)
				echo "Loading port trigger"
				nat_rule_pt_operation
			;;
			dmz)
				echo "Loading nat dmz"
				nat_dmz_operation
			;;
            alg)
                echo "Loading nat alg"
                nat_alg_run
            ;;
			*) 
                # After creating all nat chain in order, then init nat rules of remote mngt, dnsproxy, etc
                # Initializing from config file
                nat_config_init_mods_rules
                # Forwarding rule create from config file
				nat_rule_vs_operation
				nat_rule_pt_operation
				nat_dmz_operation
                nat_alg_run
			;;
		esac
    else
        echo "Flush nat forwarding chain"     
        [ -n "$nat_filter_chains" ] && {
            for fc in $nat_filter_chains; do
               nat_chain_flush filter $fc
            done
        }

        [ -n "$nat_dmz_chains" ] && {
            for dmz in $nat_dmz_chains; do
                nat_chain_flush nat $dmz
            done
        } 
        
        nat_chain_flush nat ${nat_rule_chains}_vs
        nat_chain_flush nat ${nat_rule_chains}_pt
    fi    
	return
}

nat_run() {
    # Read default section to init important variables
    nat_config_default
    # Read global and create/del nat chains
    nat_config_once nat_config_load_global nat_global

    [ -n "$NAT_SWITCH_ENABLED" -a "$NAT_SWITCH_ENABLED" == "on" ] && {
        local en=$(uci_get_state nat core enabled)
        if [ -z "$en" ]; then
            uci_set_state nat core enabled 1
        else
            uci_toggle_state nat core enabled 1
        fi

        if [ $# -eq 1 ]; then
            nat_config_rule_operation nat $1
        else
            nat_config_rule_operation "$@"
        fi
    }

    echo f >/proc/net/nf_conntrack 2>/dev/null
    nat_syslog 1

    [ -n "$NAT_SWITCH_HW_ENABLED" ] && {
        local hw_enable
        if [ "$NAT_SWITCH_HW_ENABLED" = "on" ];then
             hw_enable=1
        else
             hw_enable=0
        fi
        
        local hw_file="/tmp/nat.hw_switch"
        local hw_op=""
        local hw_status=""
            
        if [ -f "$hw_file" ];then
            hw_status=$(cat "$hw_file")
        fi            

        if [ -z "$hw_status" ]; then
            hw_op="init"
        else
            hw_op="reset"
        fi 

        if [ -z "$hw_status" -o "$hw_enable" != "$hw_status" ]; then
            echo "$hw_enable" > "$hw_file"
            [ -f $NAT_LIBDIR/nat_hw.sh ] && {
                . $NAT_LIBDIR/nat_hw.sh
                nat_hw_enable "$hw_enable" "$hw_op"
            }
        fi                
    }
}


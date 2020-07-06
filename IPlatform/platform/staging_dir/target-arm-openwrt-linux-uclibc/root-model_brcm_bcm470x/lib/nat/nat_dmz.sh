# Copyright(c) 2011-2013 Shenzhen TP-LINK Technologies Co.Ltd.
# file     nat_dmz.sh
# brief    
# author   Guo Dongxian
# version  1.0.0
# date     26Feb14
# history   arg 1.0.0, 26Feb14, Guo Dongxian, Create the file

NAT_DMZ_FW_ISSET=
DMZ_FILTER_CHAINS=

nat_config_dmz() {
	nat_config_get_section "$1" nat_dmz { \
		string name "" \
		string enable "" \
        string ipaddr "" \
		string interface "" \
	} || return
}

nat_load_dmz() {
    nat_config_dmz "$1"
    
    nat__do_dmz() {
        local ifname=$1
        local lan_ip=$2
        local wan_ip=$3    
        local iface=$4
        
        [ -z $NAT_DMZ_FW_ISSET ] && {
            for f_dmz in $DMZ_FILTER_CHAINS; do
                fw add 4 f ${f_dmz} ACCEPT $ \
                    { -d ${lan_ip} -m conntrack --ctstate DNAT }
            done
            NAT_DMZ_FW_ISSET=1
        }
        
        for n_dmz in $nat_dmz_chains; do
            fw add 4 n ${n_dmz} ACCEPT ^ \
                { -d ${wan_ip} -p icmp -m icmp --icmp-type 8 }

            fw add 4 n ${n_dmz} DNAT $ \
                { -d ${wan_ip} --to-destination ${lan_ip} }
        done
    }
    
    [ -n "$nat_dmz_interface" ] && {
        #TODO
        echo "mulit wan not support now"
        return 0
    }

    [ -z "$nat_dmz_ipaddr" -o "$nat_dmz_ipaddr" == "0.0.0.0" ] && {
        echo "host ip address is null."
        return 1
    }

    local lan_addr=$(uci_get_state nat env ${NAT_ZONE_LAN}_ip)
    local lan_mask=$(uci_get_state nat env ${NAT_ZONE_LAN}_mask)
    local same_net=$(lua /lib/nat/nat_tools.lua $nat_dmz_ipaddr $lan_addr $lan_mask)
    
    [ -z "$same_net" -o "$same_net" = "false" ] && {
        nat_syslog 81 "$nat_dmz_ipaddr"
        echo "The ip address of dmz is not in the lan subnet"
        return 1
    }

    [ "$nat_dmz_enable" == "on" ] && {
        nat_syslog 23
        local ifaces=$(uci_get_state nat core ifaces)
        for zone in $NAT_WAN_ZONES; do
            for i in ${ifaces:-"lan"}; do
                nat_config_nw_exist $zone $i && {
                    local ifname=$(uci_get_state nat env ${i}_if)
                    local dev=$(uci_get_state nat env ${i}_dev)
                    
                    local wan_ip=$(uci_get_state nat env ${i}_ip)
                    local wan_sec_ip=$(uci_get_state nat env ${i}_sec_ip)

                    [ -n "$ifname" -a -n "$dev" ] && {
                        [ -n "$wan_ip" -a "$wan_ip" != "0.0.0.0" ] && {                            
                            nat__do_dmz $ifname $nat_dmz_ipaddr $wan_ip $zone                             
                        }      
                  
                        [ -n "$wan_sec_ip" -a "$wan_sec_ip" != "0.0.0.0" -a "$wan_sec_ip" != "-" ] && {               
                            nat__do_dmz $dev $nat_dmz_ipaddr $wan_sec_ip $zone 
                        }
                    }           
                }
            done
        done
        nat_syslog 53 "$nat_dmz_ipaddr"
    }

    [ "$nat_dmz_enable" == "off" ] && nat_syslog 24
}

nat_dmz_operation() {
    unset NAT_DMZ_FW_ISSET
    [ -n "$nat_filter_chains" ] && {
        for fc in $nat_filter_chains; do
            local dmz=$(echo "$fc"|grep 'dmz$')
            [ -n "$dmz" ] && {
                append DMZ_FILTER_CHAINS $fc
                fw flush 4 f $dmz
            }
        done
    }

    [ -n "$nat_dmz_chains" ] && {
        for d in $nat_dmz_chains; do
            fw flush 4 n $d        
        done   
    }
    
    config_foreach nat_load_dmz nat_dmz
    unset DMZ_FILTER_CHAINS
}


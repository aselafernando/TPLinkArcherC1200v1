# Copyright(c) 2011-2013 Shenzhen TP-LINK Technologies Co.Ltd.
# file     nat_pt.sh
# brief    
# author   Guo Dongxian
# version  1.0.0
# date     26Feb14
# history   arg 1.0.0, 26Feb14, Guo Dongxian, Create the file

NAT_PT_MAX_EX_PORT_GROUP=5
NAT_PT_ENABLE=

PT_FILTER_CHAINS=

nat_config_rule_pt() {
	nat_config_get_section "$1" rule_pt { \
		string name "" \
		string enable "" \
		string trigger_port "" \
		string trigger_protocol "" \
		string external_port "" \
		string external_protocol "" \
        string ipaddr "" \
		string interface "" \
	} || return
}

nat_load_rule_pt() {
    local tri_proto="all tcp udp"
    local pub_proto="all tcp udp"

    nat_config_rule_pt "$1"

    [ -n "$rule_pt_interface" ] && {
		#TODO MULTI WAN
		echo "not support multi wan now"
		return 0
	}

    [ -n "$rule_pt_ipaddr" ] && {
        #TODO 
        echo "not support assigned host address now" 
        return 0
    }

    [ -n "$rule_pt_trigger_protocol" ] && {
        rule_pt_trigger_protocol=$(echo $rule_pt_trigger_protocol|tr '[A-Z]' '[a-z]')
    }

    [ -n "$rule_pt_external_protocol" ] && {
        rule_pt_external_protocol=$(echo $rule_pt_external_protocol|tr '[A-Z]' '[a-z]')
    }

    #TODO check trigger port validity

    nat__do_pt_rule() {
        local tri_proto=$1
        local pub_proto=$2
        local ifname=$3
        local ip=$4        
    
        local count=0
        for port in $rule_pt_external_port; do
            count=$((count + 1))
            [ $count -gt $NAT_PT_MAX_EX_PORT_GROUP ] && {
                echo "External port cannot over 5 group"
                nat_syslog 101
                break 
            }

            local ex_port_start=${port%-*}
            local ex_port_end=${port#*-}

            nat_port $ex_port_start && nat_port $ex_port_end && [ -n "$rule_pt_trigger_port" ] &&
                nat_port $rule_pt_trigger_port && {            
                echo "${ex_port_start}-${ex_port_end}"
                [ ${ex_port_start} -gt ${ex_port_end} ] && {
                    local tmp_ex_port=$ex_port_start
                    ex_port_start=$ex_port_end
                    ex_port_end=$tmp_ex_port                
                }
                
                nat_syslog 2 "$port" "$rule_pt_trigger_port"
                for pt_chain in $PT_FILTER_CHAINS; do
                    fw add 4 f $pt_chain TRIGGER $ \
                        { -i ${ifname} -p ${tri_proto} --trigger-type out --trigger-proto ${pub_proto} \
                            --trigger-match ${rule_pt_trigger_port}-${rule_pt_trigger_port} \
                                --trigger-relate ${ex_port_start}-${ex_port_end} }
                done
                nat_syslog 55 "$rule_pt_trigger_port" "$tri_proto" "${ex_port_start}-${ex_port_end}" "$pub_proto"
                continue                            
                
            }

            return 1
        done
        
        return 0
    }

    [ -n "$rule_pt_enable" -a "$rule_pt_enable" == "on" ] && {
        local lan_if=$(uci_get_state nat env ${NAT_ZONE_LAN}_if)
        local ifaces=$(uci_get_state nat core ifaces)
        local wan_status

        for i in ${ifaces:-"lan"}; do
            if [ "$i" != "$NAT_ZONE_LAN" ]; then
                local if_name=$(uci_get_state nat env ${i}_if)
                local if_ip=$(uci_get_state nat env ${i}_ip)   
                [ -n "$if_name" -a -n "$if_ip" -a "$if_ip" != "0.0.0.0" ] && {
                    wan_status=1
                    break
                }             
            fi
        done
        
        [ -n "$lan_if" -a -n "$wan_status" ] && {
            list_contains tri_proto ${rule_pt_trigger_protocol} && {
                list_contains pub_proto ${rule_pt_external_protocol} && {
                    nat__do_pt_rule $rule_pt_trigger_protocol $rule_pt_external_protocol $lan_if && { 
                        NAT_PT_ENABLE=1
                    }     
                }     
            }
        }

        unset wan_status
    }
}

nat_rule_pt_operation() {
    [ -n "$nat_filter_chains" ] && {
        for fc in $nat_filter_chains; do
            local pt=$(echo "$fc"|grep 'pt$')
            [ -n "$pt" ] && {
                append PT_FILTER_CHAINS $fc
                fw flush 4 f $pt
            }
        done
    }
    fw flush 4 n ${nat_rule_chains}_${NAT_N_PT_CHAIN}

    config_foreach nat_load_rule_pt rule_pt
    
    [ -n "$NAT_PT_ENABLE" ] && {
        for pt_chain in $PT_FILTER_CHAINS; do
            fw add 4 f ${pt_chain} TRIGGER $ { --trigger-type in }
        done
        
        local ifaces=$(uci_get_state nat core ifaces)
        for i in ${ifaces:-"lan"}; do
            [ "$i" != "$NAT_ZONE_LAN" ] && {
                local ip=$(uci_get_state nat env ${i}_ip)
                local sec_ip=$(uci_get_state nat env ${i}_sec_ip)

                [ -n "$ip" -a "$ip" != "0.0.0.0" ] && {
                    fw add 4 n ${nat_rule_chains}_${NAT_N_PT_CHAIN} TRIGGER $ \
                        { -d ${ip} --trigger-type dnat }                
                }
                
                [ -n "$sec_ip" -a "$sec_ip" != "-" -a "$sec_ip" != "0.0.0.0" ] && { 
                    fw add 4 n ${nat_rule_chains}_${NAT_N_PT_CHAIN} TRIGGER $ \
                        { -d ${sec_ip} --trigger-type dnat }                                           
                }        
            }
        done
        unset NAT_PT_ENABLE     
    }
    unset PT_FILTER_CHAINS
}


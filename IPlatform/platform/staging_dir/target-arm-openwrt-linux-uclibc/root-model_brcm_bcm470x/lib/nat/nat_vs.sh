# Copyright(c) 2011-2013 Shenzhen TP-LINK Technologies Co.Ltd.
# file     nat_vs.sh
# brief    
# author   Guo Dongxian
# version  1.0.0
# date     26Feb14
# history   arg 1.0.0, 26Feb14, Guo Dongxian, Create the file

vs_filter_chains=
nat_config_rule_vs() {
	nat_config_get_section "$1" rule_vs { \
		string name "" \
		string enable "" \
		string external_port "" \
		string internal_port "" \
		string protocol "" \
		string ipaddr "" \
		string interface "" \
	} || return
}

nat_load_rule_vs() {
	local proto="all tcp udp"
	echo "nat_load_rule_vs in"
	nat_config_rule_vs "$1"

	[ -n "$rule_vs_interface" ] && {
		#TODO MULTI WAN
		echo "Not support multi wan now"
		return 0
	}

	[ -z "$rule_vs_ipaddr" -o "$rule_vs_ipaddr" == "0.0.0.0" ] && {
		echo "Host addr is not set"
		return 1
	}

    local lan_addr=$(uci_get_state nat env ${NAT_ZONE_LAN}_ip)
    local lan_mask=$(uci_get_state nat env ${NAT_ZONE_LAN}_mask)
    local same_net=$(lua /lib/nat/nat_tools.lua $rule_vs_ipaddr $lan_addr $lan_mask)
    
    [ -z "$same_net" -o "$same_net" = "false" ] && {
        nat_syslog 81 "$rule_vs_ipaddr"
        echo "The ip address of rule is not in the lan subnet"
        return 1
    }

    [ -n "$rule_vs_protocol" ] && {
        rule_vs_protocol=$(echo $rule_vs_protocol|tr '[A-Z]' '[a-z]')
    }

	nat__do_vs_rule() {
		local proto=$1
		local ifname=$2
		local ip=$3
	
        [ -z "$rule_vs_external_port" ] && return
              
		local ex_port_start=${rule_vs_external_port%-*}
		local ex_port_end=${rule_vs_external_port#*-}

        nat_port $ex_port_start && nat_port $ex_port_end && {
            local in_port_start
            local in_port_end

            [ $ex_port_start -gt $ex_port_end ] && {
                local tmp_ex_port=$ex_port_start
                ex_port_start=$ex_port_end
                ex_port_end=$tmp_ex_port
            }

            if [ -z "$rule_vs_internal_port" ]; then
                in_port_start=1
                in_port_end=65535
            else
		        in_port_start=${rule_vs_internal_port%-*}
		        in_port_end=${rule_vs_internal_port#*-}                
            fi
            
	        nat_port $in_port_start && nat_port $in_port_end && {
                [ $in_port_start -gt $in_port_end ] && {
                    local tmp_in_port=$in_port_start
                    in_port_start=$in_port_end
                    in_port_end=$tmp_in_port
                }

                if [ ${ex_port_start} -eq ${ex_port_end} ]; then
			        if [ ${in_port_start} -ne ${in_port_end} ]; then
                        fw add 4 n ${nat_rule_chains}_${NAT_N_VS_CHAIN} DNAT $ \
                            { -d ${ip} -p ${proto} --dport ${ex_port_start} --to-destination ${rule_vs_ipaddr} }
					else
                        fw add 4 n ${nat_rule_chains}_${NAT_N_VS_CHAIN} DNAT $ \
                            { -d ${ip} -p ${proto} --dport ${ex_port_start} \
                                --to-destination ${rule_vs_ipaddr}:${in_port_start} }					
			        fi
						
		        else
			        if [ ${in_port_start} -ne ${in_port_end} ]; then
                        fw add 4 n ${nat_rule_chains}_${NAT_N_VS_CHAIN} DNAT $ \
                            { -d ${ip} -p ${proto} --dport ${ex_port_start}:${ex_port_end} \
                                --to-destination ${rule_vs_ipaddr} }					
			        else
                        fw add 4 n ${nat_rule_chains}_${NAT_N_VS_CHAIN} DNAT $ \
                            { -d ${ip} -p ${proto} --dport ${ex_port_start}:${ex_port_end} \
                                --to-destination ${rule_vs_ipaddr}:${in_port_start} }					
			        fi
		        fi
                
                for vs_chain in $vs_filter_chains; do
                    [ -z "$vs_chain" ] && continue
                    local dup_dport=
                    if [ $in_port_start -eq $in_port_end ]; then
                        dup_dport=$in_port_start
                    else
                        dup_dport=${in_port_start}:${in_port_end}
                    fi
                    local dup=$(fw list 4 f $vs_chain \
                        | grep "\-d $rule_vs_ipaddr/32 \-p $proto \-m $proto \--dport ${dup_dport}")
                    [ -z "$dup" ] && {
                        fw add 4 f ${vs_chain} ACCEPT $ \
                            { -p ${proto} --dport ${in_port_start}:${in_port_end} -d ${rule_vs_ipaddr} }
                    }
                done
            }
        }
	}

	if [ "$rule_vs_enable" == "on" ]; then
        nat_syslog 54 "$rule_vs_external_port" "$rule_vs_ipaddr" "$rule_vs_internal_port" "$rule_vs_protocol"
		for i in $(uci_get_state nat core ifaces); do
			if [ "$i" == "$NAT_ZONE_LAN" ]; then
				#TODO:create rule for lan
				echo "No lan rule for vs" >&2
			else
				local ifname=$(uci_get_state nat env ${i}_if)
				local dev=$(uci_get_state nat env ${i}_dev)
				local if_ip=$(uci_get_state nat env ${i}_ip)
				local if_sec_ip=$(uci_get_state nat env ${i}_sec_ip)

				if [ -n "$ifname" -a -n "$dev" ]; then
					if [ -n "$if_ip" -a "$if_ip" != "0.0.0.0" ]; then
						list_contains proto $rule_vs_protocol && {
							case $rule_vs_protocol in
								tcp)
									nat__do_vs_rule "tcp" "$ifname" "$if_ip"
									[ "$if_sec_ip" != "-" -a "$if_sec_ip" != "0.0.0.0" ] && {
										nat__do_vs_rule "tcp" "$dev" "$if_sec_ip"
									}
								;;
								udp)
									nat__do_vs_rule "udp" "$ifname" "$if_ip" 
									[ "$if_sec_ip" != "-" -a "$if_sec_ip" != "0.0.0.0" ] && {
										nat__do_vs_rule "udp" "$dev" "$if_sec_ip"
									}
								;;
								*)
									nat__do_vs_rule "tcp" "$ifname" "$if_ip" 
									nat__do_vs_rule "udp" "$ifname" "$if_ip"

									[ "$if_sec_ip" != "-" -a "$if_sec_ip" != "0.0.0.0" ] && {
										nat__do_vs_rule "tcp" "$dev" "$if_sec_ip"
										nat__do_vs_rule "udp" "$dev" "$if_sec_ip"
									}
								;;
							esac
						}
					fi
				fi
			fi
		done
	fi
	
}

nat_rule_vs_operation() {
    [ -n "$nat_filter_chains" ] && {
        for fc in $nat_filter_chains; do
            local vs=$(echo "$fc"|grep 'vs$')
            [ -n "$vs" ] && {
                append vs_filter_chains $vs
                fw flush 4 f $vs
            }
        done
    }
    fw flush 4 n ${nat_rule_chains}_${NAT_N_VS_CHAIN}

	config_foreach nat_load_rule_vs rule_vs
    unset vs_filter_chains
}


# Copyright (C) 2014-2015 TP-link
. /lib/config/uci.sh

tc_d(){
	#echo "tc $@" > /dev/console
	tc $@
}

fw_config_get_global(){
    fw_config_get_section "$1" global { \
        string enable           "off" \
        string up_band          "" \
        string down_band        "" \
        string high             "60" \
        string middle           "30" \
        string low              "10" \
        string percent          "92" \
        string up_unit          "mbps" \
        string down_unit        "mbps" \
    } || return   
}

fw_config_get_rule(){
    fw_config_get_section "$1" rule { \
        string enable           "" \
        string name             "" \
        string mac              "" \
        string proto            "" \
        string port             "" \
        string ipaddr           "" \
        string phy              "" \
        string priority         "" \
        string app              "" \
        string type             "" \
    } || return
}

fw_load_qos(){
    if [[ x"$(uci_get_state qos core)" != x"qos" ]]; then
        uci_set_state qos core "" qos
    fi
    
    [ -r "/tmp/database.bin" ] || loadqosdatabase
    fw_config_once fw_load_global global 
}

fw_exit_qos(){
    if [[ x"$(uci_get_state qos core)" != x"qos" ]]; then
        uci_set_state qos core "" qos
    fi
    if [[ x"$(uci_get_state qos core loaded)" == x1 ]]; then
        local by_phy=$(uci get profile.@qos[0].by_phy -c "/etc/profile.d")
        local by_app=$(uci get profile.@qos[0].by_app -c "/etc/profile.d")

        # fw_unset_netfilter
        fw_rule_exit
        fw_tc_stop
        # rmmod cls_fw.ko
        # rmmod sch_sfq.ko
        # rmmod sch_htb.ko

        uci_revert_state qos core loaded
        uci_set_state qos core loaded 0

	if [[ "$by_phy" == "on" ]]; then
	    rmmod appid
        fi

	#if [[ $by_phy == "on" ]]; then
            #fw_unset_switch
            # fw_unset_network  need not to do this
        #fi
    fi
}

fw_unset_switch() {
    uci delete switch.@switch_vlan[0]
    uci delete switch.@switch_vlan[0]
    uci delete switch.@switch_vlan[0]
    uci delete switch.@switch_vlan[0]
    uci delete switch.@switch_vlan[0]

    uci add switch switch_vlan
    uci add switch switch_vlan

    uci set switch.@switch_vlan[0].ports="$(nvram get vlan1ports)"
    uci set switch.@switch_vlan[0].device="switch0"
    uci set switch.@switch_vlan[0].vlan="1"

    uci set switch.@switch_vlan[1].ports="$(nvram get vlan2ports)"
    uci set switch.@switch_vlan[1].device="switch0"
    uci set switch.@switch_vlan[1].vlan="2"

    uci_commit_flash switch
   
    /etc/init.d/switch restart
}

fw_rule_exit(){
    fw flush 4 m zone_lan_qos
    fw flush 4 m zone_wan_qos

    fw flush 4 m qos_lan_rule

    fw flush 4 m qos_lan_MIDDLE
    fw flush 4 m qos_lan_HIGH
    fw flush 4 m qos_lan_LOW

    fw flush 4 m qos_wan_HIGH
    fw flush 4 m qos_wan_MIDDLE
    fw flush 4 m qos_wan_LOW
    
    fw del 4 m qos_lan_rule

    fw del 4 m qos_lan_MIDDLE
    fw del 4 m qos_lan_HIGH
    fw del 4 m qos_lan_LOW

    fw del 4 m qos_wan_HIGH
    fw del 4 m qos_wan_MIDDLE
    fw del 4 m qos_wan_LOW
}

fw_unset_network() {
    uci set network.lan.ifname="eth0.1"
    uci_commit_flash network
    
    /etc/init.d/network reload 
}

fw_set_switch() {
    if [[ x"$(uci get switch.@switch_vlan[0].ports)" != x"5 1" ]]; then
        uci delete switch.@switch_vlan[0]
        uci delete switch.@switch_vlan[0]

        uci add switch switch_vlan
        uci add switch switch_vlan
        uci add switch switch_vlan
        uci add switch switch_vlan
        uci add switch switch_vlan

        uci set switch.@switch_vlan[0].ports="5 1"
        uci set switch.@switch_vlan[0].device="switch0"
        uci set switch.@switch_vlan[0].vlan="1"

        uci set switch.@switch_vlan[1].ports="5 2"
        uci set switch.@switch_vlan[1].device="switch0"
        uci set switch.@switch_vlan[1].vlan="2"

        uci set switch.@switch_vlan[2].ports="5 3"
        uci set switch.@switch_vlan[2].device="switch0"
        uci set switch.@switch_vlan[2].vlan="3"

        uci set switch.@switch_vlan[3].ports="5 4"
        uci set switch.@switch_vlan[3].device="switch0"
        uci set switch.@switch_vlan[3].vlan="4"

        uci set switch.@switch_vlan[4].ports="0 5u"
        uci set switch.@switch_vlan[4].device="switch0"
        uci set switch.@switch_vlan[4].vlan="5"

        uci_commit_flash switch
        /etc/init.d/switch restart
    fi
}

fw_set_network() {
    if [[ x"$(uci get network.lan.ifname)" != x"eth0.1 eth0.2 eth0.3 eth0.4" ]]; then
        uci set network.lan.ifname="eth0.1 eth0.2 eth0.3 eth0.4"
        uci_commit_flash network
	  
        /etc/init.d/network reload
    fi
}

fw_set_netfilter() {
    echo 1 > /sys/class/net/br-lan/bridge/nf_call_iptables
    echo 1 > /sys/class/net/br-lan/bridge/nf_call_ip6tables

    #Notice: S99sysctl revert these values to 0
    echo 1 > /proc/sys/net/bridge/bridge-nf-call-iptables
    echo 1 > /proc/sys/net/bridge/bridge-nf-call-ip6tables
}

fw_unset_netfilter() {
    echo 0 > /sys/class/net/br-lan/bridge/nf_call_iptables
    echo 0 > /sys/class/net/br-lan/bridge/nf_call_ip6tables
    echo 0 > /proc/sys/net/bridge/bridge-nf-call-iptables
    echo 0 > /proc/sys/net/bridge/bridge-nf-call-ip6tables
}

fw_rule_load() {
    fw add 4 m qos_lan_HIGH
    fw add 4 m qos_lan_MIDDLE
    fw add 4 m qos_lan_LOW
    fw add 4 m qos_lan_rule

    fw add 4 m qos_wan_HIGH
    fw add 4 m qos_wan_MIDDLE
    fw add 4 m qos_wan_LOW

    local lan_target="MARK --set-xmark 0x1101/0xffff"
    local conn_target="CONNMARK --set-xmark 0x1101/0xffff"
    local wan_target="MARK --set-xmark 0x2101/0xffff"

    fw s_add 4 m qos_lan_HIGH "$lan_target"
    fw s_add 4 m qos_lan_HIGH "$conn_target"
    fw s_add 4 m qos_lan_HIGH ACCEPT
    fw s_add 4 m qos_wan_HIGH "$wan_target"
    fw s_add 4 m qos_wan_HIGH ACCEPT

    lan_target="MARK --set-xmark 0x1102/0xffff"
    conn_target="CONNMARK --set-xmark 0x1102/0xffff"
    wan_target="MARK --set-xmark 0x2102/0xffff"

    fw s_add 4 m qos_lan_MIDDLE "$lan_target"
    fw s_add 4 m qos_lan_MIDDLE "$conn_target"
    fw s_add 4 m qos_lan_MIDDLE ACCEPT
    fw s_add 4 m qos_wan_MIDDLE "$wan_target"
    fw s_add 4 m qos_wan_MIDDLE ACCEPT

    lan_target="MARK --set-xmark 0x1103/0xffff"
    conn_target="CONNMARK --set-xmark 0x1103/0xffff"
    wan_target="MARK --set-xmark 0x2103/0xffff"

    fw s_add 4 m qos_lan_LOW "$lan_target"
    fw s_add 4 m qos_lan_LOW "$conn_target"
    fw s_add 4 m qos_lan_LOW ACCEPT
    fw s_add 4 m qos_wan_LOW "$wan_target"
    fw s_add 4 m qos_wan_LOW ACCEPT

    # wan rules, middle is default
    fw s_add 4 m zone_wan_qos qos_wan_HIGH { "-m connmark --mark 0x1101/0xffff" }
    fw s_add 4 m zone_wan_qos qos_wan_LOW { "-m connmark --mark 0x1103/0xffff" }

    # lan rules, to avoid second match
    fw s_add 4 m zone_lan_qos qos_lan_HIGH { "-m connmark --mark 0x1101/0xffff" }
    #fw s_add 4 m zone_lan_qos qos_lan_MIDDLE { "-m connmark --mark 0x1102/0xffff" }
    fw s_add 4 m zone_lan_qos qos_lan_LOW { "-m connmark --mark 0x1103/0xffff" }
    fw s_add 4 m zone_lan_qos qos_lan_rule

    # set up iptables rules
    config_foreach fw_load_rule rule high
    config_foreach fw_load_rule rule middle
    config_foreach fw_load_rule rule low

    # default qos
    fw s_add 4 m zone_lan_qos qos_lan_MIDDLE
    fw s_add 4 m zone_wan_qos qos_wan_MIDDLE
}

fw_load_global() {

    fw_config_get_global "$1"

    # check profile
    local by_phy=$(uci get profile.@qos[0].by_phy -c "/etc/profile.d")
    local by_app=$(uci get profile.@qos[0].by_app -c "/etc/profile.d")

    if [[ x"$(uci_get_state qos core loaded)" != x1 ]]; then
        if [[ "$global_enable" == "on" ]]; then
            
            syslog $LOG_INF_FUNCTION_ENABLE

            #if [[ "$by_phy" == "on" ]]; then
                #fw_set_switch
                #fw_set_network  need not to do this
                #[ -n "$rule_phy_on" ] && fw_set_netfilter
            #if
            
            if [[ "$by_app" == "on" ]]; then
                insmod appid
                iqos-db-loader /tmp/database.bin
            fi 
            
            if [[ -n "$global_up_band" -a -n "$global_down_band" ]]; then
                fw_tc_start
            fi

            fw_rule_load

            uci_revert_state qos core loaded
            uci_set_state qos core loaded 1
        else
            syslog $LOG_INF_FUNCTION_DISABLE
            uci_revert_state qos core loaded
            uci_set_state qos core loaded 0
        fi
    fi    
}

fw_tc_start() {
    # modules
    # insmod /lib/modules/3.4.0/sch_htb.ko
    #insmod /lib/modules/3.4.0/sch_sfq.ko
    # insmod /lib/modules/3.4.0/cls_fw.ko

    local all_percent=$((${global_high}+${global_middle}+${global_low}))
    global_high=$((${global_high}*100/${all_percent}))
    global_middle=$((${global_middle}*100/${all_percent}))
    global_low=$((${global_low}*100/${all_percent}))

    # paras
    if [[ "$global_up_unit" == "mbps" ]]; then
        global_up_band=$((global_up_band*1024))
    fi
    local uplink=$((${global_percent}*${global_up_band}/100))
    
    if [[ "$global_down_unit" == "mbps" ]]; then
        global_down_band=$((global_down_band*1024))
    fi
    local downlink=$((${global_percent}*${global_down_band}/100))
    

    local up_high=$((${global_high}*${uplink}/100))
    local up_middle=$((${global_middle}*${uplink}/100))
    local up_low=$((${global_low}*${uplink}/100))

    local down_high=$((${global_high}*${downlink}/100))
    local down_middle=$((${global_middle}*${downlink}/100))
    local down_low=$((${global_low}*${downlink}/100))

    # Calculate the burst and cburst parameters for HTB 
    # Added by Jason Guo<guodongxian@tp-link.net>, 20140729 
    local hz=$(cat /proc/net/psched|awk -F ' ' '{print $4}')
    local up_iface_burst down_iface_burst
    local up_burst u_hi_burst u_mid_burst u_lo_burst
    local down_burst d_hi_burst d_mid_burst d_lo_burst 
    [ "$hz" == "3b9aca00" ] && {
        burst__calc() {
            local b=$((${1} * 1000 / 8 / 100))
            b=$((${b} + 1600))
            echo "$b"
        }
        # Uplink, unit bit
        up_burst=$(burst__calc $uplink)
        u_hi_burst=$(burst__calc $up_high)
        u_mid_burst=$(burst__calc $up_middle)
        u_lo_burst=$(burst__calc $up_low)

        # Downlink, unit bit
        down_burst=$(burst__calc $downlink)
        d_hi_burst=$(burst__calc $down_high)
        d_mid_burst=$(burst__calc $down_middle)
        d_lo_burst=$(burst__calc $down_low)

        up_iface_burst=$(burst__calc 1000000)
        down_iface_burst=$(burst__calc 1000000)
        param__convert() {
            local p=
            [ -n "$1" -a -n "$2" ] && {
                p="burst $1 cburst $2"
            }
            echo "$p"        
        }
        
        u_hi_burst=$(param__convert $u_hi_burst $up_burst)
        u_mid_burst=$(param__convert $u_mid_burst $up_burst)
        u_lo_burst=$(param__convert $u_lo_burst $up_burst)
        up_burst=$(param__convert $up_burst $up_burst)

        d_hi_burst=$(param__convert $d_hi_burst $down_burst)
        d_mid_burst=$(param__convert $d_mid_burst $down_burst)
        d_lo_burst=$(param__convert $d_lo_burst $down_burst)
        down_burst=$(param__convert $down_burst $down_burst)

        up_iface_burst=$(param__convert $up_iface_burst $up_iface_burst)
        down_iface_burst=$(param__convert $down_iface_burst $down_iface_burst)
    }
    uplink="$uplink""kbit"
    downlink="$downlink""kbit"

    up_high="$up_high""kbit"
    up_middle="$up_middle""kbit"
    up_low="$up_low""kbit"

    down_high="$down_high""kbit"
    down_middle="$down_middle""kbit"
    down_low="$down_low""kbit"

    echo "up link" $uplink
    echo "down link" $downlink

    local wan_ifname=$(uci get network.wan.ifname)
    # uplink
    tc_d qdisc add dev $wan_ifname root handle 1: htb default 1100
    tc_d class add dev $wan_ifname parent 1: classid 1:1 htb rate "$uplink" ceil "$uplink" $up_burst
    tc_d class add dev $wan_ifname parent 1: classid 1:1100 htb rate 1000000kbit ceil 1000000kbit $up_iface_burst
    tc_d qdisc add dev $wan_ifname parent 1:1100 handle 1100: sfq perturb 10

    tc_d class add dev $wan_ifname parent 1:1 classid 1:1101 htb rate "$up_high" ceil "$uplink" $u_hi_burst
    tc_d qdisc add dev $wan_ifname parent 1:1101 handle 1101: sfq perturb 10

    tc_d class add dev $wan_ifname parent 1:1 classid 1:1102 htb rate "$up_middle" ceil "$uplink" $u_mid_burst
    tc_d qdisc add dev $wan_ifname parent 1:1102 handle 1102: sfq perturb 10

    tc_d class add dev $wan_ifname parent 1:1 classid 1:1103 htb rate "$up_low" ceil "$uplink" $u_lo_burst
    tc_d qdisc add dev $wan_ifname parent 1:1103 handle 1103: sfq perturb 10

    # filter
    tc_d filter add dev $wan_ifname parent 1:0 protocol ip handle 0x1101/0xffff fw classid 1:1101
    tc_d filter add dev $wan_ifname parent 1:0 protocol ip handle 0x1102/0xffff fw classid 1:1102
    tc_d filter add dev $wan_ifname parent 1:0 protocol ip handle 0x1103/0xffff fw classid 1:1103

    # downlink
    tc_d qdisc add dev br-lan root handle 2: htb default 2100
    tc_d class add dev br-lan parent 2: classid 2:2 htb rate "$downlink" ceil "$downlink" $down_burst
    tc_d class add dev br-lan parent 2: classid 2:2100 htb rate 1000000kbit ceil 1000000kbit $down_iface_burst
    tc_d qdisc add dev br-lan parent 2:2100 handle 2100: sfq perturb 10

    tc_d class add dev br-lan parent 2:2 classid 2:2101 htb rate "$down_high" ceil "$downlink" $d_hi_burst
    tc_d qdisc add dev br-lan parent 2:2101 handle 2101: sfq perturb 10

    tc_d class add dev br-lan parent 2:2 classid 2:2102 htb rate "$down_middle" ceil "$downlink" $d_mid_burst
    tc_d qdisc add dev br-lan parent 2:2102 handle 2102: sfq perturb 10

    tc_d class add dev br-lan parent 2:2 classid 2:2103 htb rate "$down_low" ceil "$downlink" $d_lo_burst
    tc_d qdisc add dev br-lan parent 2:2103 handle 2103: sfq perturb 10

    # filter
    tc_d filter add dev br-lan parent 2:0 protocol ip handle 0x2101/0xffff fw classid 2:2101
    tc_d filter add dev br-lan parent 2:0 protocol ip handle 0x2102/0xffff fw classid 2:2102
    tc_d filter add dev br-lan parent 2:0 protocol ip handle 0x2103/0xffff fw classid 2:2103

}

fw_tc_stop(){
    tc_d qdisc del dev "$(uci get network.wan.ifname)" root
    tc_d qdisc del dev br-lan root
}

fw_rule_reload() {
    fw_rule_exit
    fw_config_get_global "$1"

    if [[ "$global_enable" == "on" ]]; then
        fw_rule_load
    fi
}

fw_load_rule() {
    fw_config_get_rule "$1"
    local limit="$2"

    if [[ x"$rule_priority" != x"$limit" ]];
    then
        return
    fi
    
    if [[ "$rule_enable" == 'on' ]]; then
        
        local rule=""
        local target=""
        local ip_rule=""
        local tcp_rule=""
        local udp_rule=""
        local mac_rule=""
        local phy_rule=""
        local icmp_rule=""
        local appid_rule=""

        list_contains rule_type "app" && {
            if [[ -n "$rule_app" ]]; then
                for app in $rule_app; do
                    local tcp_port udp_port

                    config_get custom "$app" custom
                    config_get tcp_port "$app" tcp_port
                    config_get udp_port "$app" udp_port

                    if [[ x"$custom" == x"icmp" ]]; then
                        icmp_rule="-p icmp "
                    fi

                    if [[ x"$custom" == x"appid" ]]; then
                        config_get appid "$app" appid
                        appid_rule="$appid_rule""$appid"','
                    fi
                
                    if [[ -n "$tcp_port" ]]; then
                        tcp_rule="$tcp_rule""$tcp_port"','
                    fi

                    if [[ -n "$udp_port" ]]; then
                        udp_rule="$udp_rule""$udp_port"','
                    fi
                    
                done
            fi
        }

        list_contains rule_type "custom" && {
            if [[ -n "$rule_proto" ]]; then
                case "$rule_proto" in
                    "tcp" )
                        tcp_rule="$tcp_rule""$rule_port"','
                        ;;
                    "udp" )
                        udp_rule="$udp_rule""$rule_port"','
                        ;;
                    "all" )
                        tcp_rule="$tcp_rule""$rule_port"','
                        udp_rule="$udp_rule""$rule_port"','
                        ;;
                esac
            fi
        }

        if [[ -n "$tcp_rule" ]]; then
            tcp_rule=${tcp_rule%,}
            tcp_rule="-p tcp -m multiport --dports "$tcp_rule' '
        fi

        if [[ -n "$udp_rule" ]]; then
            udp_rule=${udp_rule%,}
            udp_rule="-p udp -m multiport --dports "$udp_rule' '
        fi

        if [[ -n "$appid_rule" ]]; then
            appid_rule=${appid_rule%,}
            appid_rule="-m app --id "$appid_rule' '
        fi

        list_contains rule_type "mac" && {
            if [ -n "$rule_mac" ]; then
                local mac=$(echo $rule_mac | tr [a-z] [A-Z])
                mac_rule="-m mac --mac-source $mac "
            fi
        }

        list_contains rule_type "ip" && {
            if [ -n "$rule_ipaddr" ]; then
                ip_rule="-m iprange --src-range ${rule_ipaddr/:/-} "
            fi
        }

        list_contains rule_type "phy" && {
            if [[ -n "$rule_phy" ]]; then
                case "$rule_phy" in
                    "wireless" )
                    phy_rule="-m physdev --physdev-in eth1,eth2"
                       ;;
                    "guest" )
                    phy_rule="-m physdev --physdev-in wl0.1,wl1.1 "
                       ;;
                    "port1" )
                    phy_rule="-m physdev --physdev-in eth0.1 "
                       ;;
                    "port2" )
                    phy_rule="-m physdev --physdev-in eth0.2 "
                       ;;
                    "port3" )
                    phy_rule="-m physdev --physdev-in eth0.3 "
                       ;;
                    "port4" )
                    phy_rule="-m physdev --physdev-in eth0.4 "
                       ;;
                esac
		# rule_phy_on="yes"
            fi
        }

        case "$rule_priority" in
            high )
            lan_target=qos_lan_HIGH
                ;;
            middle )
            lan_target=qos_lan_MIDDLE
                ;;
            low )
            lan_target=qos_lan_LOW
                ;;
        esac

        if [[ -n "$icmp_rule" ]]; then
            rule="${icmp_rule}${ip_rule}${mac_rule}${phy_rule}"
            rule=${rule% }
            echo $rule
            fw s_add 4 m qos_lan_rule "$lan_target" { "$rule" }
        fi

        if [[ -n "$appid_rule" ]]; then
            rule="${ip_rule}${mac_rule}${phy_rule}${appid_rule}"
            rule=${rule% }
            echo $rule
            fw s_add 4 m qos_lan_rule "$lan_target" { "$rule" }
        fi

        if [[ -n "$tcp_rule" ]]; then
            rule="${tcp_rule}${ip_rule}${mac_rule}${phy_rule}"
            rule=${rule% }
            echo $rule
            fw s_add 4 m qos_lan_rule "$lan_target" { "$rule" }
        fi

        if [[ -n "$udp_rule" ]]; then
            rule="${udp_rule}${ip_rule}${mac_rule}${phy_rule}"
            rule=${rule% }
            echo $rule
            fw s_add 4 m qos_lan_rule "$lan_target" { "$rule" }
        fi

        if [[ -z "$tcp_rule" -a -z "$udp_rule" -a -z "$appid_rule" -a -z "$icmp_rule" ]]; then
            rule="${ip_rule}${mac_rule}${phy_rule}"
            rule=${rule% }
            echo $rule
            fw s_add 4 m qos_lan_rule "$lan_target" { "$rule" }
        fi
    fi
}



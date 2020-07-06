# Copyright (C) 2009-2010 OpenWrt.org


fw_config_get_global(){
    fw_config_get_section "$1" global { \
        string enable       "off" \
        string access_mode  "black" \
        string w_https      "pass" \
        string w_httpr      "pass" \
        string dns_filter   "on" \
        string filter_mode  "simple" \
        string dns_forward  "on" \
    } || return   
}

fw_config_get_device(){
    fw_config_get_section "$1" device { \
        string enable   "" \
        string name     "" \
        string mac      "" \
        string calendar "" \
        string note     "" \
    } || return
}

fw_config_get_white_list(){
	fw_config_get_section "$1" white_list { \
		string url "" \
	} || return
}

fw_config_get_black_list(){
	fw_config_get_section "$1" black_list { \
		string key "" \
	} || return
}

fw_load_parental_ctrl(){
	fw_config_once fw_load_global global	
}

fw_exit_parental_ctrl(){

    fw flush 4 f parental_ctrl
    fw flush 4 f parental_ctrl_list
    fw flush 4 f parental_ctrl_block
  
    local rule_tcp="-p tcp -m multiport --dports 80"
    local rule_udp="-p udp -m multiport --dports 53"
    local rule_webserver="-p tcp -m multiport --ports 80,22,20002"
    local target="MARK --set-xmark 0xdead0000/0xffff0000"

    fw s_del 4 f zone_lan parental_ctrl_block { "$rule_webserver" }

    fw s_del 4 f zone_lan_forward parental_ctrl_list { "$rule_tcp" }

    fw s_del 4 f zone_lan_forward parental_ctrl_list { "$rule_udp" }
  
    # check nss custom in profile
    local nss_custom=$(uci get profile.@parental_control[0].nss_custom -c "/etc/profile.d")
    if [[ $nss_custom == "on" ]]; then
       fw s_del 4 f zone_lan_forward "$target"
    fi

    fw s_del 4 f INPUT parental_ctrl_list { "$rule_udp" }

    fw del 4 f parental_ctrl
    fw del 4 f parental_ctrl_list
    fw del 4 f parental_ctrl_block

    fw s_del 4 f FORWARD ACCEPT { "-o br-lan -m conntrack --ctstate RELATED,ESTABLISHED" }
    fw s_add 4 f FORWARD ACCEPT 1 { "-m conntrack --ctstate RELATED,ESTABLISHED" }

    tsched_conf -D parental_control
    tsched_conf -u parental_control
    
}

fw_load_global() {
    fw_config_get_global "$1"

    # tcp 53也是合法的dns报文
    local rule_tcp="-p tcp -m multiport --dports 80"
    local rule_udp="-p udp -m multiport --dports 53"
    
    local rule_webserver="-p tcp -m multiport --ports 80,22,20002"

    fw add 4 f parental_ctrl
    fw add 4 f parental_ctrl_list
    fw add 4 f parental_ctrl_block

    if [[ "$global_enable" == "on" ]]; then
        # fw "<command>" "<family>" "<table>" "<chain>" "<target>" "{" "<rules>" "}"
        
        fw s_del 4 f FORWARD ACCEPT { "-m conntrack --ctstate RELATED,ESTABLISHED" }
        fw s_add 4 f FORWARD ACCEPT 1 { "-o br-lan -m conntrack --ctstate RELATED,ESTABLISHED" }

        if [[ "$global_dns_filter" == "on" ]]; then
            fw s_add 4 f INPUT parental_ctrl_list 1 { "$rule_udp" }
        fi

    
        [ "$global_access_mode" == "white" ] && {
            fw_config_once fw_load_white_list white_list
        }

        [  "$global_access_mode" == "black" ] && {
            fw_config_once fw_load_black_list black_list

            [ "$global_w_https" == "block" ] && {
                rule=$rule",443"       
            }

            [ "$global_w_httpr" == "block" ] && {
                rule=$rule",8080"       
            }
        }
        
        fw s_add i f zone_lan_forward parental_ctrl_list 1 { "$rule_tcp" }
        
        if [[ "$global_dns_filter" == "on" ]]; then
            if [[ "$global_dns_forward" == "on" ]]; then
                fw s_add 4 f zone_lan_forward parental_ctrl_list 1 { "$rule_udp" }
            fi
        fi
        
        fw s_add i f zone_lan parental_ctrl_block 1 { "$rule_webserver" }
        config_foreach fw_load_device device
        syslog $LOG_INF_FUNCTION_ENABLE
    else
        syslog $LOG_INF_FUNCTION_DISABLE
    fi

    conntrack -F
    syslog $LOG_NTC_FLUSH_CT_SUCCESS   
    tsched_conf -u parental_control
}

fw_load_white_list() {
	echo "Loading parental control: white_list mode"

    # check nss custom in profile
    local nss_custom=$(uci get profile.@parental_control[0].nss_custom -c "/etc/profile.d")
    if [[ $nss_custom == "on" ]]; then
        local target="MARK --set-xmark 0xdead0000/0xffff0000"
        fw s_add 4 f parental_ctrl "$target"
    fi
    
	fw_config_get_white_list "$1"

    local urls=${white_list_url// /,}

    urls=$(echo $urls | tr [A-Z] [a-z])

    [ $global_filter_mode == "simple" ] && {
        local rule="-p tcp -m httphost --host $urls --mode white --type http"
        fw s_add 4 f parental_ctrl RETURN { "$rule" }

        if [[ "$global_dns_filter" == "on" ]]; then
            rule="-p udp -m httphost --host $urls --mode white --type dns"
            fw s_add 4 f parental_ctrl RETURN { "$rule" }
        fi
    }

    [ $global_filter_mode == "ac" ] && {
        local rule="-p tcp -m urlfilter --url $urls --mode white --type http"
        fw s_add 4 f parental_ctrl RETURN { "$rule" }

        if [[ "$global_dns_filter" == "on" ]]; then
            rule="-p udp -m urlfilter --url $urls --mode white --type dns"
            fw s_add 4 f parental_ctrl RETURN { "$rule" }
        fi
    }

    fw add 4 f parental_ctrl "REJECT --reject-with tcp-reset" { "-p tcp" }
    fw add 4 f parental_ctrl DROP

}

fw_load_black_list() {
	echo "Loading parental control: black_list mode"

    # check nss custom in profile
    local nss_custom=$(uci get profile.@parental_control[0].nss_custom -c "/etc/profile.d")
    if [[ $nss_custom == "on" ]]; then
        local target="MARK --set-xmark 0xdead0000/0xffff0000"
        fw s_add 4 f parental_ctrl "$target"
    fi

	fw_config_get_black_list "$1"

    local keys=${black_list_key// /,}

    [ $global_filter_mode == "simple" ] && {
        local rule="-p tcp -m httphost --host $keys --mode black --type http"
        fw s_add 4 f parental_ctrl DROP { "$rule" }

        if [[ "$global_dns_filter" == "on" ]]; then
            rule="-p udp -m httphost --host $keys --mode black --type dns"
            fw s_add 4 f parental_ctrl DROP { "$rule" }
        fi
    }

    [ $global_filter_mode == "ac" ] && {
        local rule="-p tcp -m urlfilter --url $keys --mode black --type http"
        fw s_add 4 f parental_ctrl DROP { "$rule" }

        if [[ "$global_dns_filter" == "on" ]]; then
            rule="-p udp -m urlfilter --url $keys --mode black --type dns"
            fw s_add 4 f parental_ctrl DROP { "$rule" }
        fi
    }

    fw add 4 f parental_ctrl RETURN
}

fw_load_device() {
	fw_config_get_device "$1"
    local mac=$(echo $device_mac | tr [a-z] [A-Z])

    if [ $device_enable == 'on' ]; then
         fw s_add 4 f parental_ctrl_block DROP { "-m mac --mac-source ${mac//-/:}" }
        tsched_conf -a parental_control "${mac//:/-}" "$device_calendar"
    fi
}

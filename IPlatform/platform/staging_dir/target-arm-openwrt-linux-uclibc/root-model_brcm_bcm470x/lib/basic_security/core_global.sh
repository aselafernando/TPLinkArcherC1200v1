# Copyright (C) 2009-2010 OpenWrt.org

fw_config_get_global() {
	fw_config_get_section "$1" global { \
		string spi_enable 			"on" \
		string wan_ping 			"on" \
		string lan_ping 			"on" \
		string remote_access_brlan 	"off" \
		string lan_ip_limit 		"on" \
	} || return
}


fw_load_all() {
	fw_config_once fw_load_global global
}

fw_exit_all() {

	local icmp="-p icmp -m icmp --icmp-type 8"

	fw s_del 4 f zone_wan_forward zone_wan_ACCEPT
	fw s_add 4 f zone_wan_forward zone_wan_DROP

	fw s_del 4 f zone_lan DROP { "$icmp" }

	fw s_del 4 f zone_wan ACCEPT { "$icmp" }

	fw s_del 4 f zone_wan zone_wan_br_protection
	fw del 4 f zone_wan_br_protection
	
	fw s_del 4 f zone_wan ACCEPT { "-p gre" }

	fw flush 4 f zone_lan_ip_limit
	fw s_del 4 f zone_lan_forward zone_lan_ip_limit
	fw del 4 f zone_lan_ip_limit
}


fw_load_global() {
	fw_config_get_global $1
	local icmp="-p icmp -m icmp --icmp-type 8"

	local br_lan

	config_load "network"
	config_get br_lan 'lan' ipaddr ""
	config_get mask   'lan' netmask ""

	fw add 4 f zone_wan_br_protection
	fw add 4 f zone_lan_ip_limit
	fw add 4 f zone_wan_vpn_access
	fw add 4 f forwarding_rule_vpn

	# # pptp fix 
	# # todo: modify
	fw s_add 4 f zone_wan ACCEPT 1 { "-p gre" }
	# vpn pass
	fw s_add 4 f zone_wan zone_wan_vpn_access 1
	fw s_add 4 f forwarding_rule forwarding_rule_vpn 1
	# #bridge pass
	fw s_add i f forwarding_rule ACCEPT 1 { "-m physdev --physdev-is-bridged" }
	
	case $global_spi_enable in
		off )
			fw s_del 4 f zone_wan_forward zone_wan_DROP
			fw s_add 4 f zone_wan_forward zone_wan_ACCEPT
		;;
		on )
			fw s_del 4 f zone_wan_forward zone_wan_ACCEPT
			fw s_add 4 f zone_wan_forward zone_wan_DROP
		;;
	esac

	case $global_wan_ping in
		on )
			fw s_add 4 f zone_wan ACCEPT 1 { "$icmp" }
		;;
		off )
			fw s_del 4 f zone_wan ACCEPT { "$icmp" }
		;;
	esac

	case $global_lan_ping in
		on )
			fw s_del 4 f zone_lan DROP { "$icmp" }
		;;
		off )
			fw s_add 4 f zone_lan DROP 1 { "$icmp" }
		;;
	esac

	case $global_remote_access_brlan in
		on )
			fw flush 4 f zone_wan_br_protection
		;;
		off )
			fw s_add 4 f zone_wan_br_protection DROP { "-d ${br_lan}/32" }
		;;
	esac

	case $global_lan_ip_limit in
		on )
			if [[ $br_lan ]]; then
				if [[ $mask ]]; then
					fw flush 4 f zone_lan_ip_limit
					fw s_add 4 f zone_lan_forward zone_lan_ip_limit 1
			
					# fw s_add 4 f zone_lan_ip_limit RETURN { "-s 0.0.0.0/32" }
					fw s_add 4 f zone_lan_ip_limit DROP { "! -s ${br_lan}/${mask}" }
				fi
			fi
			
		;;
		off )
			fw s_del 4 f zone_lan_forward zone_lan_ip_limit
	
			fw flush 4 f zone_lan_ip_limit
		;;
	esac

	# zone_wan_br_protection should be first
	fw s_add 4 f zone_wan zone_wan_br_protection 1
	
	#we should let icmpv6 packet passthrough ,by yaopeiyong
	fw s_del 6 f zone_wan zone_wan_DROP
	fw s_add 6 f zone_wan ACCEPT { "-p udp -m udp --dport 546" }
	fw s_add 6 f zone_wan ACCEPT { "-p udp -m udp --dport 547" }
	fw s_add 6 f zone_wan ACCEPT { "-p ipv6-icmp" }
	fw s_add 6 f zone_wan zone_wan_DROP
	
	# flush ct
	conntrack -F
	syslog $LOG_NTC_FLUSH_CT_SUCCESS
}

fw_update_br_lan() {

	fw_config_get_global global

	config_load "network"
	config_get br_lan 'lan' ipaddr ""
	config_get mask   'lan' netmask ""

	case $global_remote_access_brlan in
		on )
		
			fw flush 4 f zone_wan_br_protection
		
		;;
		off )
			fw flush 4 f zone_wan_br_protection
			fw s_add 4 f zone_wan_br_protection DROP { "-d ${br_lan}/32" }

		;;
	esac

	case $global_lan_ip_limit in
		on )
			if [[ $br_lan ]]; then
				if [[ $mask ]]; then
					fw flush 4 f zone_lan_ip_limit
					fw s_add 4 f zone_lan_forward zone_lan_ip_limit 1
			
					# fw s_add 4 f zone_lan_ip_limit RETURN { "-s 0.0.0.0/32" }
					fw s_add 4 f zone_lan_ip_limit DROP { "! -s ${br_lan}/${mask}" }
				fi
			fi
			
		;;
		off )
			fw s_del 4 f zone_lan_forward zone_lan_ip_limit
		
			fw flush 4 f zone_lan_ip_limit
		;;
	esac
}


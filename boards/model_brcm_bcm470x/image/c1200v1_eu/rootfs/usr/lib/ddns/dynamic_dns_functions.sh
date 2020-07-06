# /usr/lib/dynamic_dns/dynamic_dns_functions.sh
#
# Written by Eric Paul Bishop, Janary 2008
# Distributed under the terms of the GNU General Public License (GPL) version 2.0
#
# This script is (loosely) based on the one posted by exobyte in the forums here:
# http://forum.openwrt.org/viewtopic.php?id=14040



. /lib/functions.sh
. /lib/functions/network.sh
# TP LOG
. /usr/lib/ddns/dynamic_dns_log.sh
# Dyndns.com no-ip.com
. /usr/lib/ddns/dynamic_dns_dyndns.sh
. /usr/lib/ddns/dynamic_dns_noip.sh

#loads all options for a given package and section
#also, sets all_option_variables to a list of the variable names
load_all_config_options()
{
	pkg_name="$1"
	section_id="$2"

	ALL_OPTION_VARIABLES=""
	# this callback loads all the variables
	# in the section_id section when we do
	# config_load. We need to redefine
	# the option_cb for different sections
	# so that the active one isn't still active
	# after we're done with it.  For reference
	# the $1 variable is the name of the option
	# and $2 is the name of the section
	config_cb()
	{
		if [ ."$2" = ."$section_id" ]; then
			option_cb()
			{
				ALL_OPTION_VARIABLES="$ALL_OPTION_VARIABLES $1"
			}
		else
			option_cb() { return 0; }
		fi
	}


	config_load "$pkg_name"
	for var in $ALL_OPTION_VARIABLES
	do
		config_get "$var" "$section_id" "$var"
	done
}


get_current_ip()
{

	#if ip source is not defined, assume we want to get ip from wan 
	if [ "$ip_source" != "interface" ] && [ "$ip_source" != "web" ] && [ "$ip_source" != "script" ]
	then
		ip_source="network"
	fi

	if [ "$ip_source" = "network" ]
	then
		if [ -z "$ip_network" ]
		then
			ip_network="wan"
		fi
	fi

	current_ip='';
	if [ "$ip_source" = "network" ]
	then
		network_get_ipaddr current_ip "$ip_network" || return
	elif [ "$ip_source" = "interface" ]
	then
		current_ip=$(ifconfig $ip_interface | grep -o 'inet addr:[0-9.]*' | grep -o "$ip_regex")
	elif [ "$ip_source" = "script" ]
	then
		# get ip from script
		current_ip=$($ip_script)
	else
		# get ip from web
		# we check each url in order in ip_url variable, and if no ips are found we use dyndns ip checker
		# ip is set to FIRST expression in page that matches the ip_regex regular expression
		for addr in $ip_url
		do
			if [ -z "$current_ip" ]
			then
				current_ip=$(echo $( curl -o - $addr 2>/dev/null) | grep -o "$ip_regex")
			fi
		done

		#here we hard-code the dyndns checkip url in case no url was specified
		if [ -z "$current_ip" ]
		then
			#current_ip=$(echo $( curl -o - http://checkip.dyndns.org 2>/dev/null) | grep -o "$ip_regex")
			current_ip = "0.0.0.0"
		fi
	fi

	echo "$current_ip"
}


verbose_echo()
{
	if [ "$verbose_mode" = 1 ]
	then
		echo $1
	fi
}

check_iface_conn()
{
	local wanIf="$1"
	local service="$2"

    config_get type "wan" wan_type
    local interface physlink ifc
    case $type in
        static|dhcp) interface="wan" ;;
        *) interface="internet" ;;
    esac
    #network_get_ipaddr ipaddr $interface
    #[ -z "$ipaddr" -o "$ipaddr" = "0.0.0.0" ] && return 1
    [ "$wanIf" != "$interface" ] && return 1
    uci_revert_state ddns "$service"

    network_get_physdev ifc $interface
	network_get_link physlink $ifc
	[ -z "$physlink" -o "$physlink" = 0 ] && return 1

    return 0
}

start_daemon_for_all_ddns_sections()
{
    lock /var/run/ddns.lock
	local event_interface="$1"
    local service="$2"

	SECTIONS=""
	config_cb() 
	{
		SECTIONS="$SECTIONS $2"
	}
	config_load "ddns"
    # Load network config, and revert state, Guo Dongxian, April 04 2014    
    config_load "network"

    ! check_iface_conn "$event_interface" "$service" && {
        echo "Wan is not connected or interface is invaild."
        lock -u /var/run/ddns.lock
        return
    }
    lock -u /var/run/ddns.lock
	for section in $SECTIONS
	do
		local iface
		config_get iface "$section" interface "wan"
		[ "$iface" = "$event_interface" ] || continue
        [ -z "$service" ] || [ "$service" = "$section" ] || continue
		/usr/lib/ddns/dynamic_dns_updater.sh $section 0 > /dev/null 2>&1 &
		sleep 1
	done
}

monotonic_time()
{
	local uptime
	read uptime < /proc/uptime
	echo "${uptime%%.*}"
}

ddns_clear_state()
{
    [ -n "$1" ] && {
        uci_revert_state ddns "$1"
        return 0
    }
    
    uci_revert_state ddns
    return 0
}

ddns_state_good()
{
    local service="$1"
    
    [ -z "$service" ] && return 1
            
    local state=$(uci_get_state ddns $service state)
    
    [ $state -eq 0 ] && return 0
    
    return 1
}

ddns_get_external_ip()
{
    external_ip="0.0.0.0"
    config_get type "wan" wan_type
    if [ -n "$type" ] 
    then
        case $type in
            static|dhcp) # Maybe NAT behind no external ip
                external_ip=$(echo $( curl -o - http://checkip.dyndns.org 2>/dev/null) | grep -o "$ip_regex" )
                [ -z "$external_ip" ] && {
                    external_ip="0.0.0.0"
                    ddns_syslog $DDNS_LOG_WNG_GET_EXIP_FAIL "checkip.dyndns.org"
                }
            ;;
            *) external_ip="0.0.0.0" ;;
        esac
    fi
    
    echo "$external_ip"
}

ddns_parse_retcode()
{
    local service="$1"

    ${service}_parse_retcode $service
}

ddns_set_state()
{
    local service="$1"
    
    shift
    ${service}_set_state "$1"      
}

ddns_update_check()
{
    local service="$1"
    local ip="$2"
    local iface

    config_get type "wan" wan_type
    case $type in
        static|dhcp) iface="wan" ;;
        *) iface="internet" ;;
    esac

    ${service}_check "$iface" "$ip"     
}

ddns_retry_interval()
{
    local service="$1"

    ddns_retry_int=''
    ddns_retry_int=$(${service}_retry_interval)
 
    echo "$ddns_retry_int"
}


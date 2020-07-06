#!/bin/sh
# Copyright(c) 2011-2013 Shenzhen TP-LINK Technologies Co.Ltd.
#
# /usr/lib/dnsproxy/dnsproxy_api.sh
# Written by Guo Dongxian<guodongxian@tp-link.com.cn>, April 2014
#
# Implement and Provide api for dnsproxy module 
# version 1.0.1

. /lib/functions.sh
. /lib/functions/network.sh

dnsproxy_module="dnsproxy"

dnsproxy_config_append()
{
    CONFIG_APPEND=1 config_load "$@"
	unset CONFIG_APPEND
}

dnsproxy_anet()
{
    echo "4"
}

dnsproxy_enable()
{
    config_get en "$dnsproxy_module" enable
    
    [ "$en" == "on" ] && return 0
    
    return 1
}

dnsproxy_get_check_int()
{
    config_get chk_int "$dnsproxy_module" check_interval
    echo "$chk_int"
}

dnsproxy_get_retry_times()
{
    config_get retries "$dnsproxy_module" retry_times
    echo "$retries"
}

dnsproxy_get_timeout()
{
    config_get tmo "$dnsproxy_module" query_timeout
    echo "$tmo"
}

dnsproxy_get_domains()
{
    config_get domains "$dnsproxy_module" domains
    echo "$domains"
}

dnsproxy_get_dns_servers()
{
    local iface=

    config_get type "wan" wan_type
    case $type in
        static|dhcp) iface=wan ;;
        *) iface=internet ;;    
    esac
    network_get_dnsserver dns_svrs $iface
    echo "$dns_svrs"
}

dnsproxy_force_check()
{
    local rule_num=$(fw list 4 n prerouting_rule_dns|wc -l)

    rule_num=$((rule_num - 1))

    [ $rule_num -eq 1 ] && return 1

    return 0
}

dnsproxy_start_deamon()
{
    local interface="$1"
    #local iface=  
  
    #config_get type "wan" wan_type
    #case $type in
    #    static|dhcp) iface=wan ;;
    #    *) iface=internet ;;    
    #esac
    
    #[ "$interface" = "$iface" ] && {
    local anet=$(dnsproxy_anet)
    case $anet in
        4) nice -n 20 /usr/lib/dnsproxy/dnsproxy_deamon.sh > /dev/null 2>&1 & ;;
        6) echo "Not support ipv6 now" ;;
        *) echo "Unknown ANET" ;;
    esac
    #}
}

